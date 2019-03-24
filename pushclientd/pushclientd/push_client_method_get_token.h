#ifndef METHOD_GET_TOKEN
#define METHOD_GET_TOKEN

#include <QString>

class QJsonObject;

namespace push_clientd {

    struct GetToken {
        static QString createMessage(const QString& phoneId, const QString& appId);
        static bool isCanParseResponse(const QJsonObject& json);
    };

} // namespace push_clientd

#endif // METHOD_GET_TOKEN

