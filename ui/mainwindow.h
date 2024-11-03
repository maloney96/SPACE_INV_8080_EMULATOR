#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QThread>
#include "../inputmanager/inputManager.h"
#include "../emulator/emulatorWrapper.h"
#include <QMap>
#include <QOpenGLWidget>

#include "glwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onButtonPlayClicked();
    void onButtonSettingsClicked();
    void onButtonInstructionsClicked();

private:
    Ui::MainWindow *ui;
    QThread inputManagerThread;
    QThread emulatorWrapperThread;
    InputManager* inputManager;
    EmulatorWrapper* emulatorWrapper;
    bool isGameRunning = false;

    QMap<QString, int> keyMappings;
    QVector<int> keycodes;

    GLWidget *glWidget = nullptr;

    void loadKeyMappings();
    void saveKeyMappings();
    void restoreMainMenu();
    void removeOpenGLWidget();  // New function to remove OpenGL widget

};
#endif // MAINWINDOW_H
