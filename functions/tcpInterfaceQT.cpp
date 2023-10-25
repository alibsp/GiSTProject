//
// Created by shahabseddigh on 10/17/23.
//

#include "../headers/tcpInterfaceQT.hpp"

tcpInterfaceQT::tcpInterfaceQT(Part& part, QString csvFile, QObject *parent) : QObject{parent}
{
    server = new QTcpServer(parent);
    commManObj = new commMan(&part, csvFile);

    connect(server, SIGNAL(newConnection()), this, SLOT(incomingUserConnectionSLOT()));
    if(runServer(10240) == 0)
    {
        printServerAddr();
    }
}

void tcpInterfaceQT::incomingUserConnectionSLOT()
{
    qDebug() << "Incoming connection!";
    QTcpSocket *clientConnection = server->nextPendingConnection();
    clientConnection->waitForReadyRead(-1);
    std::string data = clientConnection->readAll().toStdString();

    printf("data = %d, %d, %d\n", data[data.length() - 3], data[data.length() - 2], data[data.length() - 1]);
    fflush(stdout);

    std::vector<UUID> searchCommandResult;
    commManObj->commandHandler(data, searchCommandResult);

    //clientConnection->write(std::to_string(searchCommandResult.size()).c_str());
    for(UUID uuid : searchCommandResult)
    {
        char toHex[37];
        GeneralUtils::binToHexStr(uuid.val, toHex);
        clientConnection->write(toHex);
    }
    clientConnection->disconnectFromHost();
}

int tcpInterfaceQT::runServer(const quint16 port, const QHostAddress& addr)
{
    if (!server->listen(addr, port))
    {
        server->close();
        return -1;
    }
    return 0;
}

void tcpInterfaceQT::printServerAddr()
{
    QList<QHostAddress> IPv4NICs = QNetworkInterface::allAddresses();
    for(const QHostAddress& IPv4NICAddress : IPv4NICs)
    {
        printf("Listening on %s:%d\n", IPv4NICAddress.toString().toStdString().c_str(), server->serverPort());
        fflush(stdout);
    }

}