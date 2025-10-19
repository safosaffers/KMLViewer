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

  painter.setPen(QPen(Qt::black, 5));
  QColor br = Qt::black;
  br.setAlphaF(0.5);
  painter.setBrush(br);
  static const QPointF points2[4] = {
      QPointF(20.0, 500.0), QPointF(200.0, 150.0), QPointF(700.0, 50.0),
      QPointF(450.0, 450.0)};
  painter.drawConvexPolygon(points2, 4);

  painter.setPen(QPen(Qt::green, 5));
  br = Qt::green;
  br.setAlphaF(0.5);
  painter.setBrush(br);
  static const QPointF points[4] = {QPointF(50.0, 400.0), QPointF(100.0, 50.0),
                                    QPointF(400.0, 150.0),
                                    QPointF(450.0, 350.0)};
  painter.drawConvexPolygon(points, 4);

  painter.end();
}
void OpenGLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}
void OpenGLWidget::setPolygons(QList<QPolygonF> polygons) {
  this->polygons = polygons;
}
