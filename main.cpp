#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));//������ ����� ������ ��� ��������� ������� ��������
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.on_show();
    return a.exec();
}
