#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QPainter>
class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
public:
  explicit OpenGLWidget(QWidget* parent = nullptr);
  ~OpenGLWidget();

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
};
#endif  // OPENGLWIDGET_H
