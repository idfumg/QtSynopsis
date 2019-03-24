#ifndef METHOD_NOTIFICATIONS
#define METHOD_NOTIFICATIONS

#include <QString>
#include <memory>

class QJsonObject;

namespace push_clientd {

    struct Notifications {
        QString phoneId;
        QString method;
        QString data;

        Notifications(const QString& phoneId, const QString& method, const QString& data)
            : phoneId(phoneId), method(method), data(data)
        {

        }

        static QString createMessage(const QString& phoneId);
        static std::unique_ptr<Notifications> parse(const QJsonObject& json);
        static bool isCanParseResponse(const QJsonObject& json);
    };

} // namespace push_clientd

#endif // METHOD_NOTIFICATIONS

