#ifndef KMLFILELOADER_H
#define KMLFILELOADER_H

#include <QDomDocument>
#include <QDomNodeList>
#include <QFile>
#include <QList>
#include <QPair>
#include <QPointF>
#include <QPolygonF>
#include <QString>
#include <QStringList>

class KmlFileLoader {
public:
  // Load and parse KML file
  static QDomDocument* loadKmlFile(const QString& filePath);

  // Parse coordinates from KML document
  static QList<QPolygonF> parseCoordinatesFromDocument(QDomDocument* document, double& minLon,
                                                       double& maxLon, double& minLat,
                                                       double& maxLat);

  // Parse coordinates from a string (coordinates element content)
  static QPolygonF parseCoordinateString(const QString& coordsString, double& minLon,
                                         double& maxLon, double& minLat, double& maxLat);

private:
  // Helper to update min/max lat/lon values
  static void updateMinMaxLatLon(double longitude, double latitude, double& minLon, double& maxLon,
                                 double& minLat, double& maxLat);
};

#endif  // KMLFILELOADER_H
