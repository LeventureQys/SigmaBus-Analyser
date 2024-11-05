#include "HIDMessageAnalyser.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HIDMessageAnalyser w;
    w.show();
    return a.exec();
}
