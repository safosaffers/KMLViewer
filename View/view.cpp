#include "view.h"
View::View(QWidget* parent) : QMainWindow(parent), ui(new Ui::View) {
  ui->setupUi(this);
  glwidget = new OpenGLWidget(ui->glwidget);
  ui->glWidgetLayout->addWidget(glwidget);
  set_parameters_validators();
  show();
}

View::~View() { delete ui; }
void View::setSaveSimplificationPolygonsAvailable(bool flag) {
  ui->btnSaveSimplifyPoligons->setEnabled(flag);
  ui->action_saveSimplifyPoligons->setEnabled(flag);
}
void View::setSimplificationAvailable(bool flag) {
  ui->progressBar->setValue(0);
  ui->btnSimplifyPoligons->setEnabled(flag);
  ui->leEpsilon->setEnabled(flag);
  ui->btnSaveSimplifyPoligons->setEnabled(!flag);
  if (flag) ui->leEpsilon->setText("1");
}
void View::clearChosenFileName() {
  ui->lblChosenFilename->setText("—");
}
void View::clearPolygonStats() {
  ui->lblNumberOfPolygons->setText("—");
  ui->lblNumberOfPolygonsPoints->setText("—");
  ui->lblNumberOfSimplifiedPolygonsPoints->setText("—");
}
void View::clearViewData() {
  clearPolygonStats();
  clearChosenFileName();
  setSimplificationAvailable(false);
  getGLWidget()->clearPolygons();
  getGLWidget()->clearSimplifiedPolygons();
  getGLWidget()->update();
}

OpenGLWidget* View::getGLWidget() { return glwidget; }
void View::showMessageError(QString text) {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Error");
  msgBox.setText(text);
  msgBox.exec();
}
void View::on_btnSimplifyPoligons_clicked() {
  if (ui->leEpsilon->hasAcceptableInput()) {
    QString strEpsilon = ui->leEpsilon->text();
    double epsilon = strEpsilon.toDouble();
    emit polygonSimplifyRequested(epsilon);
  }
}

void View::set_parameters_validators() {
  auto* validator = new QDoubleValidator(1, 1e9, 6, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  ui->leEpsilon->setValidator(validator);
  ui->leEpsilon->setPlaceholderText("  Epsilon > 0");
}

void View::updateNumberOfPolygons(int numberOfPolygons) {
  ui->lblNumberOfPolygons->setText(QString::number(numberOfPolygons));
}
void View::updateNumberOfPolygonsPoints(int numberOfPolygonsPoints) {
  ui->lblNumberOfPolygonsPoints->setText(
      QString::number(numberOfPolygonsPoints));
}
void View::updateNumberOfSimplifiedPolygonsPoints(
    int numberOfSimplifiedPolygonsPoints) {
  ui->lblNumberOfSimplifiedPolygonsPoints->setText(
      QString::number(numberOfSimplifiedPolygonsPoints));
}
void View::uploadaKMLFile() {
  QString filePath = QFileDialog::getOpenFileName(
      nullptr, "Open File", "", "Text files (*.kml);;All files (*)");
  if (!filePath.isEmpty()) {
    // qInfo()<< "model choosed: " << filePath;
    emit fileNameChoosed(filePath);
    ui->lblChosenFilename->setText(filePath);
  }
}

void View::saveKMLFile() {
  QString pathToSave = QFileDialog::getSaveFileName(
      this, tr("Save Simplified Polygons as KML"), "simplified_polygons.kml",
      "KML Files (*.kml)");

  if (!pathToSave.isEmpty()) {
    if (!pathToSave.endsWith(".kml", Qt::CaseInsensitive)) {
      pathToSave += ".kml";
    }
    emit saveSimplifyPoligons(pathToSave);
  }
}
void View::on_btnUploadaKMLFile_clicked() { uploadaKMLFile(); }

void View::on_action_uploadaKMLFile_triggered() { uploadaKMLFile(); }
void View::on_btnSaveSimplifyPoligons_clicked() { saveKMLFile(); }

void View::on_action_saveSimplifyPoligons_triggered() { saveKMLFile(); }
void View::confirmitionExit(QCloseEvent* event) {
  event->ignore();
  if (QMessageBox::Yes ==
      QMessageBox::question(this, "Подтверждне закрытия",
                            "Вы уверены, что хотите выйти?",
                            QMessageBox::Yes | QMessageBox::No)) {
    // Если добавлю стили
    // то тут сохраняем настройки стилей...
    glwidget->update();
    event->accept();
  }
}
void View::closeEvent(QCloseEvent* event) { confirmitionExit(event); }

void View::on_action_exit_triggered() { close(); }
