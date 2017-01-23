#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));//задаем кодек текста для поддержки русских символов
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.on_show();
    return a.exec();
}
