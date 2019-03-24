#ifndef PUSHCLIENT
#define PUSHCLIENT

#include <QObject>
#include <QUrl>

class QWebSocket;
class QTimer;
class QJsonObject;

namespace push_clientd {

    class LocalServer;

    class PushClient : public QObject {
        Q_OBJECT

    public:
        enum class PhoneRegisterState {
            NotConfirmed = 0,
            Requested,
            Confirmed
        };

        enum class ConnectedState {
            NotConnected = 0,
            ConnectRequested,
            Connected
        };

    public:
        PushClient(const QUrl& url, QObject* parent = nullptr);
        PushClient(const PushClient& pushClient) = delete;
        PushClient(PushClient&& pushClient) = delete;
        PushClient& operator=(const PushClient& pushClient) = delete;
        PushClient& operator=(PushClient&& pushClient) = delete;
        ~PushClient();

    public slots:
        void onConnected();
        void onDisconnected();
        void onTextMessageReceived(QString message);
        void onRegisterPhoneTimeout();
        void onConnectTimeout();
        void onTokenIssueRequested(QString appId);

    private:
        void createNewConnection();
        void sendRegisterPhoneRequest();
        void sendGetTokenRequest(const QString& appId);
        void handleRegisterPhoneResponse(const QJsonObject& json, const QString& text);
        void handleGetTokenResponse(const QJsonObject& json, const QString& text);
        bool sendTextMessage(const QString& text);

    private:
        QUrl m_url;
        QWebSocket* m_socket;
        PhoneRegisterState m_registerState;
        ConnectedState m_connectedState;
        QTimer* m_connectTimer;
        QTimer* m_registerTimer;
        LocalServer* m_localServer;
    };

} // namespace push_clientd

#endif // PUSHCLIENT

