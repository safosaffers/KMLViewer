#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
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
  int emptyPercent = 20;
  qreal emptySpaceX = Max.x() * emptyPercent / 100;
  qreal emptySpaceY = Max.y() * emptyPercent / 100;
  scaleViewport = qMin(width(), height()) /
                  (qMax(Max.x(), Max.y()) + qMax(emptySpaceX, emptySpaceY));
  initialTransformViewport.scale(scaleViewport, scaleViewport);
  initialTransformViewport.translate(emptySpaceX / 2, emptySpaceY / 2);

  QTransform swapY;
  swapY.translate(0, height());
  swapY.scale(1, -1);

  initialTransformViewport *= swapY;
  transformViewport = initialTransformViewport;
  return initialTransformViewport;
}
