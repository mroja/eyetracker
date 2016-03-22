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
#ifndef ETR_WS_SERVER_H
#define ETR_WS_SERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QHostAddress)

class EyetrackerWebsocketServer : public QObject
{
    Q_OBJECT

public:
    explicit EyetrackerWebsocketServer(QObject * parent = nullptr);
    ~EyetrackerWebsocketServer();

    bool listen(const QHostAddress & address, quint16 port);

    void setDebug(bool debug = true);

    void sendMessageToAllClients(const QString & message);

signals:
    void closed();

private slots:
    void onNewConnection();
    void socketDisconnected();

private:
    QWebSocketServer * m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    bool m_debug;
};

#endif // ETR_WS_SERVER_H

