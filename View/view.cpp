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
  qDebug() << "btnSimplifyPoligons was clicked!";

  if (ui->leEpsilon->hasAcceptableInput()) {
    qDebug()<< "number is correct";
  }else{
    qDebug()<< "there is no number";
  }
}

void View::set_parameters_validators() {
  auto* validator = new QDoubleValidator(1e-6, 1e9, 6, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  ui->leEpsilon->setValidator(validator);
}
