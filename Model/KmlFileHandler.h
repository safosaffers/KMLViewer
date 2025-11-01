#ifndef KMLFILEHANDLER_H
#define KMLFILEHANDLER_H

#include <QDomDocument>
#include <QDomNodeList>
#include <QFile>
#include <QList>
#include <QPointF>
#include <QPolygonF>
#include <QString>
#include <QStringList>
#include <utility>  // for std::pair

using PolygonPair = QPair<QPolygonF, QPolygonF>;

class KmlFileHandler {
 public:
  // Load and parse KML file
  static QDomDocument* loadKmlFile(const QString& filePath);

  // Parse coordinates from KML document
  static QList<QPolygonF> parseCoordinatesFromDocument(QDomDocument* document,
                                                       double& minLon,
                                                       double& maxLon,
                                                       double& minLat,
                                                       double& maxLat);

  // Parse coordinates from a string (coordinates element content)
  static QPolygonF parseCoordinateString(const QString& coordsString,
                                         double& minLon, double& maxLon,
                                         double& minLat, double& maxLat);

  // Save KML file
  static bool saveKmlFile(const QDomDocument& document,
                          const QString& fileName);

  // Update coordinates in KML document
  static void updateCoordinatesInDocument(QDomDocument& doc,
                                          const QList<PolygonPair>& simplified);

  // Convert polygon to KML coordinate string
  static QString polygonToKmlCoords(const QPolygonF& polygon);

 private:
  // Helper to update min/max lat/lon values
  static void updateMinMaxLatLon(double longitude, double latitude,
                                 double& minLon, double& maxLon, double& minLat,
                                 double& maxLat);
};

#endif  // KMLFILEHANDLER_H