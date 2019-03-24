#include <QCoreApplication>
#include <QLocalSocket>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <stdio.h>

extern "C" {

    void fn()
    {
        printf("fn called");
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QLocalSocket* socket = new QLocalSocket(&a);
    socket->connectToServer("/tmp/push_client_request_token");
    if (not socket->waitForConnected()) {
        qDebug() << "waitForConnected:" << socket->errorString();
    }

    QJsonObject root;
    root["AppId"] = "0001";
    const QString data = QJsonDocument(root).toJson(QJsonDocument::Compact);

    socket->write(data.toLatin1());
    if (not socket->waitForBytesWritten()) {
        qDebug() << "waitForBytesWritten:" << socket->errorString();
        return -1;
    }

    if (not socket->waitForReadyRead(10000)) {
        qDebug() << "waitForReadyRead:" << socket->errorString();
        return -1;
    }

    const QString response = socket->readAll();

    qDebug() << "response = " << response;

    return a.exec();
}

