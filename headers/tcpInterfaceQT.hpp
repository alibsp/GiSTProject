//
// Created by shahabseddigh on 10/17/23.
//

#ifndef GISTPROJECT_TCPINTERFACEQT_HPP
#define GISTPROJECT_TCPINTERFACEQT_HPP


#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QObject>
#include "commMan.hpp"


class tcpInterfaceQT : public QObject
{
Q_OBJECT
public:
    explicit tcpInterfaceQT(Part& part, QString csvFile, QObject *parent = nullptr);

    int runServer(quint16 port, const QHostAddress& addr = QHostAddress::AnyIPv4);
    void printServerAddr();

public slots:
    void incomingUserConnectionSLOT();

private:
    QTcpServer* server;
    commMan* commManObj;

};



#endif //GISTPROJECT_TCPINTERFACEQT_HPP
