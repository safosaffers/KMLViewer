#include "view.h"
View::View(QWidget* parent) : QMainWindow(parent), ui(new Ui::View) {
  ui->setupUi(this);

  QVBoxLayout* layout = new QVBoxLayout(ui->glwidget);
  layout->setContentsMargins(0, 0, 0, 0);
  ui->glwidget->setLayout(layout);
  glwidget = new OpenGLWidget(this);
  layout->addWidget(glwidget);
  set_parameters_validators();
  // setCentralWidget(openglwidget);
}

View::~View() { delete ui; }

void View::on_btnUploadaKMLFile_clicked() {
  QString filePath = QFileDialog::getOpenFileName(
      nullptr, "Open File", "", "Text files (*.kml);;All files (*)");
  if (!filePath.isEmpty()) {
    // qInfo()<< "model choosed: " << filePath;
    emit fileNameChoosed(filePath);
  }
}
OpenGLWidget* View::getGLWidget() { return glwidget; }
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

void View::on_btnSaveSimplifyPoligons_clicked() {
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
