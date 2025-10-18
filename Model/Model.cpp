#include "Model.h"

Model::Model() {}
Model::~Model() {}
typedef QList<QPair<double, double>> Polygon;
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
static Polygon parseLonLatString(
    const QString& coordsString) {
  Polygon result;

  const QStringList coordPairs = coordsString.split(" ", Qt::SkipEmptyParts);
  for (const QString& pair : coordPairs) {
    QStringList lonLatAlt = pair.split(",", Qt::SkipEmptyParts);
    if (lonLatAlt.size() >= 2) {
      bool okLon = false, okLat = false;
      double longitude = lonLatAlt[0].toDouble(&okLon);
      double latitude = lonLatAlt[1].toDouble(&okLat);
      if (okLon && okLat) {
        result.append(QPair<double, double>(longitude, latitude));
      }
    }
  }

  return result;
}

QList<Polygon> parseLonLatFromKML(QString filePath) {
  QDomDocument* doc = setContentFromFile(filePath);
  if (!doc) {
    throw std::invalid_argument("Failed to open or parse KML file");
  }

  QList<Polygon> result;
  QDomNodeList coordsList = doc->elementsByTagName(QString("coordinates"));
  for (const QDomNode& coords : coordsList) {
    QString coordsString = coords.firstChild().nodeValue();
    result.append(parseLonLatString(coordsString));
  }

  delete doc;
  return result;
}
void Model::initializeModel(QString filePath) {
  try {
    const QList<Polygon> LonLatQList = parseLonLatFromKML(filePath);
    int i =0;
    for (const Polygon &poly : LonLatQList)
    {
      qDebug()<<"\n" <<i << "Poly: \n";
      i++;
      for (const QPair<double, double> latlon : poly) {
        qDebug() << "Longitude: " << latlon.first << ", Latitude: " << latlon.second;
      }
    }
  } catch (const std::invalid_argument& e) {
    qDebug() << "Error: " << e.what();
  }

  // todo
}
