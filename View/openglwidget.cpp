#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
  isPanning = false;
}
OpenGLWidget::~OpenGLWidget() {}
void OpenGLWidget::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(1.f, 1.f, 1.f, 1.f);
}
void OpenGLWidget::setPenWidthAccordingToViewport(QPainter& painter,
                                                  QColor color) {
  QRectF viewportRect(0, 0, width(), height());
  QRectF logicalBB = transformViewport.inverted().mapRect(viewportRect);
  qreal penWidth =
      qMax(logicalBB.width(), logicalBB.height()) / LINE_WIDTH_RATIO;
  painter.setPen(QPen(color, penWidth));
}
void OpenGLWidget::setBrushWithAlpha(QPainter& painter, QColor color,
                                     qreal alpha) {
  QColor br = color;
  br.setAlphaF(alpha);
  painter.setBrush(br);
}
void OpenGLWidget::drawPolygons(QPainter& painter,
                                const QList<QPolygonF>& polygons,
                                const QColor& color) {
  setPenWidthAccordingToViewport(painter, color);
  setBrushWithAlpha(painter, color, 0.5);
  for (const QPolygonF& poly : polygons) {
    painter.drawPolygon(poly, Qt::WindingFill);
  }
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
void OpenGLWidget::setPolygons(QList<QPolygonF> polygons) {
  this->polygons = polygons;
}
void OpenGLWidget::resetSimplifiedPolygons() {
  this->simplifiedPolygons.clear();
}
void OpenGLWidget::setSimplifiedPolygons(QList<QPolygonF> simplifiedPolygons) {
  this->simplifiedPolygons = simplifiedPolygons;
}
QTransform OpenGLWidget::setInitialViewport(QPointF Max) {
  QTransform t;
  int emptyPercent = 20;
  qreal emptySpaceX = Max.x() * emptyPercent / 100;
  qreal emptySpaceY = Max.y() * emptyPercent / 100;
  scaleViewport = qMin(width(), height()) /
                  (qMax(Max.x(), Max.y()) + qMax(emptySpaceX, emptySpaceY));
  t.scale(scaleViewport, scaleViewport);
  t.translate(emptySpaceX / 2, emptySpaceY / 2);

  QTransform swapY;
  swapY.translate(0, height());
  swapY.scale(1, -1);

  t *= swapY;
  initialTransformViewport=t;
  transformViewport = initialTransformViewport;
  minAllowedScale = initialTransformViewport.m11() * MIN_ZOOM_FACTOR;
  maxAllowedScale = initialTransformViewport.m11() * MAX_ZOOM_FACTOR;

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
  if (newScale < minAllowedScale || newScale > maxAllowedScale) {
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
