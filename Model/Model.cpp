#include "Model.h"

#include "CoordinateConverter.h"
#include "KmlFileLoader.h"
#include "KmlFileSaver.h"

Model::Model() { currentDocument = NULL; }
Model::~Model() {
  if (currentDocument) delete currentDocument;
}

QList<PolygonPair> Model::convertToMeters(QList<QPolygonF> LonLatQList,
                                          double& minLon, double& minLat) {
  latLonToMetersPolygons.clear();
  for (const QPolygonF& latLonPoly : LonLatQList) {
    PolygonPair latLonMetPair = qMakePair(latLonPoly, QPolygonF());
    PolygonPair convertedPair = CoordinateConverter::convertLatLonToMeters(
        latLonMetPair, minLon, minLat);
    latLonToMetersPolygons.append(convertedPair);
  }
  return latLonToMetersPolygons;
}
QPointF Model::getCornerInMeters(double& minLon, double& maxLon, double& minLat,
                                 double& maxLat) {
  return CoordinateConverter::getCornerInMeters(minLon, maxLon, minLat, maxLat);
}
void Model::initializeModel(QString filePath) {
  try {
    double minLon = 90.0;
    double maxLon = -90.0;
    double minLat = 180.0;
    double maxLat = -180.0;
    if (currentDocument) {
      delete currentDocument;
      currentDocument = NULL;
    }
    currentDocument = KmlFileLoader::loadKmlFile(filePath);
    if (!currentDocument) {
      throw std::invalid_argument("Failed to open or parse KML file");
    }

    QList<QPolygonF> LonLatQList = KmlFileLoader::parseCoordinatesFromDocument(
        currentDocument, minLon, maxLon, minLat, maxLat);
    latLonToMetersPolygons = convertToMeters(LonLatQList, minLon, minLat);

    // -> top-left corner is just 0,0
    downRightCornerForViewPort =
        getCornerInMeters(minLon, maxLon, minLat,
                          maxLat);  // -> down-right corner
  } catch (const std::invalid_argument& e) {
    qDebug() << "Error: " << e.what();
  }
}
QList<PolygonPair> Model::getPolygons() { return latLonToMetersPolygons; }
QPointF Model::getDownRightCornerForViewPort() {
  return downRightCornerForViewPort;
}

int Model::getNumberOfPolygons() { return latLonToMetersPolygons.size(); }
int Model::getQListQPolygonFPointsCount(const QList<PolygonPair>& polygons) {
  int result = 0;
  for (const PolygonPair& polygon : polygons) {
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

PolygonPair Model::simplifyPolygon(PolygonPair latLonMetPoly, double epsilon) {
  return PolygonSimplifier::simplifyPolygon(latLonMetPoly, epsilon);
}

void Model::simplifyPolygons(double epsilon) {
  QList<PolygonPair> simplified;
  for (const PolygonPair& poly : latLonToMetersPolygons) {
    simplified.append(simplifyPolygon(poly, epsilon));
  }
  setSimplifiedPolygons(simplified);
}

void Model::saveSimplifiedModel(QString fileName) {
  if (!currentDocument) {
    qWarning() << "No source KML document to preserve structure";
    return;
  }

  QDomDocument doc = currentDocument->cloneNode(true).toDocument();
  KmlFileSaver::updateCoordinatesInDocument(doc, simplifiedPolygons);
  KmlFileSaver::saveKmlFile(doc, fileName);
}
