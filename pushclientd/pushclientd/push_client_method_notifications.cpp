#include "push_client_method_notifications.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDebug>

namespace push_clientd {

    namespace {

        const QString NOTIFICATIONS_METHOD = QStringLiteral("Notifications");

    } // anonymous namespace

    QString Notifications::createMessage(const QString& phoneId)
    {
        QJsonObject root;
        root["PhoneId"] = phoneId;
        root["Method"] = NOTIFICATIONS_METHOD;
        return QJsonDocument(root).toJson(QJsonDocument::Compact);
    }

    std::unique_ptr<Notifications> Notifications::parse(const QJsonObject& json)
    {
        const QString phoneId = json["PhoneId"].toString();
        const QString method = json["Method"].toString();
        const QString data = json.contains("Data") and json["Data"].isObject()
                ? QJsonDocument(json["Data"].toObject()).toJson(QJsonDocument::Compact)
                : "";

        if (data.isEmpty()) {
            qDebug() << Q_FUNC_INFO << ": Warning! Wrong notifications response!";
            return nullptr;
        }

        return std::unique_ptr<Notifications>(new Notifications(phoneId, method, data));
    }

    bool Notifications::isCanParseResponse(const QJsonObject& json)
    {
        return json["Method"].toString() == NOTIFICATIONS_METHOD;
    }

} // namespace push_clientd

