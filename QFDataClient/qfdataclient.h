#ifndef QFCLIENT_H
#define QFCLIENT_H

#include <QTcpSocket>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QNetworkSession;
QT_END_NAMESPACE

class QFDataClient : public QObject
{
    Q_OBJECT
public:
    explicit QFDataClient(QHostAddress, int, QObject *parent = 0);

private :
    QTcpSocket *tcpSocket;
    QDataStream in;
    QNetworkSession *networkSession;
    void printMessage(QString msg);

private slots:
    void sessionOpened();
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);

signals:

public slots:
};

#endif // QFCLIENT_H
