#include "Controller.h"

Controller::Controller(Model* m, View* v) : QObject(v), model(m), view(v) {
  connect(view, &View::fileNameChoosed, this, &Controller::HandleModelLoading);
  connect(view, &View::polygonSimplifyRequested, this,
          &Controller::HandleModelSimplify);
  connect(&watcher, &QFutureWatcher<QPolygonF>::finished, this,
          [this] { finishModelSimplify(); });
  connect(&watcher, &QFutureWatcher<QPolygonF>::progressRangeChanged,
          view->ui->progressBar, &QProgressBar::setRange);
  connect(&watcher, &QFutureWatcher<QPolygonF>::progressValueChanged,
          view->ui->progressBar, &QProgressBar::setValue);
}

Controller::~Controller() { watcher.cancel(); }

void Controller::HandleModelLoading(QString fileName) {
  model->initializeModel(fileName);
  view->getGLWidget()->setPolygons(model->getPolygons());
  view->getGLWidget()->resetSimplifiedPolygons();
  view->getGLWidget()->updateViewport(model->getDownRightCornerForViewPort());
  view->updateNumberOfPolygons(model->getNumberOfPolygons());
  view->updateNumberOfPolygonsPoints(model->getNumberOfPolygonsPoints());
  view->ui->progressBar->setValue(0);
  view->getGLWidget()->update();
}

void Controller::HandleModelSimplify(double epsilon) {
  if (view->ui->btnSimplifyPoligons->isChecked()) {
    view->ui->btnUploadaKMLFile->setEnabled(false);
    view->ui->btnSimplifyPoligons->setText(tr("Cancel"));
    view->ui->lblNumberOfSimplifiedPolygonsPoints->setText(
        tr("Calculating ..."));
    view->ui->progressBar->setValue(0);

    const double eps = epsilon;

    auto polygonsToSimplify = model->getPolygons();

    timer.start();

    // starts simplification in parrallel
    auto future = QtConcurrent::mapped(polygonsToSimplify,
                                       [eps](const QPolygonF& p) -> QPolygonF {
                                         return Model::simplifyPolygon(p, eps);
                                       });

    watcher.setFuture(future);

  } else {
    watcher.cancel();
    view->ui->progressBar->setValue(0);
    view->ui->btnUploadaKMLFile->setEnabled(true);
    view->ui->lblNumberOfSimplifiedPolygonsPoints->setText(tr("—"));
    view->ui->btnSimplifyPoligons->setText(tr("Start"));
  }
}

void Controller::finishModelSimplify() {
  if (watcher.isCanceled()) return;

  QList<QPolygonF> simplified = watcher.future().results();
  model->setSimplifiedPolygons(simplified);

  view->ui->btnUploadaKMLFile->setEnabled(true);
  view->ui->btnSimplifyPoligons->setChecked(false);
  view->ui->btnSimplifyPoligons->setText(tr("Start"));
  view->ui->lblNumberOfSimplifiedPolygonsPoints->setText(
      QString::number(model->getNumberOfSimplifiedPolygonsPoints()));

  view->getGLWidget()->setSimplifiedPolygons(simplified);
  view->getGLWidget()->update();
}
