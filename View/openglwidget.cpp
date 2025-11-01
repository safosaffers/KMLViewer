#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
  isPanning = false;
  normalizeFactor = 1;
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
                                     const QList<QPolygonF>& polygons,
                                     const QColor& color) {
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
  setPenForPoints(painter, color);
  painter.setBrush(Qt::NoBrush);
  painter.drawPath(path);
  painter.restore();
}
void OpenGLWidget::drawPolygons(QPainter& painter,
                                const QList<QPolygonF>& polygons,
                                const QColor& color) {
  setBrushWithAlpha(painter, color, 0.5);
  for (const QPolygonF& poly : polygons) {
    setPenForEdges(painter, color);
    painter.drawPolygon(poly, Qt::WindingFill);

    setPenForPoints(painter, Qt::red);
  }

  drawVertexMarkers(painter, polygons, Qt::red);
}
void OpenGLWidget::paintGL() {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), Qt::white);

  painter.setWorldTransform(transformViewport);

  drawPolygons(painter, polygons, Qt::black);
  drawPolygons(painter, simplifiedPolygons, Qt::green);

  painter.end();
}
void OpenGLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}

void OpenGLWidget::setPolygons(const QList<QPolygonF>& polygons) {
  this->polygons = polygons;
}
void OpenGLWidget::resetSimplifiedPolygons() {
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
