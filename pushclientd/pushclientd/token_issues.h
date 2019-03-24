#ifndef TOKEN_ISSUES_H
#define TOKEN_ISSUES_H

#include <QDateTime>
#include <QVector>

namespace push_clientd {

    struct TokenIssue {
        enum class IssueState {
            Initial = 0,
            Requested,
            Confirmed,
            Error,
            Timeout
        };

        QString appId;
        QDateTime initTime;
        IssueState state;
    };

    QVector<TokenIssue> ReadTokenIssues();
    void ChangeTokenIssueState(const TokenIssue& tokenIssue, const TokenIssue::IssueState& state);

} // namespace push_clientd

#endif // TOKEN_ISSUES_H

