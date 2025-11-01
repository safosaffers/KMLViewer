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
  qreal scaleX = sqrt(transformViewport.m11() * transformViewport.m11() +
                      transformViewport.m12() * transformViewport.m12());
  qreal penWidth = MIN_PEN_WIDTH;
  qreal scaledPenWidth = LINE_WIDTH_RATIO / scaleX;
  if (scaledPenWidth > MIN_PEN_WIDTH) {
    penWidth = scaledPenWidth;
  }
  painter.setPen(QPen(color, penWidth));
}
void OpenGLWidget::setBrushWithAlpha(QPainter& painter, QColor color,
                                     qreal alpha) {
  QColor br = color;
  br.setAlphaF(alpha);
  painter.setBrush(br);
}
void OpenGLWidget::drawPolygons(QPainter& painter,
                                const QList<PolygonPair>& polygons,
                                const QColor& color) {
  setPenWidthAccordingToViewport(painter, color);
  setBrushWithAlpha(painter, color, 0.5);
  for (const PolygonPair& pairPoly : polygons) {
    painter.drawPolygon(pairPoly.second, Qt::WindingFill);
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
void OpenGLWidget::normalizePolygons(QPointF maxCoord) {
  qreal maxCoordVal = qMax(maxCoord.x(), maxCoord.y());
  for (PolygonPair& poly : polygons) {
    for (QPointF& point : poly.second) {
      point /= maxCoordVal;
    }
  }
  this->maxCoord = maxCoord / maxCoordVal;
}
void OpenGLWidget::setPolygons(QList<PolygonPair> polygons) {
  this->polygons = polygons;
}
void OpenGLWidget::resetSimplifiedPolygons() {
  this->simplifiedPolygons.clear();
}
void OpenGLWidget::setSimplifiedPolygons(
    QList<PolygonPair> simplifiedPolygons) {
  this->simplifiedPolygons = simplifiedPolygons;
}
QTransform OpenGLWidget::setInitialViewport() {
  QTransform t;
  int emptyPercent = 20;
  qreal emptySpaceX = this->maxCoord.x() * emptyPercent / 100;
  qreal emptySpaceY = this->maxCoord.y() * emptyPercent / 100;
  scaleViewport =
      qMin(width(), height()) / (qMax(this->maxCoord.x(), this->maxCoord.y()) +
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
