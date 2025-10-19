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


  // for(QPolygonF &poly : polygons){
  //   painter.drawPolygon(poly, Qt::WindingFill);
  // }
  updateViewport(QPointF(20,20));
  painter.setWorldTransform(transformViewport);
  static const QPointF points2[4] = {
      QPointF(0., 0.), QPointF(20., 0.), QPointF(20., 20.),
      QPointF(0., 20.)};
  painter.drawConvexPolygon(points2, 4);

  painter.end();
}
void OpenGLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}
void OpenGLWidget::setPolygons(QList<QPolygonF> polygons) {
  this->polygons = polygons;
}
QTransform OpenGLWidget::updateViewport(QPointF Max){
  QTransform t;
  double emptyPercent = 20;
  double emptySpaceX = Max.x()*emptyPercent/100;
  double emptySpaceY = Max.x()*emptyPercent/100;
  double scale =  (qMax(Max.x(), Max.y())+emptySpaceX) / qMin(width(), height());
  t.scale(1/scale, 1/scale);
  t.translate(emptySpaceX/2, emptySpaceY/2);
  transformViewport = t;
  return t;
}
