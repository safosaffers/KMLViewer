#include "Model.h"

Model::Model() {}
Model::~Model() {}
QDomDocument* setContentFromFile(QString filePath) {
  QDomDocument* doc = NULL;

  QFile file(filePath);
  if (file.open(QIODevice::ReadOnly)) {
    doc = new QDomDocument("KML Document");
    if (!doc->setContent(&file)) {
      delete doc;
      doc = NULL;
    }
    file.close();
  }

  return doc;
}
void updateMaxMinLatLon(double longitude, double latitude, double& minLon,
                        double& maxLon, double& minLat, double& maxLat) {
  if (minLon > longitude) {
    minLon = longitude;
  } else if (maxLon < longitude) {
    maxLon = longitude;
  }
  if (minLat > latitude) {
    minLat = latitude;
  } else if (maxLat < latitude) {
    maxLat = latitude;
  }
}
static QPolygonF parseLonLatString(const QString& coordsString, double& minLon,
                                   double& maxLon, double& minLat,
                                   double& maxLat) {
  QPolygonF result;

  const QStringList coordPairs = coordsString.split(" ", Qt::SkipEmptyParts);
  for (const QString& pair : coordPairs) {
    QStringList lonLatAlt = pair.split(",", Qt::SkipEmptyParts);
    if (lonLatAlt.size() >= 2) {
      bool okLon = false, okLat = false;
      double longitude = lonLatAlt[0].toDouble(&okLon);
      double latitude = lonLatAlt[1].toDouble(&okLat);
      if (okLon && okLat) {
        result.append(QPointF(longitude, latitude));
        updateMaxMinLatLon(longitude, latitude, minLon, maxLon, minLat, maxLat);
      }
    }
  }

  return result;
}

