#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
  isPanning = false;
  normalizeFactor = 1;
  selectedPolygonId = -1;  // No polygon selected initially
}
OpenGLWidget::~OpenGLWidget() {}
void OpenGLWidget::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(1.f, 1.f, 1.f, 1.f);
}

void OpenGLWidget::setPenForEdges(QPainter& painter, QColor color) {
  qreal scaleX = sqrt(transformViewport.m11() * transformViewport.m11() +
                      transformViewport.m12() * transformViewport.m12());
  qreal penWidth = MIN_PEN_WIDTH;
  qreal scaledPenWidth = LINE_WIDTH_RATIO / scaleX;
  if (scaledPenWidth > MIN_PEN_WIDTH) {
    penWidth = scaledPenWidth;
  }
  painter.setPen(QPen(color, penWidth));
}
void OpenGLWidget::setPenForPoints(QPainter& painter, QColor color) {
  qreal scaleX = qSqrt(transformViewport.m11() * transformViewport.m11() +
                       transformViewport.m12() * transformViewport.m12());
  qreal penWidth = MIN_PEN_WIDTH;
  qreal scaledPenWidth = LINE_WIDTH_RATIO / scaleX;
  if (scaledPenWidth > MIN_PEN_WIDTH) {
    penWidth = scaledPenWidth;
  }
  penWidth *= 4;
  painter.setPen(QPen(color, penWidth, Qt::SolidLine, Qt::RoundCap));
}
void OpenGLWidget::setBrushWithAlpha(QPainter& painter, QColor color,
                                     qreal alpha) {
  QColor br = color;
  br.setAlphaF(alpha);
  painter.setBrush(br);
}

void OpenGLWidget::drawVertexMarkers(QPainter& painter,
                                     const QList<QPolygonF>& polygons) {
  if (polygons.isEmpty()) return;

  qreal scaleX = qSqrt(transformViewport.m11() * transformViewport.m11() +
                       transformViewport.m12() * transformViewport.m12());
  if (scaleX >= 1e+6) scaleX = 1e+6;
  if (scaleX <= 1e-6) scaleX = 1e-6;
  const qreal worldLength = 1.0 / scaleX;

  QPainterPath path;
  for (const auto& poly : polygons) {
    for (const auto& pt : poly) {
      path.moveTo(pt);
      path.lineTo(pt.x() + worldLength, pt.y());
    }
  }

  painter.save();
  painter.setBrush(Qt::NoBrush);
  painter.drawPath(path);
  painter.restore();
}
void OpenGLWidget::drawPolygons(QPainter& painter,
                                const QList<QPolygonF>& polygons,
                                const QColor& colorPoly,
                                const QColor& colorPoints) {
  setBrushWithAlpha(painter, colorPoly, 0.5);
  for (const QPolygonF& poly : polygons) {
    setPenForEdges(painter, colorPoly);
    painter.drawPolygon(poly, Qt::WindingFill);
  }

  setPenForPoints(painter, colorPoints);
  drawVertexMarkers(painter, polygons);
}
void OpenGLWidget::paintGL() {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), Qt::white);

  painter.setWorldTransform(transformViewport);

  // Draw regular polygons
  if (!polygons.isEmpty()) {
    if (selectedPolygonId >= 0 && selectedPolygonId < polygons.size()) {
      // Draw non-selected polygons normally
      for (int i = 0; i < polygons.size(); ++i) {
        if (i == selectedPolygonId) {
          // Skip for now, draw selected polygon last to have it on top
          continue;
        }
        QPolygonF poly = polygons[i];
        drawPolygons(painter, QList<QPolygonF>() << poly, Qt::black, Qt::red);
      }
      // Draw selected polygon with unique color
      if (selectedPolygonId < polygons.size()) {
        QPolygonF selectedPoly = polygons[selectedPolygonId];
        drawPolygons(painter, QList<QPolygonF>() << selectedPoly,
                     SELECTED_POLYGON_COLOR, Qt::red);
      }
    } else {
      // No polygon selected, draw all normally
      drawPolygons(painter, polygons, Qt::black, Qt::red);
    }
  }

  // Draw simplified polygons
  if (!simplifiedPolygons.isEmpty()) {
    if (selectedPolygonId >= 0 &&
        selectedPolygonId < simplifiedPolygons.size()) {
      // Draw non-selected simplified polygons normally
      for (int i = 0; i < simplifiedPolygons.size(); ++i) {
        if (i == selectedPolygonId) {
          // Skip for now, draw selected polygon last to have it on top
          continue;
        }
        QPolygonF poly = simplifiedPolygons[i];
        drawPolygons(painter, QList<QPolygonF>() << poly, Qt::green,
                     QColor(qRgb(48, 106, 42)));
      }
      // Draw selected simplified polygon with unique color
      if (selectedPolygonId < simplifiedPolygons.size()) {
        QPolygonF selectedPoly = simplifiedPolygons[selectedPolygonId];
        drawPolygons(painter, QList<QPolygonF>() << selectedPoly,
                     SELECTED_POLYGON_COLOR, QColor(qRgb(48, 106, 42)));
      }
    } else {
      // No polygon selected, draw all normally
      drawPolygons(painter, simplifiedPolygons, Qt::green,
                   QColor(qRgb(48, 106, 42)));
    }
  }

  painter.end();
}
void OpenGLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void OpenGLWidget::setPolygons(const QList<QPolygonF>& polygons) {
  this->polygons = polygons;
}
void OpenGLWidget::clearPolygons() { this->polygons.clear(); }
void OpenGLWidget::clearSimplifiedPolygons() {
  this->simplifiedPolygons.clear();
}
void OpenGLWidget::setSimplifiedPolygons(
    const QList<QPolygonF>& simplifiedPolygons) {
  this->simplifiedPolygons = simplifiedPolygons;
}
QTransform OpenGLWidget::setInitialViewport(QPointF maxCoord) {
  QTransform t;
  int emptyPercent = 20;
  qreal emptySpaceX = maxCoord.x() * emptyPercent / 100;
  qreal emptySpaceY = maxCoord.y() * emptyPercent / 100;
  scaleViewport = qMin(width(), height()) / (qMax(maxCoord.x(), maxCoord.y()) +
                                             qMax(emptySpaceX, emptySpaceY));
  t.scale(scaleViewport, scaleViewport);
  t.translate(emptySpaceX / 2, emptySpaceY / 2);

  QTransform swapY;
  swapY.translate(0, height());
  swapY.scale(1, -1);

  t *= swapY;
  initialTransformViewport = t;
  transformViewport = initialTransformViewport;
  minAllowedScale = initialTransformViewport.m11() * MIN_ZOOM_FACTOR;

  return initialTransformViewport;
}
void OpenGLWidget::wheelEvent(QWheelEvent* event) {
  double angle = event->angleDelta().y();
  if (!transformViewport.isInvertible() || qAbs(angle) < 1e-6) {
    event->accept();
    return;
  }
  QPointF cursorPos = event->position();
  QTransform inv;

  inv = transformViewport.inverted();
  QPointF worldCursor = inv.map(cursorPos);

  double factor = 1.0;
  factor = 1 + (angle > 0 ? 0.1 : -0.1);

  double currentScale = transformViewport.m11();
  // qDebug() << "currentScale = " << currentScale;

  double newScale = currentScale * factor;
  // qDebug() << "newScale = " << newScale;
  if (newScale < minAllowedScale) {  // || newScale > maxAllowedScale
    event->accept();
    return;
  }

  // === Apply zooming relative to the cursor ===
  // So that the point under the cursor remains in place after the zoom:
  // 1. Shift the coordinate system so that the cursor is at (0, 0)
  // 2. Apply the zoom
  // 3. We return the system back
  transformViewport.translate(worldCursor.x(), worldCursor.y());
  transformViewport.scale(factor, factor);
  transformViewport.translate(-worldCursor.x(), -worldCursor.y());

  update();
  event->accept();
}

void OpenGLWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    isPanning = true;
    lastMousePos = event->pos();
    setCursor(Qt::ClosedHandCursor);
  }
  event->accept();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent* event) {
  if (isPanning) {
    QPointF currentScreenPos = event->position();
    QPointF lastScreenPos = lastMousePos;

    QTransform inv = transformViewport.inverted();
    QPointF currentWorld = inv.map(currentScreenPos);
    QPointF lastWorld = inv.map(lastScreenPos);

    QPointF worldDelta = lastWorld - currentWorld;

    transformViewport.translate(-worldDelta.x(), -worldDelta.y());

    lastMousePos = event->pos();
    update();
  }
  event->accept();
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    isPanning = false;
    setCursor(Qt::ArrowCursor);
  }
  event->accept();
}

void OpenGLWidget::setSelectedPolygonId(int id) { selectedPolygonId = id; }

void OpenGLWidget::centerOnPolygon(int id) {
  if (id < 0) return;  // No polygon to center on

  QPolygonF poly;

  // Check if we should use regular polygons or simplified polygons
  if (!simplifiedPolygons.isEmpty() && id < simplifiedPolygons.size()) {
    poly = simplifiedPolygons[id];
  } else if (id < polygons.size()) {
    poly = polygons[id];
  } else {
    return;  // Invalid polygon id
  }

  if (poly.isEmpty()) return;

  // Calculate bounding rectangle of the polygon
  QRectF boundingRect = poly.boundingRect();

  // Get the current widget size
  qreal widgetWidth = width();
  qreal widgetHeight = height();

  // Calculate scale to fit the bounding rectangle with some margin
  qreal marginPercent = 20; // 20% margin around the polygon
  qreal marginX = boundingRect.width() * marginPercent / 100;
  qreal marginY = boundingRect.height() * marginPercent / 100;
  
  qreal scaleX = (widgetWidth - 2 * marginX) / boundingRect.width();
  qreal scaleY = (widgetHeight - 2 * marginY) / boundingRect.height();
  
  // Take the minimum scale to ensure the whole polygon fits in the view
  qreal calculatedScale = qMin(scaleX, scaleY);
  
  // The calculatedScale is in "pixels per world unit", 
  // but we need to calculate the actual scale factor relative to initialTransformViewport
  // which already contains an initial scale factor
  qreal initialScale = initialTransformViewport.m11();  // Get initial scale from the initial transform
  
  // Calculate relative scale factor to apply to the initial transform
  qreal newScale = calculatedScale / initialScale;
  
  // Calculate new translation to center the bounding rectangle
  QPointF polygonCenter = boundingRect.center();
  QPointF widgetCenter(widgetWidth / 2.0, widgetHeight / 2.0);
  
  // Start with initial transform and apply the relative scale
  transformViewport = initialTransformViewport;
  transformViewport.scale(newScale, newScale);
  
  // Apply translation to center the polygon in the view
  QPointF currentWidgetCenter = transformViewport.inverted().map(widgetCenter);
  QPointF translation = currentWidgetCenter - polygonCenter;
  transformViewport.translate(translation.x(), translation.y());

  update();  // Trigger repaint
}
