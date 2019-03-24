#ifndef PUSHSERVER
#define PUSHSERVER

#include <QObject>

class QWebSocketServer;
class QWebSocket;

class PushServer : public QObject {
    Q_OBJECT

public:
    PushServer(quint16 port, QObject* parent = nullptr);

public slots:
    void onNewConnection();
    void onClosed();
    void onTextMessageReceived(QString message);
    void onSocketDisconnected();

private:
    QWebSocketServer* const m_server;
    QList<QWebSocket*> m_clients;
};

#endif // PUSHSERVER

