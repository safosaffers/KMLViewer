#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPolygonF>

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
 private:
  QList<QPolygonF> polygons;

 public:
  explicit OpenGLWidget(QWidget* parent = nullptr);
  ~OpenGLWidget();
  void setPolygons(QList<QPolygonF> polygons);

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
};
#endif  // OPENGLWIDGET_H
