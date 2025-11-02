#include "Controller.h"

#include <QElapsedTimer>

Controller::Controller(Model* m, View* v) : QObject(v), model(m), view(v) {
  // Initialize polygon info model
  polygonInfoModel = new PolygonInfoModel(this);
  view->ui->tvPolygonsInfo->setModel(polygonInfoModel);

  connect(view, &View::fileNameChoosed, this, &Controller::HandleModelLoading);
  connect(view, &View::polygonSimplifyRequested, this,
          &Controller::HandleModelSimplify);
  connect(view, &View::saveSimplifyPoligons, this,
          &Controller::HandleModelSimplifySave);
  // Connect progress bar signals
  connect(&watcher, &QFutureWatcher<SimplificationResult>::progressRangeChanged,
          view->ui->progressBar, &QProgressBar::setRange);
  connect(&watcher, &QFutureWatcher<SimplificationResult>::progressValueChanged,
          view->ui->progressBar, &QProgressBar::setValue);

  connect(view, &View::fileNameChoosed, this, &Controller::HandleModelLoading);
  connect(view, &View::polygonSimplifyRequested, this,
          &Controller::HandleModelSimplify);
  connect(view, &View::saveSimplifyPoligons, this,
          &Controller::HandleModelSimplifySave);
  connect(&watcher, &QFutureWatcher<SimplificationResult>::finished, this,
          &Controller::finishModelSimplify);
}

Controller::~Controller() { watcher.cancel(); }

void Controller::HandleModelLoading(QString fileName) {
  try {
    model->initializeModel(fileName);
    view->getGLWidget()->setPolygons(model->getNormalizedPolygons());
    view->getGLWidget()->clearSimplifiedPolygons();
    view->clearSimplificationInfo();
    view->getGLWidget()->setInitialViewport(
        model->getNormalizedMaxCoord());  // always called after
                                          // normalizePolygons
    view->updateNumberOfPolygons(model->getNumberOfPolygons());
    view->updateNumberOfPolygonsPoints(model->getNumberOfPolygonsPoints());

    // Initialize polygon info model with initial data
    polygonInfoModel->setPolygonCount(model->getNumberOfPolygons());
    QList<QPolygonF> metersPolygons = model->getMetersPolygons();
    for (int i = 0; i < metersPolygons.size(); i++) {
      PolygonInfo info(i, metersPolygons[i].size(), 0, 0, 0.0);
      polygonInfoModel->setPolygonInfo(i, info);
    }

    // Resize columns to fit content
    view->ui->tvPolygonsInfo->resizeColumnsToContents();

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
    view->showProgressBar();  // Show the progress bar

    const double eps = epsilon;

    QList<PolygonPair> polygonsToSimplify;
    for (int i = 0; i < model->getNumberOfPolygons(); i++) {
      polygonsToSimplify.append(QPair(model->getLonLatPolygons().at(i),
                                      model->getMetersPolygons().at(i)));
    }

    timer.start();

    // starts simplification in parallel with timing information
    auto future = QtConcurrent::mapped(
        polygonsToSimplify,
        [eps](const PolygonPair& p) -> SimplificationResult {
          // Use the new method that calculates proper deviation
          return Model::simplifyPolygonWithDeviation(p, eps);
        });

    watcher.setFuture(future);

  } else {
    watcher.cancel();
    view->ui->progressBar->setValue(0);
    view->hideProgressBar();  // Hide the progress bar when cancelled
    view->ui->btnUploadaKMLFile->setEnabled(true);
    view->ui->lblNumberOfSimplifiedPolygonsPoints->setText(tr("—"));
    view->ui->btnSimplifyPoligons->setText(tr("Упростить"));
  }
}

void Controller::finishModelSimplify() {
  if (watcher.isCanceled()) {
    view->hideProgressBar();  // Hide the progress bar if cancelled
    return;
  }

  // Get the elapsed time for the total simplification process
  const qint64 elapsed = timer.nsecsElapsed();

  QList<SimplificationResult> results = watcher.future().results();
  QList<QPolygonF> polyMeters;
  QList<QPolygonF> polyLonLat;

  for (int i = 0; i < results.size(); i++) {
    SimplificationResult result = results.at(i);
    polyLonLat.append(result.simplifiedPolygons.first);
    polyMeters.append(result.simplifiedPolygons.second);

    // Update the polygon info model with individual polygon data
    polygonInfoModel->updatePolygonAfterSimplification(
        i,
        result.simplifiedPoints,  // points after simplification
        result.timeNs,            // elapsed time in nanoseconds
        result.maxDeviation       // max deviation
    );
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

  // Display the total simplification time
  view->ui->lblTimeAlgorithm->setText(QString::number(elapsed) + " ns");

  view->getGLWidget()->setSimplifiedPolygons(
      model->getSimplifiedNormalizedPolygons());
  // Resize columns to fit the updated content after simplification
  view->ui->tvPolygonsInfo->resizeColumnsToContents();
  view->getGLWidget()->update();

  view->hideProgressBar();  // Hide the progress bar when finished
}

void Controller::HandleModelSimplifySave(QString fileName) {
  model->saveSimplifiedModel(fileName);
}
