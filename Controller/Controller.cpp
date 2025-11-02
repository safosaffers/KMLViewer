#include "Controller.h"

Controller::Controller(Model* m, View* v) : QObject(v), model(m), view(v) {
  connect(view, &View::fileNameChoosed, this, &Controller::HandleModelLoading);
  connect(view, &View::polygonSimplifyRequested, this,
          &Controller::HandleModelSimplify);
  connect(view, &View::saveSimplifyPoligons, this,
          &Controller::HandleModelSimplifySave);
  connect(&watcher, &QFutureWatcher<QPolygonF>::finished, this,
          &Controller::finishModelSimplify);
  connect(&watcher, &QFutureWatcher<QPolygonF>::progressRangeChanged,
          view->ui->progressBar, &QProgressBar::setRange);
  connect(&watcher, &QFutureWatcher<QPolygonF>::progressValueChanged,
          view->ui->progressBar, &QProgressBar::setValue);
}

Controller::~Controller() { watcher.cancel(); }

void Controller::HandleModelLoading(QString fileName) {
  try {
    model->initializeModel(fileName);
    view->getGLWidget()->setPolygons(model->getNormalizedPolygons());
    view->getGLWidget()->clearSimplifiedPolygons();
    view->getGLWidget()->setInitialViewport(
        model->getNormalizedMaxCoord());  // always called after
                                          // normalizePolygons
    view->updateNumberOfPolygons(model->getNumberOfPolygons());
    view->updateNumberOfPolygonsPoints(model->getNumberOfPolygonsPoints());
    view->setSimplificationAvailable(true);
    view->getGLWidget()->update();
  } catch (const std::exception& ex) {
    view->clearViewData();
    view->showMessageError(ex.what());
  } catch (...) {
    view->clearViewData();
    view->showMessageError("An unknown error occurred while loading the file.");
  }
}

void Controller::HandleModelSimplify(double epsilon) {
  if (view->ui->btnSimplifyPoligons->isChecked()) {
    view->ui->btnUploadaKMLFile->setEnabled(false);
    view->ui->btnSimplifyPoligons->setText(tr("Отмена"));
    view->ui->lblNumberOfSimplifiedPolygonsPoints->setText(
        tr("Вычисление ..."));
    view->ui->progressBar->setValue(0);

    const double eps = epsilon;

    QList<PolygonPair> polygonsToSimplify;
    for (int i = 0; i < model->getNumberOfPolygons(); i++) {
      polygonsToSimplify.append(QPair(model->getLonLatPolygons().at(i),
                                      model->getMetersPolygons().at(i)));
    }

    timer.start();

    // starts simplification in parrallel
    auto future = QtConcurrent::mapped(
        polygonsToSimplify, [eps](const PolygonPair& p) -> PolygonPair {
          return Model::simplifyPolygon(p, eps);
        });

    watcher.setFuture(future);

  } else {
    watcher.cancel();
    view->ui->progressBar->setValue(0);
    view->ui->btnUploadaKMLFile->setEnabled(true);
    view->ui->lblNumberOfSimplifiedPolygonsPoints->setText(tr("—"));
    view->ui->btnSimplifyPoligons->setText(tr("Упростить"));
  }
}

void Controller::finishModelSimplify() {
  if (watcher.isCanceled()) return;

  QList<PolygonPair> simplified = watcher.future().results();
  QList<QPolygonF> polyMeters;
  QList<QPolygonF> polyLonLat;
  for (int i = 0; i < simplified.size(); i++) {
    PolygonPair simplifiedPair = simplified.at(i);
    polyLonLat.append(simplifiedPair.first);
    polyMeters.append(simplifiedPair.second);
  }
  model->setSimplifiedLonLatPolygons(polyLonLat);
  model->setSimplifiedMetersPolygons(polyMeters);
  model->normalizeSimplifiedPolygons();

  view->ui->btnUploadaKMLFile->setEnabled(true);
  view->setSaveSimplificationPolygonsAvailable(true);
  view->ui->btnSimplifyPoligons->setChecked(false);
  view->ui->btnSimplifyPoligons->setText(tr("Упростить"));
  view->ui->lblNumberOfSimplifiedPolygonsPoints->setText(
      QString::number(model->getNumberOfSimplifiedPolygonsPoints()));

  view->getGLWidget()->setSimplifiedPolygons(
      model->getSimplifiedNormalizedPolygons());
  view->getGLWidget()->update();
}

void Controller::HandleModelSimplifySave(QString fileName) {
  model->saveSimplifiedModel(fileName);
}
