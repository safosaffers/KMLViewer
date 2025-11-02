#ifndef VIEW_H
#define VIEW_H

#include <QDoubleValidator>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
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
  Ui::View* ui;
  View(QWidget* parent = nullptr);
  ~View();
  OpenGLWidget* getGLWidget();
  void setSaveSimplificationPolygonsAvailable(bool flag);
  void setSimplificationAvailable(bool flag);
  void clearPolygonStats();
  void clearViewData();
  void showMessageError(QString text);
  void updateNumberOfPolygons(int numberOfPolygons);
  void updateNumberOfPolygonsPoints(int numberOfPolygonsPoints);
  void updateNumberOfSimplifiedPolygonsPoints(
      int numberOfSimplifiedPolygonsPoints);

 private:
  void saveKMLFile();
  void uploadaKMLFile();
  void confirmitionExit(QCloseEvent* event);
  void closeEvent(QCloseEvent* event);
 private slots:
  void on_btnUploadaKMLFile_clicked();
  void on_btnSimplifyPoligons_clicked();

  void on_btnSaveSimplifyPoligons_clicked();

  void on_action_uploadaKMLFile_triggered();

  void on_action_saveSimplifyPoligons_triggered();

  void on_action_exit_triggered();

 private:
  OpenGLWidget* glwidget;
  void set_parameters_validators();
 signals:
  /**
   * @brief Signal emitted when a file name is selected by the user
   * @param fileName Path to the selected model file
   */
  void fileNameChoosed(QString fileName);
  void polygonSimplifyRequested(double epsilon);
  void saveSimplifyPoligons(QString fileName);
};
#endif  // VIEW_H
