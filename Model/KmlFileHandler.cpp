#include "KmlFileHandler.h"

#include <QDebug>
#include <stdexcept>

QDomDocument* KmlFileHandler::loadKmlFile(const QString& filePath) {
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

void KmlFileHandler::updateMinMaxLatLon(double longitude, double latitude,
                                        double& minLon, double& maxLon,
                                        double& minLat, double& maxLat) {
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

QPolygonF KmlFileHandler::parseCoordinateString(const QString& coordsString,
                                                double& minLon, double& maxLon,
                                                double& minLat,
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
        updateMinMaxLatLon(longitude, latitude, minLon, maxLon, minLat, maxLat);
      }
    }
  }

  return result;
}

QList<QPolygonF> KmlFileHandler::parseCoordinatesFromDocument(
    QDomDocument* document, double& minLon, double& maxLon, double& minLat,
    double& maxLat) {
  if (!document) {
    throw std::invalid_argument("Document is null");
  }

  QList<QPolygonF> result;
  QDomNodeList coordsList = document->elementsByTagName(QString("coordinates"));
  for (const QDomNode& coords : coordsList) {
    QString coordsString = coords.firstChild().nodeValue();
    QPolygonF res =
        parseCoordinateString(coordsString, minLon, maxLon, minLat, maxLat);
    result.append(res);
  }

  return result;
}

bool KmlFileHandler::saveKmlFile(const QDomDocument& document,
                                 const QString& fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qWarning() << "Failed to write KML to" << fileName;
    return false;
  }
  QTextStream stream(&file);
  stream << document.toString();
  file.close();
  return true;
}

QString KmlFileHandler::polygonToKmlCoords(const QPolygonF& polygon) {
  QStringList parts;
  for (const QPointF& pt : polygon) {
    parts << QString::number(pt.x(), 'g', 12) + "," +
                 QString::number(pt.y(), 'g', 12) + "," +
                 QString::number(0, 'g', 12);
  }
  return parts.join(" ");
}

void KmlFileHandler::updateCoordinatesInDocument(
    QDomDocument& doc, const QList<PolygonPair>& simplified) {
  QDomNodeList coordNodes = doc.elementsByTagName("coordinates");
  if (coordNodes.size() != simplified.size()) {
    qWarning() << "Mismatch: KML has" << coordNodes.size()
               << "coordinate blocks, but we have" << simplified.size()
               << "simplified polygons";
    return;
  }

  for (int i = 0; i < coordNodes.size(); ++i) {
    QString newCoords = polygonToKmlCoords(simplified[i].first);
    coordNodes.at(i).firstChild().setNodeValue(newCoords);
  }
}