QList<QPolygonF> parseLonLatFromKML(QString filePath, double& minLon,
                                    double& maxLon, double& minLat,
                                    double& maxLat) {
  QDomDocument* doc = setContentFromFile(filePath);
  if (!doc) {
    throw std::invalid_argument("Failed to open or parse KML file");
  }

  QList<QPolygonF> result;
  QDomNodeList coordsList = doc->elementsByTagName(QString("coordinates"));
  for (const QDomNode& coords : coordsList) {
    QString coordsString = coords.firstChild().nodeValue();
    result.append(
        parseLonLatString(coordsString, minLon, maxLon, minLat, maxLat));
  }

  delete doc;
  return result;
}
double degToRad(double deg) { return deg * M_PI / 180.0; }
double Haversine(double theta) {
  double radiansTheta = degToRad(theta);
  return pow(sin(radiansTheta / 2.0), 2);
}
double latDifferenceInMeters(double lat1, double lat2) {
  double R = EQUATORIAL_EARTH_RADIUS_METERS;
  double deltaLat = fabs(lat2 - lat1);
  double havTheta = Haversine(deltaLat);
  double result = 2 * R * asin(sqrt(havTheta));
  return result;
}
double lonDifferenceInMeters(double lat1, double lat2, double lon1,
                             double lon2) {
  double R = EQUATORIAL_EARTH_RADIUS_METERS;
  double deltaLon = fabs(lon2 - lon1);
  double radLat1 = degToRad(lat1);
  double radLat2 = degToRad(lat2);
  double havTheta = cos(radLat1) * cos(radLat2) * Haversine(deltaLon);
  double result = 2 * R * asin(sqrt(havTheta));
  return result;
}
QList<QPolygonF> convertToMeters(QList<QPolygonF> LonLatQList, double& minLon,
                                 double& minLat) {
  QList<QPolygonF> MetersQList;
  for (QPolygonF& latLonPoly : LonLatQList) {
    QPolygonF metersPoly;
    for (QPointF lonlat : latLonPoly) {
      double xLen =
          lonDifferenceInMeters(minLat, lonlat.y(), minLon, lonlat.x());
      double yLen = latDifferenceInMeters(minLat, lonlat.y());
      metersPoly.append(QPointF(xLen, yLen));
    }
    MetersQList.append(metersPoly);
  }
  return MetersQList;
}
QPointF getCornerInMeters(double& minLon, double& maxLon, double& minLat,
                          double& maxLat) {
  double xLen = latDifferenceInMeters(minLat, maxLat);
  double yLen = lonDifferenceInMeters(minLat, maxLat, minLon, maxLon);
  return QPointF(xLen, yLen);
}
void Model::initializeModel(QString filePath) {
  try {
    double minLon = 90.0;
    double maxLon = -90.0;
    double minLat = 180.0;
    double maxLat = -180.0;
    const QList<QPolygonF> LonLatQList =
        parseLonLatFromKML(filePath, minLon, maxLon, minLat, maxLat);
    polygons = convertToMeters(LonLatQList, minLon, minLat);

    // -> top-left corner is just 0,0
    downRightCornerForViewPort =
        getCornerInMeters(minLon, maxLon, minLat,
                          maxLat);  // -> down-right corner
  } catch (const std::invalid_argument& e) {
    qDebug() << "Error: " << e.what();
  }
}
QList<QPolygonF> Model::getPolygons() { return polygons; }
QPointF Model::getDownRightCornerForViewPort() {
  return downRightCornerForViewPort;
}
qreal Model::distanceBetweenQLineFAndPoint(const QLineF& line,
                                           const QPointF& p) const {
  // transform to loocal coordinates system (0,0) - (lx, ly)
  QPointF p1 = line.p1();
  QPointF p2 = line.p2();
  qreal x = p.x() - p1.x();
  qreal y = p.y() - p1.y();
  qreal x2 = p2.x() - p1.x();
  qreal y2 = p2.y() - p1.y();

  // if line is a point (nodes are the same) =>
  // just return distance between point and one line node
  qreal norm = sqrt(x2 * x2 + y2 * y2);
  if (norm <= std::numeric_limits<int>::epsilon()) return sqrt(x * x + y * y);

  // distance
  return fabs(x * y2 - y * x2) / norm;
}
QPolygonF Model::simplifyPolygon(QPolygonF polygon, double epsilon) {
  if (polygon.size() <= 2 || epsilon <= 0) return polygon;
  QPolygonF simplifiedPolygon;
  QPointF A = polygon.first();
  QPointF B = polygon.last();
  QLineF AB(A, B);
  qreal d_max = 0.0;
  qsizetype C_idx = 0;
  for (qsizetype i = 1; i < polygon.size() - 1; i++) {
    qreal d = distanceBetweenQLineFAndPoint(AB, polygon.at(i));
    if (d > d_max) {
      d_max = d;
      C_idx = i;
    }
  }
  if (d_max <= epsilon) {
    simplifiedPolygon.append(A);
    simplifiedPolygon.append(B);
  } else {
    QPolygonF A___C = polygon.first(C_idx + 1);
    QPolygonF C___B = polygon.last(polygon.size() - C_idx);
    QPolygonF A___C_simplified = simplifyPolygon(A___C, epsilon);
    QPolygonF C___B_simplified = simplifyPolygon(C___B, epsilon);
    simplifiedPolygon = std::move(A___C_simplified);
    simplifiedPolygon.removeLast();
    simplifiedPolygon.append(C___B_simplified);
  }
  return simplifiedPolygon;
}

QList<QPolygonF> Model::getSimplifiedPolygons() { return simplifiedPolygons; }
void Model::simplifyPolygons(double epsilon) {
  for (QPolygonF& polygon : polygons) {
    simplifiedPolygons.append(simplifyPolygon(polygon, epsilon));
  }
}

int Model::getNumberOfPolygons() { return polygons.size(); }
int Model::getQListQPolygonFPointsCount(QList<QPolygonF> polygons) {
  int result = 0;
  for (QPolygonF polygon : polygons) {
    result += polygon.size();
  }
  return result;
}
int Model::getNumberOfPolygonsPoints() {
  return getQListQPolygonFPointsCount(polygons);
}
int Model::getNumberOfSimplifiedPolygonsPoints() {
  return getQListQPolygonFPointsCount(simplifiedPolygons);
}
