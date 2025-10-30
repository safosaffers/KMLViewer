#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPolygonF>
using PolygonPair = QPair<QPolygonF, QPolygonF>;
class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT
 private:
  QTransform transformViewport;
  QTransform initialTransformViewport;
  QPointF lastMousePos;
  bool isPanning;
  QList<PolygonPair> polygons;
  QList<PolygonPair> simplifiedPolygons;
  QPointF maxCorner;
  qreal scaleViewport;
  static constexpr qreal LINE_WIDTH_RATIO = 1000.0;
  qreal minAllowedScale;
  qreal maxAllowedScale;
  static constexpr const double MIN_ZOOM_FACTOR = 0.02;
  static constexpr const double MAX_ZOOM_FACTOR = 50.0;

 public:
  explicit OpenGLWidget(QWidget* parent = nullptr);
  ~OpenGLWidget();
  void setPolygons(QList<PolygonPair> polygons);
  void resetSimplifiedPolygons();
  void setSimplifiedPolygons(QList<PolygonPair> polygons);
  QTransform setInitialViewport(QPointF Max);

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void drawPolygons(QPainter& painter, const QList<PolygonPair>& polygons,
                    const QColor& color);
  void setPenWidthAccordingToViewport(QPainter& painter, QColor color);
  void setBrushWithAlpha(QPainter& painter, QColor color, qreal alpha);
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
};
#endif  // OPENGLWIDGET_H
