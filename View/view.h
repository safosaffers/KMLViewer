#ifndef VIEW_H
#define VIEW_H

#include <QDoubleValidator>
#include <QFileDialog>
#include <QMainWindow>
#include <QVBoxLayout>

#include "openglwidget.h"
#include "ui_view.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class View;
}
QT_END_NAMESPACE

class OpenGLWidget;

class View : public QMainWindow {
  Q_OBJECT

 public:
  View(QWidget* parent = nullptr);
  ~View();
  OpenGLWidget* getGLWidget();

  void updateNumberOfPolygons(int numberOfPolygons);
  void updateNumberOfPolygonsPoints(int numberOfPolygonsPoints);
  void updateNumberOfSimplifiedPolygonsPoints(
      int numberOfSimplifiedPolygonsPoints);

 private slots:
  void on_btnUploadaKMLFile_clicked();
  void on_btnSimplifyPoligons_clicked();

 private:
  Ui::View* ui;
  OpenGLWidget* glwidget;
  void set_parameters_validators();
 signals:
  /**
   * @brief Signal emitted when a file name is selected by the user
   * @param fileName Path to the selected model file
   */
  void fileNameChoosed(QString fileName);
  void polygonSimplifyRequested(double epsilon);
};
#endif  // VIEW_H
