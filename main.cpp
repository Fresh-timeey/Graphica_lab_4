#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

   w.showMaximized();
  //    w.showFullScreen(); // Показывает окно в полноэкранном режиме
    return a.exec();
}
