#include <QCoreApplication>

#include "pushserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PushServer* server = new PushServer(1234);
    Q_UNUSED(server);

    return a.exec();
}

