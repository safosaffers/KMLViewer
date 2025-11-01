#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QtMath>
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
  QPointF maxCoord;
  qreal scaleViewport;
  qreal minAllowedScale;
  qreal normalizeFactor;
  static constexpr const double MIN_ZOOM_FACTOR = 0.10;
  static constexpr const double MIN_PEN_WIDTH = 5e-6;
  static constexpr const double LINE_WIDTH_RATIO = 2;

 public:
  explicit OpenGLWidget(QWidget* parent = nullptr);
  ~OpenGLWidget() override;
  void normalizePolygons(QPointF MaxCoord);
  void normalizeSimplifiedPolygons();
  void setPolygons(QList<PolygonPair> polygons);
  void resetSimplifiedPolygons();
  void setSimplifiedPolygons(QList<PolygonPair> polygons);
  QTransform setInitialViewport();

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void drawPolygons(QPainter& painter, const QList<PolygonPair>& polygons,
                    const QColor& color);
  void setPenForEdges(QPainter& painter, QColor color);
  void setPenForPoints(QPainter& painter, QColor color);
  void drawVertexMarkers(QPainter& painter, const QList<QPolygonF>& polygons,
                         const QColor& color);
  void setBrushWithAlpha(QPainter& painter, QColor color, qreal alpha);
  void wheelEvent(QWheelEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
};
#endif  // OPENGLWIDGET_H
