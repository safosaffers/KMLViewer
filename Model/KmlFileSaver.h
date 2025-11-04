#ifndef KMLFILESAVER_H
#define KMLFILESAVER_H

#include <QDomDocument>
#include <QDomNodeList>
#include <QFile>
#include <QList>
#include <QPair>  // Qt's QPair
#include <QPointF>
#include <QPolygonF>
#include <QString>
#include <QStringList>
#include <QTextStream>

using PolygonPair = QPair<QPolygonF, QPolygonF>;

class KmlFileSaver {
public:
  // Save KML file
  static bool saveKmlFile(const QDomDocument& document, const QString& fileName);

  // Update coordinates in KML document
  static void updateCoordinatesInDocument(QDomDocument& doc, const QList<PolygonPair>& simplified);

  // Convert polygon to KML coordinate string
  static QString polygonToKmlCoords(const QPolygonF& polygon);
};

#endif  // KMLFILESAVER_H