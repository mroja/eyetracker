/*
 * This file is part of PISAK project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ws_server.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "QtWebSockets/qwebsocket.h"
#include <QtCore/QDebug>

EyetrackerWebsocketServer::EyetrackerWebsocketServer(QObject * parent)
    : QObject(parent)
    , m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Eyetracker Server"),
                                              QWebSocketServer::NonSecureMode, this))
    , m_clients()
    , m_debug(false)
{
}

bool EyetrackerWebsocketServer::listen(const QHostAddress & address, quint16 port)
{
    if(m_pWebSocketServer->listen(address, port))
    {
        if(m_debug)
            qDebug() << "Websocket server listening on" << (address.toString() + ":" + QString::number(port));
        
        connect(m_pWebSocketServer, 
                &QWebSocketServer::newConnection,
                this, 
                &EyetrackerWebsocketServer::onNewConnection
        );
        
        connect(m_pWebSocketServer, 
                &QWebSocketServer::closed, 
                this, 
                &EyetrackerWebsocketServer::closed
        );

        return true;
    }
    else
    {
        if(m_debug)
            qDebug() << "Error listening on on" << (address.toString() + ":" + QString::number(port));

        return false;
    }
}

EyetrackerWebsocketServer::~EyetrackerWebsocketServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void EyetrackerWebsocketServer::setDebug(bool debug)
{
    m_debug = debug;
}

void EyetrackerWebsocketServer::sendMessageToAllClients(const QString & message)
{
    for(QWebSocket * client : m_clients)
        client->sendTextMessage(message);
}

void EyetrackerWebsocketServer::onNewConnection()
{
    QWebSocket * pSocket = m_pWebSocketServer->nextPendingConnection();
    connect(pSocket, &QWebSocket::disconnected, this, &EyetrackerWebsocketServer::socketDisconnected);
    m_clients << pSocket;
}

void EyetrackerWebsocketServer::socketDisconnected()
{
    QWebSocket * pClient = qobject_cast<QWebSocket *>(sender());
    if(m_debug)
        qDebug() << "socketDisconnected:" << pClient;
    
    if(pClient) 
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}

