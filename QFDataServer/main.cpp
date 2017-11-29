#include <QCoreApplication>
#include "qfdataserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFDataServer *mServer = new QFDataServer();
    return a.exec();
}
