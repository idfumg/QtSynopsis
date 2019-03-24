#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    QLibrary lib("/home/idfumg/pushclientd/build-localClient-Desktop-Debug/localClient");
//    lib.load();

//    if (not lib.isLoaded()) {
//        qDebug() << "can not load library! " << lib.errorString();
//        return -1;
//    }

//    typedef void (*Fn)();
//    Fn fn = (Fn) lib.resolve("fn");
//    if (fn) {
//        fn();
//    }
//    else {
//        qDebug() << "can not find function!";
//        return -1;
//    }

    void *handle;
    double (*cosine)(double);
    char *error;

    handle = dlopen ("/home/idfumg/pushclientd/build-localClient-Desktop-Debug/localClient", RTLD_LAZY);
    if (!handle) {
        fputs (dlerror(), stderr);
        exit(1);
    }

    return a.exec();
}

