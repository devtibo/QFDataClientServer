#ifndef QFSERVER_H
#define QFSERVER_H
#include <QtNetwork>

QT_BEGIN_NAMESPACE
class QTcpServer;
class QNetworkSession;
QT_END_NAMESPACE

class QFDataServer : public QObject
{
    Q_OBJECT

public:
    QFDataServer(QObject *parent = Q_NULLPTR);
    QTcpServer *tcpServer;
    QNetworkSession *networkSession;

    QHostAddress ip;
    int port;

private:
    void printMessage(QString);

private slots:
    void sessionOpened();
    void sendData();
};

#endif // QFSERVER_H
