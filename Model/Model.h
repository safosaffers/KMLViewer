#ifndef MODEL_H
#define MODEL_H
#define _USE_MATH_DEFINES

#include <QDebug>
#include <QDomAttr>
#include <QFile>
#include <QLineF>
#include <QPointF>
#include <QPolygonF>
#include <QString>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <QElapsedTimer>
#include "CoordinateConverter.h"
#include "KmlFileLoader.h"
#include "KmlFileSaver.h"
#include "Normalizer.h"
#include "PolygonSimplifier.h"
#include "PolyRepr.h"

// Structure to hold simplification results
struct SimplificationResult {
    PolygonPair simplifiedPolygons;  // Pair of lon/lat and meters polygons
    qint64 timeNs;                   // Time in nanoseconds
    double maxDeviation;            // Maximum deviation after simplification
    int originalPoints;             // Number of points before simplification
    int simplifiedPoints;           // Number of points after simplification
    
    SimplificationResult() : timeNs(0), maxDeviation(0.0), originalPoints(0), simplifiedPoints(0) {}
    SimplificationResult(PolygonPair poly, qint64 time, double deviation, int origPoints, int simpPoints)
        : simplifiedPolygons(poly), timeNs(time), maxDeviation(deviation), 
          originalPoints(origPoints), simplifiedPoints(simpPoints) {}
};

class Model {
 private:
  PolyRepr polygonRepresentations;  // All original polygon representations in one place
  PolyRepr polygonRepresentationsSimplified;  // All simplified polygon representations in one place
  QPointF downRightCornerForViewPort;
  QDomDocument* currentDocument;
  QString currentFilePath;
  QPointF normalizedMaxCoord;
  qreal normalizeFactor;
  qreal maxCoord;

 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
  PolyRepr getPolygons();
  QPointF getNormalizedMaxCoord();

  static PolygonPair simplifyPolygon(const PolygonPair metersPoly, double epsilon);
  static SimplificationResult simplifyPolygonWithDeviation(const PolygonPair metersPoly, double epsilon);

  int getNumberOfPolygons();
  int getQListQPolygonFPointsCount(const QList<QPolygonF>& polygons);
  int getNumberOfPolygonsPoints();
  int getNumberOfSimplifiedPolygonsPoints();
  void setSimplifiedPolygons(const QList<QPolygonF>& polys);
  void saveSimplifiedModel(QString fileName);

  // Normalization methods
  void normalizePolygons();
  void normalizeSimplifiedPolygons();
  QPointF getMaxCoord() const;

  // Access to different polygon representations (original)
  QList<QPolygonF> getLonLatPolygons() const;
  QList<QPolygonF> getMetersPolygons() const;
  QList<QPolygonF> getNormalizedPolygons() const;
  void setLonLatPolygons(const QList<QPolygonF>& polys);
  void setMetersPolygons(const QList<QPolygonF>& polys);
  void setNormalizedPolygons(const QList<QPolygonF>& polys);

  // Access to different polygon representations (simplified)
  QList<QPolygonF> getSimplifiedLonLatPolygons() const;
  QList<QPolygonF> getSimplifiedMetersPolygons() const;
  QList<QPolygonF> getSimplifiedNormalizedPolygons() const;
  void setSimplifiedLonLatPolygons(const QList<QPolygonF>& polys);
  void setSimplifiedMetersPolygons(const QList<QPolygonF>& polys);
  void setSimplifiedNormalizedPolygons(const QList<QPolygonF>& polys);
  QList<QPolygonF> getNormalizedSimplifiedPolygons();
 private:
  QList<QPolygonF> convertToMeters(QList<QPolygonF> LonLatQList,
                                   double& minLon, double& minLat);
  QPointF getCornerInMeters(double& minLon, double& maxLon, double& minLat,
                            double& maxLat);
};
#endif  // MODEL_H
