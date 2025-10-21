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
  QList<QPolygonF> simplifiedPolygons;
  QPointF maxCorner;
  QTransform transformViewport;
  qreal scaleViewport;
  static constexpr qreal LINE_WIDTH_RATIO = 300.0;

 public:
  explicit OpenGLWidget(QWidget* parent = nullptr);
  ~OpenGLWidget();
  void setPolygons(QList<QPolygonF> polygons);
  void setSimplifiedPolygons(QList<QPolygonF> polygons);
  QTransform updateViewport(QPointF maxCorner);
  QTransform updateViewport();

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void setPenWidthAccordingToViewport(QPainter& painter, QColor color);
  void setBrushWithAlpha(QPainter& painter, QColor color, qreal alpha);
};
#endif  // OPENGLWIDGET_H
