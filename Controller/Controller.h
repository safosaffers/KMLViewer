#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QFuture>
#include <QFutureWatcher>
#include <QObject>
#include <QtConcurrent>

#include "../Model/Model.h"
#include "../View/view.h"

class Controller : public QObject {
  Q_OBJECT

 private:
  Model* model;
  View* view;
  QElapsedTimer timer;
  QFutureWatcher<QPolygonF> watcher;

 public:
  Controller(Model* model, View* view);
  ~Controller();
 public slots:
  void HandleModelLoading(QString fileName);
  void HandleModelSimplify(double epsilon);
  void finishModelSimplify();
};

#endif  // CONTROLLER_H
