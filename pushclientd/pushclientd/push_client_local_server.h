#ifndef LOCALCONNECTIONSSERVER
#define LOCALCONNECTIONSSERVER

#include <QObject>
#include <QVector>

class QLocalServer;
class QLocalSocket;

namespace push_clientd {

    class LocalClient {
    public:
        LocalClient() noexcept;
        LocalClient(const QString& appId, QLocalSocket* socket, const QString& token = "") noexcept;
        LocalClient(const LocalClient& client) = default;
        LocalClient(LocalClient&& client) = default;
        LocalClient& operator=(const LocalClient& client) = default;
        LocalClient& operator=(LocalClient&& client) = default;
        bool operator==(const LocalClient& client) const noexcept;

    public:
        QString appId;
        QLocalSocket* socket;
        QString token;
    };

    class LocalServer : public QObject {
        Q_OBJECT

    public:
        LocalServer(QObject* parent = nullptr);
        LocalServer(const LocalServer& server) = delete;
        LocalServer(LocalServer&& server) = delete;
        LocalServer& operator=(const LocalServer& server) = delete;
        LocalServer& operator=(LocalServer&& server) = delete;
        ~LocalServer();

    public slots:
        void onNewConnection();
        void onReadyRead();
        void onBytesWritten(qint64 bytes);
        void onDisconnected();
        void onResponseReceived(QString appId, QString token);

    signals:
        void finished();
        void tokenIssueRequested(QString appId);

    private:
        QLocalServer* m_server;
        QVector<LocalClient> m_clients;
    };

} // namespace push_clientd

#endif // LOCALCONNECTIONSSERVER

