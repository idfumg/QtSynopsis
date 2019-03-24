#include <QCoreApplication>
#include <QUrl>

#include "push_client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto c = new push_clientd::PushClient(QUrl(QStringLiteral("ws://localhost:1234")), &a);
    Q_UNUSED(c);

    return a.exec();
}

