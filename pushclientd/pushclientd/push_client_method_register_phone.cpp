#include "push_client_method_register_phone.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDebug>

namespace push_clientd {

    namespace {

        const QString REGISTER_PHONE_METHOD = QStringLiteral("RegisterPhone");

    } // anonymous namespace

    QString RegisterPhone::createMessage(const QString& phoneId)
    {
        QJsonObject root;
        root["PhoneId"] = phoneId;
        root["Method"] = REGISTER_PHONE_METHOD;
        return QJsonDocument(root).toJson(QJsonDocument::Compact);
    }

    std::unique_ptr<RegisterPhone> RegisterPhone::parse(const QJsonObject& json)
    {
        const QString phoneId = json["PhoneId"].toString();
        const QString method = json["Method"].toString();
        const QString data = json.contains("Data") and json["Data"].isString()
                ? json["Data"].toString()
                : "";

        if (data.isEmpty()) {
            qDebug() << Q_FUNC_INFO << ": Warning! Wrong RegisterPhone response!";
            return nullptr;
        }

        return std::unique_ptr<RegisterPhone>(new RegisterPhone(phoneId, method, data));
    }

    bool RegisterPhone::isCanParseResponse(const QJsonObject& json)
    {
        return json["Method"].toString() == REGISTER_PHONE_METHOD;
    }

} // namespace push_clientd
