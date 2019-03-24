#include "push_client.h"
#include "push_client_local_server.h"
#include "push_client_method_register_phone.h"
#include "push_client_method_get_token.h"

#include <QtWebSockets/QWebSocket>

#include <QJsonObject>
#include <QJsonDocument>

#include <QTimer>

namespace push_clientd {

    namespace {

        const uint16_t REGISTER_PHONE_TIMEOUT = 1000;
        const uint16_t CONNECT_TIMEOUT = 2000;

        QString GetPhoneId()
        {
            static const QString PHONE_ID = QStringLiteral("1234567890");
            return PHONE_ID;
        }

        bool IsValidResponse(const QJsonDocument& document)
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

        bool IsErrorResponse(const QJsonObject& json)
        {
            return json.contains("Error") and json["Error"].isString();
        }

        bool IsCertificateAllowed(const QJsonObject&)
        {
            return true;
        }

//        QString JsonToString(const QJsonObject& json)
//        {
//            return QJsonDocument(json).toJson(QJsonDocument::Compact);
//        }

    } // anonymous namespace

    PushClient::PushClient(const QUrl& url, QObject *parent)
        : QObject(parent),
          m_url(url),
          m_socket(nullptr),
          m_registerState(PhoneRegisterState::NotConfirmed),
          m_connectedState(ConnectedState::NotConnected),
          m_connectTimer(new QTimer(this)),
          m_registerTimer(new QTimer(this)),
          m_localServer(new LocalServer())
    {
        qDebug() << Q_FUNC_INFO;

        connect(m_connectTimer, &QTimer::timeout, this, &PushClient::onConnectTimeout);
        connect(m_registerTimer, &QTimer::timeout, this, &PushClient::onRegisterPhoneTimeout);
        connect(m_localServer, &LocalServer::tokenIssueRequested, this, &PushClient::onTokenIssueRequested);

        onConnectTimeout();
    }

    PushClient::~PushClient()
    {
        qDebug() << Q_FUNC_INFO;
    }

    void PushClient::onTokenIssueRequested(QString appId)
    {
        qDebug() << Q_FUNC_INFO;

        sendGetTokenRequest(appId);
    }

    void PushClient::onConnected()
    {
        qDebug() << Q_FUNC_INFO;

        m_connectedState = ConnectedState::Connected;
        m_connectTimer->stop();

        sendRegisterPhoneRequest();
    }

    void PushClient::onDisconnected()
    {
        qDebug() << Q_FUNC_INFO << ": " << m_socket->errorString();

        m_connectedState = ConnectedState::NotConnected;
        m_connectTimer->start(CONNECT_TIMEOUT);
        m_registerTimer->stop();
    }

    void PushClient::onRegisterPhoneTimeout()
    {
        qDebug() << Q_FUNC_INFO;

        sendRegisterPhoneRequest();
    }

    void PushClient::onConnectTimeout()
    {
        qDebug() << Q_FUNC_INFO;

        createNewConnection();
    }

    void PushClient::onTextMessageReceived(QString message)
    {
        qDebug() << m_socket << "text message received:" << message;

        QJsonDocument document(QJsonDocument::fromJson(message.toLatin1()));
        if (not IsValidResponse(document)) {
            qDebug() << Q_FUNC_INFO << ": Warning! Wrong response from server!";
            return;
        }

        const QJsonObject& json = document.object();

        if (not IsCertificateAllowed(json)) {
            qDebug() << "Error! Wrong certificate received! Skip it...";
        }

        if (RegisterPhone::isCanParseResponse(json)) {
            handleRegisterPhoneResponse(json, message);
        }
        else if (GetToken::isCanParseResponse(json)) {
            handleGetTokenResponse(json, message);
        }
    }

    void PushClient::sendRegisterPhoneRequest()
    {
        if (m_connectedState == ConnectedState::Connected and
            m_registerState != PhoneRegisterState::Confirmed)
        {
            qDebug() << Q_FUNC_INFO;

            sendTextMessage(RegisterPhone::createMessage(GetPhoneId()));
            m_registerState = PhoneRegisterState::Requested;
            m_registerTimer->start(REGISTER_PHONE_TIMEOUT);
        }
    }

    void PushClient::sendGetTokenRequest(const QString& appId)
    {
        if (m_connectedState == ConnectedState::Connected and
            m_registerState == PhoneRegisterState::Confirmed)
        {
            qDebug() << Q_FUNC_INFO << ": 1";

            sendTextMessage(GetToken::createMessage(GetPhoneId(), appId));
        }
        else {
            qDebug() << Q_FUNC_INFO << ": 2";

            const QString reason = m_connectedState != ConnectedState::Connected
                    ? "Not connected"
                    : "Not registered";
            m_localServer->onResponseReceived(appId, "Error! Can't send get token request! " + reason);
        }
    }

    bool PushClient::sendTextMessage(const QString& text)
    {
        if (not m_socket->isValid()) {
            qDebug() << Q_FUNC_INFO << ": Warning! Socket is not valid!";
            return false;
        }

        qDebug() << Q_FUNC_INFO << ": " << text;

        m_socket->sendTextMessage(text);
        return true;
    }

    void PushClient::handleRegisterPhoneResponse(const QJsonObject& json, const QString& text)
    {
        qDebug() << Q_FUNC_INFO;

        if (m_registerState == PhoneRegisterState::Confirmed) {
            return;
        }

        if (IsErrorResponse(json)) {
            qDebug() << "Register error response! " << text;
            return;
        }

        const auto registerPhone = RegisterPhone::parse(json);
        if (registerPhone and registerPhone->data == "Registered") {
            qDebug() << "Phone registered!";
            m_registerState = PhoneRegisterState::Confirmed;
            m_registerTimer->stop();
        }
    }

    void PushClient::handleGetTokenResponse(const QJsonObject& json, const QString& text)
    {
        qDebug() << Q_FUNC_INFO;

        m_localServer->onResponseReceived(json["AppId"].toString(), text);
    }

    void PushClient::createNewConnection()
    {
        qDebug() << Q_FUNC_INFO << ": 1";

        if (m_connectedState == ConnectedState::Connected) {
            return;
        }

        qDebug() << Q_FUNC_INFO << ": 2";

        if (m_socket) {
            m_socket->close();
            m_socket->deleteLater();
        }

        m_socket = new QWebSocket("PushClient", QWebSocketProtocol::VersionLatest, this);
        connect(m_socket, &QWebSocket::textMessageReceived, this, &PushClient::onTextMessageReceived);
        connect(m_socket, &QWebSocket::connected, this, &PushClient::onConnected);
        connect(m_socket, &QWebSocket::disconnected, this, &PushClient::onDisconnected);
        m_socket->open(m_url);

        m_connectedState = ConnectedState::ConnectRequested;
        m_connectTimer->start(CONNECT_TIMEOUT);
    }

} // namespace push_clientd
