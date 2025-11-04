#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QElapsedTimer>
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QPair>
#include <QtConcurrent>
#include <stdexcept>

#include "../Model/Model.h"
#include "../Model/PolygonInfoModel.h"
#include "../View/view.h"
class Controller : public QObject {
  Q_OBJECT

private:
  Model* model;
  View* view;
  QElapsedTimer timer;
  QFutureWatcher<SimplificationResult> watcher;  // Simplification result with all needed info
  PolygonInfoModel* polygonInfoModel;

public:
  Controller(Model* model, View* view);
  ~Controller();

public slots:
  void HandleModelLoading(QString fileName);
  void HandleModelSimplify(double epsilon);
  void finishModelSimplify();
  void HandleModelSimplifySave(QString fileName);
};

#endif  // CONTROLLER_H
