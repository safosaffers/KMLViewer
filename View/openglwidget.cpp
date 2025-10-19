#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
}
OpenGLWidget::~OpenGLWidget() {}
void OpenGLWidget::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(1.f, 1.f, 1.f, 1.f);
}
void OpenGLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.setPen(QPen(Qt::black, 1));
  QColor br = Qt::black;
  br.setAlphaF(0.5);
  painter.setBrush(br);

  painter.setWorldTransform(transformViewport);

  for (QPolygonF& poly : polygons) {
    painter.drawPolygon(poly, Qt::WindingFill);
  }

  painter.end();
}
void OpenGLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
  updateViewport();
}
void OpenGLWidget::setPolygons(QList<QPolygonF> polygons) {
  this->polygons = polygons;
}
QTransform OpenGLWidget::updateViewport() {
  QTransform t;
  double emptyPercent = 20;
  double emptySpaceX = maxCorner.x() * emptyPercent / 100;
  double emptySpaceY = maxCorner.y() * emptyPercent / 100;
  double scale =
      (qMax(maxCorner.x(), maxCorner.y()) + qMax(emptySpaceX, emptySpaceY)) /
      qMin(width(), height());
  t.scale(1 / scale, 1 / scale);
  t.translate(emptySpaceX / 2, emptySpaceY / 2);

  QTransform swapY;
  swapY.translate(0, height());
  swapY.scale(1, -1);

  transformViewport = t * swapY;
  return t;
}
QTransform OpenGLWidget::updateViewport(QPointF Max) {
  maxCorner = Max;
  updateViewport();
  return transformViewport;
}
