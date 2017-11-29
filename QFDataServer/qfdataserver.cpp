#include "qfdataserver.h"
// Code inspired by "Fortune Server Exemple" of Qt

#include <QDateTime>
QFDataServer::QFDataServer(QObject *parent): QObject(parent),
    tcpServer(Q_NULLPTR),
    networkSession(0)
{

    qInfo("**********************************************");
    qInfo("***        Welcolme to QFDataServer        ***");
    qInfo("***              version 1.0               ***");
    qInfo("***             by Tibo, 2017              ***");
    qInfo("**********************************************\n");

    printMessage("Starting Server");


    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
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
        networkSession = new QNetworkSession(config,this);
        connect(networkSession, &QNetworkSession::opened, this, &QFDataServer::sessionOpened);

        networkSession->open();
        printMessage("Opening network session.");
    } else {
        printMessage("**WARNING** No network session was open");
        sessionOpened();
    }

    connect(tcpServer, &QTcpServer::newConnection, this, &QFDataServer::sendData);
}

void QFDataServer::sessionOpened()
{
    // Save the used configuration
    if (networkSession) {
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
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
                ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress(ipAddress),1985)) {
        printMessage(QString("**ERROR** Unable to start the server: %1.")
                     .arg(tcpServer->errorString()).toLatin1());
        exit(0);
        return;
    }

    this->ip = QHostAddress(ipAddress);
    this->port = tcpServer->serverPort();

    printMessage("Session is Open");
    printMessage(QString("Server is on IP: %1:%2").arg(ipAddress).arg(port));
    printMessage("Server is running ...");
}

void QFDataServer::sendData()
{
    printMessage("Data send!");
    QByteArray respData;
    respData.append(0x01);
    respData.append(0x02);
    respData.append(0x03);
    respData.append(0x04);
    respData.append(0x05);
    respData.append(0x06);
    respData.append(0x07);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out.writeBytes(respData,respData.length());

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
}

void QFDataServer::printMessage(QString msg)
{
    QDateTime now = QDateTime::currentDateTime();
    QString time_str = QString::asprintf("%02d:%02d:%02d::%03d",now.time().hour(),now.time().minute(),now.time().second(),now.time().msec());

    qInfo( "[QFDataServer]\t" + time_str.toLatin1() + "\t" +   msg.toLatin1());
}
