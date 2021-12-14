#include "DataRecovery.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataRecovery w;
    w.show();
    return a.exec();
}
