#include "push_client_method_get_token.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDebug>

namespace push_clientd {

    namespace {

        const QString GET_TOKEN_METHOD = QStringLiteral("GetToken");

    } // anonymous namespace

    QString GetToken::createMessage(const QString& phoneId, const QString& appId)
    {
        QJsonObject root;
        root["AppId"] = appId;
        root["PhoneId"] = phoneId;
        root["Method"] = GET_TOKEN_METHOD;
        return QJsonDocument(root).toJson(QJsonDocument::Compact);
    }

    bool GetToken::isCanParseResponse(const QJsonObject& json)
    {
        return json.contains("AppId") and json["AppId"].isString() and json["Method"].toString() == GET_TOKEN_METHOD;
    }

} // namespace push_clientd

