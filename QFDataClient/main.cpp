#include <QCoreApplication>
#include "qfdataclient.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFDataClient *mClient = new QFDataClient(QHostAddress("192.168.1.20"),1985);
    return a.exec();
}
