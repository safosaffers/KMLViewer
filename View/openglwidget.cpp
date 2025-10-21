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
void OpenGLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.setWorldTransform(transformViewport);

  setPenWidthAccordingToViewport(painter, Qt::black);
  setBrushWithAlpha(painter, Qt::black, 0.5);
  for (QPolygonF& poly : polygons) {
    painter.drawPolygon(poly, Qt::WindingFill);
  }

  setPenWidthAccordingToViewport(painter, Qt::green);
  setBrushWithAlpha(painter, Qt::green, 0.5);
  for (QPolygonF& poly : simplifiedPolygons) {
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
void OpenGLWidget::setSimplifiedPolygons(QList<QPolygonF> simplifiedPolygons) {
  this->simplifiedPolygons = simplifiedPolygons;
}
QTransform OpenGLWidget::updateViewport() {
  QTransform t;
  int emptyPercent = 20;
  qreal emptySpaceX = maxCorner.x() * emptyPercent / 100;
  qreal emptySpaceY = maxCorner.y() * emptyPercent / 100;
  scaleViewport =
      qMin(width(), height()) /
      (qMax(maxCorner.x(), maxCorner.y()) + qMax(emptySpaceX, emptySpaceY));
  t.scale(scaleViewport, scaleViewport);
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
