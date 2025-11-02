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
  connect(&watcher, &QFutureWatcher<QPair<PolygonPair, qint64>>::finished, this,
          &Controller::finishModelSimplify);
  // Note: Progress tracking might need to be handled differently if needed
}

Controller::~Controller() { 
    watcher.cancel(); 
}

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

    const double eps = epsilon;

    QList<PolygonPair> polygonsToSimplify;
    for (int i = 0; i < model->getNumberOfPolygons(); i++) {
      polygonsToSimplify.append(QPair(model->getLonLatPolygons().at(i),
                                      model->getMetersPolygons().at(i)));
    }

    timer.start();

    // starts simplification in parallel with timing information
    auto future = QtConcurrent::mapped(polygonsToSimplify, [eps](const PolygonPair& p) -> QPair<PolygonPair, qint64> {
        QElapsedTimer individualTimer;
        individualTimer.start();
        
        PolygonPair simplified = Model::simplifyPolygon(p, eps);
        qint64 elapsed = individualTimer.elapsed();
        
        // Return both the simplified polygon pair and the time it took
        // We'll use epsilon as the maxDeviation for now since that's the threshold
        return QPair<PolygonPair, qint64>(simplified, elapsed);
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

  // Get the elapsed time for the total simplification process
  const qint64 elapsed = timer.elapsed();

  QList<QPair<PolygonPair, qint64>> results = watcher.future().results();
  QList<QPolygonF> polyMeters;
  QList<QPolygonF> polyLonLat;
  
  for (int i = 0; i < results.size(); i++) {
    QPair<PolygonPair, qint64> result = results.at(i);
    polyLonLat.append(result.first.first);
    polyMeters.append(result.first.second);
    
    // Update the polygon info model with individual polygon data
    // Using epsilon as maxDeviation for now
    polygonInfoModel->updatePolygonAfterSimplification(
        i, 
        result.first.second.size(), 
        result.second,  // elapsed time
        0.0  // Using 0.0 temporarily - could calculate actual deviation
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
  view->ui->lblTimeAlgorithm->setText(
      QString::number(elapsed) + " ms");

  view->getGLWidget()->setSimplifiedPolygons(
      model->getSimplifiedNormalizedPolygons());
  // Resize columns to fit the updated content after simplification
  view->ui->tvPolygonsInfo->resizeColumnsToContents();
  view->getGLWidget()->update();
}

void Controller::HandleModelSimplifySave(QString fileName) {
  model->saveSimplifiedModel(fileName);
}
