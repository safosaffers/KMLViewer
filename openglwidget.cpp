#include "openglwidget.h"

OpenGLWidget::OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
  setFocusPolicy(Qt::StrongFocus);
}
OpenGLWidget::~OpenGLWidget() {}
void OpenGLWidget::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(1.f, 1.f, 1.f, 1.f);
}
void OpenGLWidget::paintGL() { glClear(GL_COLOR_BUFFER_BIT); }
void OpenGLWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);
}
