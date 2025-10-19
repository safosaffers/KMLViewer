#ifndef MODEL_H
#define MODEL_H
#define _USE_MATH_DEFINES

#include <QDebug>
#include <QDomAttr>
#include <QFile>
#include <QPointF>
#include <QString>
#include <cmath>
#define EQUATORIAL_EARTH_RADIUS_METERS 6378137
class Model {
 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
};
#endif  // MODEL_H
