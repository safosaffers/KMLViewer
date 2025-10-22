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
QList<PolygonPair> Model::convertToMeters(QList<QPolygonF> LonLatQList,
                                          double& minLon, double& minLat) {
  latLonToMetersPolygons.clear();
  for (QPolygonF& latLonPoly : LonLatQList) {
    QPolygonF metersPoly;
    for (QPointF lonlat : latLonPoly) {
      double xLen =
          lonDifferenceInMeters(minLat, lonlat.y(), minLon, lonlat.x());
      double yLen = latDifferenceInMeters(minLat, lonlat.y());
      metersPoly.append(QPointF(xLen, yLen));
    }
    latLonToMetersPolygons.append({latLonPoly, metersPoly});
  }
  return latLonToMetersPolygons;
}
QPointF Model::getCornerInMeters(double& minLon, double& maxLon, double& minLat,
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
    QList<QPolygonF> LonLatQList =
        parseLonLatFromKML(filePath, minLon, maxLon, minLat, maxLat);
    latLonToMetersPolygons = convertToMeters(LonLatQList, minLon, minLat);

    // -> top-left corner is just 0,0
    downRightCornerForViewPort =
        getCornerInMeters(minLon, maxLon, minLat,
                          maxLat);  // -> down-right corner
  } catch (const std::invalid_argument& e) {
    qDebug() << "Error: " << e.what();
  }
}
QList<PolygonPair> Model::getPolygons() {
  return latLonToMetersPolygons;
}
QPointF Model::getDownRightCornerForViewPort() {
  return downRightCornerForViewPort;
}
qreal Model::distanceBetweenQLineFAndPoint(const QLineF& line,
                                           const QPointF& p) {
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
PolygonPair Model::simplifyPolygon(PolygonPair latLonMetPoly, double epsilon) {
  if (latLonMetPoly.second.size() <= 2 || epsilon <= 0) return latLonMetPoly;
  PolygonPair simplifiedPolygon;
  QPointF A = latLonMetPoly.second.first();
  QPointF B = latLonMetPoly.second.last();
  QLineF AB(A, B);
  qreal d_max = 0.0;
  qsizetype C_idx = 0;
  for (qsizetype i = 1; i < latLonMetPoly.second.size() - 1; i++) {
    qreal d = distanceBetweenQLineFAndPoint(AB, latLonMetPoly.second.at(i));
    if (d > d_max) {
      d_max = d;
      C_idx = i;
    }
  }
  if (d_max <= epsilon) {
    simplifiedPolygon.first<<latLonMetPoly.first.first()<<latLonMetPoly.first.last();
    simplifiedPolygon.second<<latLonMetPoly.second.first()<<latLonMetPoly.second.last();
  } else {
    PolygonPair A___C = {{latLonMetPoly.first.first(C_idx + 1)}, {latLonMetPoly.second.first(C_idx + 1)}};
    PolygonPair C___B = {{latLonMetPoly.first.last(latLonMetPoly.second.size() - C_idx)}, {latLonMetPoly.second.last(latLonMetPoly.second.size() - C_idx)}};
    PolygonPair A___C_simplified = simplifyPolygon(A___C, epsilon);
    PolygonPair C___B_simplified = simplifyPolygon(C___B, epsilon);
    simplifiedPolygon = std::move(A___C_simplified);
    simplifiedPolygon.first.removeLast();
    simplifiedPolygon.second.removeLast();
    simplifiedPolygon.first <<C___B_simplified.first;
    simplifiedPolygon.second <<C___B_simplified.second;
  }
  return simplifiedPolygon;
}

// QList<QPolygonF> Model::getSimplifiedPolygons() { return simplifiedPolygons; }
// void Model::simplifyPolygons(double epsilon) {
//   simplifiedPolygons.clear();
//   for (QPolygonF& polygon : polygons) {
//     simplifiedPolygons.append(simplifyPolygon(polygon, epsilon));
//   }
// }

int Model::getNumberOfPolygons() { return latLonToMetersPolygons.size(); }
int Model::getQListQPolygonFPointsCount(QList<PolygonPair> polygons) {
  int result = 0;
  for (PolygonPair& polygon : polygons) {
    result += polygon.first.size();
  }
  return result;
}
int Model::getNumberOfPolygonsPoints() {
  return getQListQPolygonFPointsCount(latLonToMetersPolygons);
}
int Model::getNumberOfSimplifiedPolygonsPoints() {
  return getQListQPolygonFPointsCount(simplifiedPolygons);
}
void Model::setSimplifiedPolygons(const QList<PolygonPair>& polys) {
  simplifiedPolygons = polys;
}
