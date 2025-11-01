#ifndef KMLFILESAVER_H
#define KMLFILESAVER_H

#include <QDomDocument>
#include <QFile>
#include <QPolygonF>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDomNodeList>
#include <QTextStream>
#include <QPair>  // Qt's QPair

using PolygonPair = QPair<QPolygonF, QPolygonF>;

class KmlFileSaver {
public:
    // Save KML file
    static bool saveKmlFile(const QDomDocument& document, const QString& fileName);
    
    // Update coordinates in KML document
    static void updateCoordinatesInDocument(QDomDocument& doc, 
                                           const QList<PolygonPair>& simplified);
    
    // Convert polygon to KML coordinate string
    static QString polygonToKmlCoords(const QPolygonF& polygon);
};

#endif // KMLFILESAVER_H