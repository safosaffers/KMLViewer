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
