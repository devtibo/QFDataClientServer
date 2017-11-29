#include "qfdataclient.h"
// Code inspired by "Fortune Client Exemple" of Qt

QFDataClient::QFDataClient(QHostAddress ip, int port, QObject *parent) : QObject(parent)
  , tcpSocket(new QTcpSocket(this))
  , networkSession(Q_NULLPTR)
{

    qInfo("**********************************************");
    qInfo("***        Welcolme to QFDataClient        ***");
    qInfo("***              version 1.0               ***");
    qInfo("***             by Tibo, 2017              ***");
    qInfo("**********************************************\n");

    printMessage("Starting Ctient");

    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
    {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, &QNetworkSession::opened, this, &QFDataClient::sessionOpened);

        networkSession->open();
        printMessage("Opening network session.");
    }
    else
        printMessage("**WARNING** No network session was open");

    connect(tcpSocket, &QIODevice::readyRead, this, &QFDataClient::readData);
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(tcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, &QFDataClient::displayError);

    tcpSocket->abort();
    tcpSocket->connectToHost(ip,port);
}


void QFDataClient::readData()
{
    in.startTransaction();
    QByteArray rcvData;
    in >> rcvData;

    QString tmp="";
    for (int i=0; i<rcvData.size()*2 ; i=i+2)
    { tmp+= "0x"; tmp += rcvData.toHex().at(i); tmp += rcvData.toHex().at(i+1);tmp+=" ";}

    printMessage(QString("Data Received (length %1 Byte(s))").arg(rcvData.size()).toLatin1());
    printMessage("Data: " + tmp.toLatin1());

    if (!in.commitTransaction())
    {
        printMessage("**ERROR** Transaction failed!");
        return;
    }
}

void QFDataClient::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        printMessage(QString("**ERROR** The host was not found. Please check the host name and port settings.").toLatin1());
        break;
    case QAbstractSocket::ConnectionRefusedError:
        printMessage(QString("**ERROR** The connection was refused by the peer. "
                      "Make sure the fortune server is running, "
                      "and check that the host name and port "
                      "settings are correct.").toLatin1());
        break;
    default:
        printMessage(QString("**ERROR** The following error occurred: %1.")
              .arg(tcpSocket->errorString()).toLatin1());
    }
}


void QFDataClient::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    printMessage("Session is Open");
}


void QFDataClient::printMessage(QString msg)
{
    QDateTime now = QDateTime::currentDateTime();
    QString time_str = QString::asprintf("%02d:%02d:%02d::%03d",now.time().hour(),now.time().minute(),now.time().second(),now.time().msec());

    qInfo( "[QFDataClient]\t" + time_str.toLatin1() + "\t" +   msg.toLatin1());
}


