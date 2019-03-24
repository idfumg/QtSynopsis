#include "pushserver.h"

#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace {

    const QString REGISTER_PHONE_METHOD = QStringLiteral("RegisterPhone");
    const QString GET_TOKEN_METHOD = QStringLiteral("GetToken");

    bool IsRegisterPhoneRequest(const QJsonObject& json)
    {
        return json["Method"].toString() == REGISTER_PHONE_METHOD;
    }

    bool IsGetTokenRequest(const QJsonObject& json)
    {
        return json["Method"].toString() == GET_TOKEN_METHOD;
    }

    bool IsValidRequest(const QJsonDocument& document)
    {
        if (not document.isObject()) {
            return false;
        }

        const QJsonObject& json = document.object();

        if (not json.contains("PhoneId") or not json.contains("Method")) {
            return false;
        }

        if (not json["PhoneId"].isString() or not json["Method"].isString()) {
            return false;
        }

        if (json["PhoneId"].toString().isEmpty() or json["Method"].toString().isEmpty()) {
            return false;
        }

        return true;
    }

    QString CreateErrorMessage(const QString& msg)
    {
        QJsonObject root;
        root["Error"] = msg;
        return QJsonDocument(root).toJson(QJsonDocument::Compact);
    }

    QString CreateRegisterPhoneResponse(const QJsonObject& json)
    {
        QJsonObject root;
        root["PhoneId"] = json["PhoneId"];
        root["Method"] = json["Method"];
        root["Data"] = "Registered";
        return QJsonDocument(root).toJson(QJsonDocument::Compact);
    }

    QString CreateGetTokenResponse(const QJsonObject& json)
    {
        QJsonObject root;
        root["PhoneId"] = json["PhoneId"];
        root["Method"] = json["Method"];
        root["AppId"] = json["AppId"];
        root["Data"] = "AAAABBBBCCCDDD";
        return QJsonDocument(root).toJson(QJsonDocument::Compact);
    }

} // anonymous namespace

PushServer::PushServer(quint16 port, QObject *parent)
    : QObject(parent),
      m_server(new QWebSocketServer(QStringLiteral("PushServer"), QWebSocketServer::NonSecureMode, this))
{
    if (m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "new server created on port" << port;
        connect(m_server, &QWebSocketServer::newConnection, this, &PushServer::onNewConnection);
        connect(m_server, &QWebSocketServer::closed, this, &PushServer::onClosed);
    }
}

void PushServer::onNewConnection()
{
    qDebug() << "new connection established";
    QWebSocket *socket = m_server->nextPendingConnection();
    if (socket) {
        connect(socket, &QWebSocket::textMessageReceived, this, &PushServer::onTextMessageReceived);
        connect(socket, &QWebSocket::disconnected, this, &PushServer::onSocketDisconnected);
        m_clients << socket;
    }
}

void PushServer::onClosed()
{
    qDebug() << "push server closed";
}

void PushServer::onTextMessageReceived(QString message)
{
    qDebug() << "message received =" << message;
    QWebSocket* const socket = qobject_cast<QWebSocket*>(sender());

    QJsonDocument document(QJsonDocument::fromJson(message.toLatin1()));
    if (not IsValidRequest(document)) {
        qDebug() << Q_FUNC_INFO << ": Warning! Wrong response from client!";
        socket->sendTextMessage(CreateErrorMessage("Not a valid request!"));
        return;
    }

    const QJsonObject& json = document.object();
    if (IsRegisterPhoneRequest(json)) {
        socket->sendTextMessage(CreateRegisterPhoneResponse(json));
    }
    else if (IsGetTokenRequest(json)) {
        socket->sendTextMessage(CreateGetTokenResponse(json));
    }
    else {
        qDebug() << Q_FUNC_INFO << ": Warning! Unknown request!";
        socket->sendTextMessage(CreateErrorMessage("Warning! Unknown request!"));
    }
}

void PushServer::onSocketDisconnected()
{
    QWebSocket* const socket = qobject_cast<QWebSocket*>(sender());
    if (socket) {
        m_clients.removeAll(socket);
        socket->deleteLater();
        qDebug() << "socket disconnected";
    }
}
