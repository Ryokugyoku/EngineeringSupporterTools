#ifndef GITHUBUPDATER_H
#define GITHUBUPDATER_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

class GitHubUpdater : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit GitHubUpdater(QObject *parent = nullptr);

    Q_INVOKABLE void checkForUpdates(const QString &repoPath);
};

#endif // GITHUBUPDATER_H
