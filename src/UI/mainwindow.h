#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qevent.h>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void ProgressBarInit();
    void HideAll();

private:
    Ui::MainWindow* ui;
};

#endif// MAINWINDOW_H
