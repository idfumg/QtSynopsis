#ifndef METHOD_REGISTER_PHONE
#define METHOD_REGISTER_PHONE

#include <QString>
#include <memory>

class QJsonObject;

namespace push_clientd {

    struct RegisterPhone {
        QString phoneId;
        QString method;
        QString data;

        RegisterPhone(const QString& phoneId, const QString& method, const QString& data)
            : phoneId(phoneId), method(method), data(data)
        {

        }

        static QString createMessage(const QString& phoneId);
        static std::unique_ptr<RegisterPhone> parse(const QJsonObject& json);
        static bool isCanParseResponse(const QJsonObject& json);
    };

} // namespace push_clientd

#endif // METHOD_REGISTER_PHONE

