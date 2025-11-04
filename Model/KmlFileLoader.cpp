#include "KmlFileLoader.h"

#include <QDebug>
#include <stdexcept>

void KmlFileLoader::updateMinMaxLatLon(double longitude, double latitude, double& minLon,
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

QPolygonF KmlFileLoader::parseCoordinateString(const QString& coordsString, double& minLon,
                                               double& maxLon, double& minLat, double& maxLat) {
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
        updateMinMaxLatLon(longitude, latitude, minLon, maxLon, minLat, maxLat);
      }
    }
  }

  return result;
}

QDomDocument* KmlFileLoader::loadKmlFile(const QString& filePath) {
  QDomDocument* doc = nullptr;
  QFile file(filePath);
  if (file.open(QIODevice::ReadOnly)) {
    doc = new QDomDocument("KML Document");
    if (!doc->setContent(&file)) {
      delete doc;
      doc = nullptr;
    }
    file.close();
  }

  return doc;
}

QList<QPolygonF> KmlFileLoader::parseCoordinatesFromDocument(QDomDocument* document, double& minLon,
                                                             double& maxLon, double& minLat,
                                                             double& maxLat) {
  if (!document) {
    throw std::invalid_argument("Document is null");
  }

  QList<QPolygonF> result;
  QDomNodeList coordsList = document->elementsByTagName(QString("coordinates"));
  for (const QDomNode& coords : coordsList) {
    QString coordsString = coords.firstChild().nodeValue();
    QPolygonF res = parseCoordinateString(coordsString, minLon, maxLon, minLat, maxLat);
    result.append(res);
  }

  return result;
}