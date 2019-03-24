#include <token_issues.h>

namespace push_clientd {

    namespace {

    QVector<TokenIssue>& TokenIssuesContainer()
    {
        static QVector<TokenIssue> tokenIssues;

        if (tokenIssues.empty()) {
            const auto tokenIssue = TokenIssue {
                "001",
                QDateTime(),
                TokenIssue::IssueState::Requested
            };

            tokenIssues << tokenIssue;
        }

        return tokenIssues;
    }

    } // anonymous namespace

    QVector<TokenIssue> ReadTokenIssues()
    {
        return TokenIssuesContainer();
    }

    void ChangeTokenIssueState(const TokenIssue& tokenIssue_, const TokenIssue::IssueState& state)
    {
        for (auto& tokenIssue : TokenIssuesContainer()) {
            if (tokenIssue.appId == tokenIssue_.appId) {
                tokenIssue.state = state;
                break;
            }
        }
    }

} // namespace push_clientd
