#ifndef MODEL_H
#define MODEL_H
#include <QDebug>
#include <QDomAttr>
#include <QFile>
#include <QString>
class Model {
 public:
  Model();
  ~Model();
  void initializeModel(QString filePath);
};
#endif  // MODEL_H
