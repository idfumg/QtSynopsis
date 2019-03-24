#include "push_client_local_server.h"

#include <QFile>
#include <QFileInfo>

#include <QLocalServer>
#include <QLocalSocket>

#include <QJsonObject>
#include <QJsonDocument>

namespace push_clientd {

    namespace {

        const QString LOCAL_SERVER_ADDRESS = QStringLiteral("/tmp/push_client_request_token");

        LocalClient* FindLocalClient(const QLocalSocket* socket, QVector<LocalClient>& clients)
        {
            for (auto& client : clients) {
                if (client.socket == socket) {
                    return &client;
                }
            }

            return nullptr;
        }

        LocalClient* FindLocalClient(const QString& appId, QVector<LocalClient>& clients)
        {
            for (auto& client : clients) {
                if (client.appId == appId) {
                    return &client;
                }
            }

            return nullptr;
        }

        QString ReadAppIdFromSocket(QLocalSocket* socket)
        {
            qDebug() << Q_FUNC_INFO;

            if (not socket->isOpen()) {
                qDebug() << "Warning! socket is not open!";
                return "";
            }

            if (not socket->isReadable()) {
                qDebug() << "Warning! socket is not readable!";
                return "";
            }

            qDebug() << "bytesAvailable() = " << socket->bytesAvailable();
            const auto data = QString(socket->readAll());
            qDebug() << "data = " << data;

            QJsonDocument document(QJsonDocument::fromJson(data.toLatin1()));
            if (not document.isObject()) {
                qDebug() << "not document.isObject()";
                return "";
            }

            const QJsonObject& json = document.object();
            if (not json.contains("AppId") or not json["AppId"].isString()) {
                qDebug() << "Error! AppId is missing!";
                return "";
            }

            return json["AppId"].toString();
        }

        void DeleteFileIfExists(const QString& filename)
        {
            QFileInfo info(filename);
            if (not info.exists() or info.isDir() or info.isSymLink() or not info.isWritable()) {
                return;
            }

            QFile(filename).remove();
        }

    } // anonymous namespace

    LocalClient::LocalClient() noexcept
        : appId(), socket(nullptr), token()
    {

    }

    LocalClient::LocalClient(const QString& appId, QLocalSocket* socket, const QString& token) noexcept
        : appId(appId), socket(socket), token(token)
    {

    }

    bool LocalClient::operator==(const LocalClient& client) const noexcept
    {
        return appId == client.appId and socket == client.socket;
    }

    LocalServer::LocalServer(QObject* parent)
        : QObject(parent),
          m_server(new QLocalServer(this))
    {
        qDebug() << Q_FUNC_INFO;

        DeleteFileIfExists(LOCAL_SERVER_ADDRESS);

        if (not m_server->listen(LOCAL_SERVER_ADDRESS)) {
            qDebug() << Q_FUNC_INFO << ": listen error";
            return;
        }

        m_server->setMaxPendingConnections(100);

        connect(m_server, &QLocalServer::newConnection, this, &LocalServer::onNewConnection);
    }

    LocalServer::~LocalServer()
    {
        qDebug() << Q_FUNC_INFO;

        DeleteFileIfExists(LOCAL_SERVER_ADDRESS);
    }

    void LocalServer::onNewConnection()
    {
        qDebug() << Q_FUNC_INFO;

        QLocalSocket* socket = m_server->nextPendingConnection();
        connect(socket, &QLocalSocket::readyRead, this, &LocalServer::onReadyRead);
        connect(socket, &QLocalSocket::bytesWritten, this, &LocalServer::onBytesWritten);
        connect(socket, &QLocalSocket::disconnected, this, &LocalServer::onDisconnected);
    }

    void LocalServer::onReadyRead()
    {
        qDebug() << Q_FUNC_INFO;

        QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());

        const auto appId = ReadAppIdFromSocket(socket);

        if (appId.isEmpty()) {
            qDebug() << "Warning! appId is empty!";
            socket->disconnectFromServer();
            return;
        }

        m_clients.append(LocalClient(appId, socket));

        emit tokenIssueRequested(appId);
    }

    void LocalServer::onBytesWritten(qint64 bytes)
    {
        qDebug() << Q_FUNC_INFO;

        QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
        const LocalClient* client = FindLocalClient(socket, m_clients);
        if (client) {
            if (bytes != client->token.size()) {
                qDebug() << "Warning! Not all bytes written yet!";
                return;
            }
            client->socket->deleteLater();
            m_clients.removeAll(*client);
        }
    }

    void LocalServer::onDisconnected()
    {
        qDebug() << Q_FUNC_INFO;

        QLocalSocket* socket = qobject_cast<QLocalSocket*>(sender());
        const LocalClient* client = FindLocalClient(socket, m_clients);
        if (client) {
            m_clients.removeAll(*client);
        }
        socket->deleteLater();
    }

    void LocalServer::onResponseReceived(QString appId, QString token)
    {
        qDebug() << Q_FUNC_INFO;

        LocalClient* client = FindLocalClient(appId, m_clients);
        if (client) {
            client->token = token;
            if (client->socket->isWritable()) {
                client->socket->write(token.toLatin1());
                client->socket->flush();
            }
            else {
                if (client->socket->isOpen()) {
                    client->socket->close();
                }
            }
        }
    }

} // namespace push_clientd
