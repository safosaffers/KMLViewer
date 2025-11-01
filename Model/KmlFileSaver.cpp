#include "KmlFileSaver.h"
#include <QDebug>

QString KmlFileSaver::polygonToKmlCoords(const QPolygonF& polygon) {
    QStringList parts;
    for (const QPointF& pt : polygon) {
        parts << QString::number(pt.x(), 'g', 12) + "," +
                     QString::number(pt.y(), 'g', 12) + "," +
                     QString::number(0, 'g', 12);
    }
    return parts.join(" ");
}

void KmlFileSaver::updateCoordinatesInDocument(QDomDocument& doc, 
                                               const QList<PolygonPair>& simplified) {
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

bool KmlFileSaver::saveKmlFile(const QDomDocument& document, const QString& fileName) {
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