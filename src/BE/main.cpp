#include "mainwindow.h"
#include <QApplication>
#include <qsize.h>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.resize(QSize(900, 900));
    window.show();

    return a.exec();
}
