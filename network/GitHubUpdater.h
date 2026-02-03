#ifndef GITHUBUPDATER_H
#define GITHUBUPDATER_H

#include <QObject>
#include <QtQml/qqmlregistration.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class GitHubUpdater : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString currentVersion READ currentVersion WRITE setCurrentVersion NOTIFY currentVersionChanged)
    Q_PROPERTY(QString latestVersion READ latestVersion NOTIFY latestVersionChanged)
    Q_PROPERTY(double downloadProgress READ downloadProgress NOTIFY downloadProgressChanged)
    Q_PROPERTY(bool isDownloading READ isDownloading NOTIFY isDownloadingChanged)

public:
    explicit GitHubUpdater(QObject *parent = nullptr);

    Q_INVOKABLE void checkForUpdates(const QString &repoPath);
    Q_INVOKABLE void downloadUpdate(const QString &url);
    Q_INVOKABLE void installUpdate();

    QString currentVersion() const { return m_currentVersion; }
    void setCurrentVersion(const QString &version);

    QString latestVersion() const { return m_latestVersion; }
    double downloadProgress() const { return m_downloadProgress; }
    bool isDownloading() const { return m_isDownloading; }

signals:
    void currentVersionChanged();
    void latestVersionChanged();
    void downloadProgressChanged();
    void isDownloadingChanged();
    void updateAvailable(const QString &newVersion, const QString &downloadUrl);
    void downloadFinished(const QString &filePath);
    void errorOccurred(const QString &message);

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_currentVersion = APP_VERSION;
    QString m_latestVersion;
    QString m_downloadedFilePath;
    double m_downloadProgress = 0;
    bool m_isDownloading = false;
};

#endif // GITHUBUPDATER_H
