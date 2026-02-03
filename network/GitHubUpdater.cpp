#include "GitHubUpdater.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QCoreApplication>

GitHubUpdater::GitHubUpdater(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
}

void GitHubUpdater::setCurrentVersion(const QString &version)
{
    if (m_currentVersion != version) {
        m_currentVersion = version;
        emit currentVersionChanged();
    }
}

void GitHubUpdater::checkForUpdates(const QString &repoPath)
{
    if (repoPath.isEmpty()) {
        emit errorOccurred("Repository path is empty");
        return;
    }

    QUrl url(QString("https://api.github.com/repos/%1/releases/latest").arg(repoPath));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "EngineeringSupporter-Updater");

    qDebug() << "Requesting update info from:" << url.toString();
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void GitHubUpdater::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Network error: %1").arg(reply->errorString()));
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    if (doc.isNull() || !doc.isObject()) {
        emit errorOccurred("Invalid JSON response from GitHub");
        return;
    }

    QJsonObject obj = doc.object();
    QString tagName = obj["tag_name"].toString();
    if (tagName.isEmpty()) {
        emit errorOccurred("Could not find tag_name in release info");
        return;
    }

    // Normalize versions for comparison (remove 'v' prefix)
    QString latest = tagName.startsWith('v') ? tagName.mid(1) : tagName;
    QString current = m_currentVersion.startsWith('v') ? m_currentVersion.mid(1) : m_currentVersion;

    m_latestVersion = tagName;
    emit latestVersionChanged();

    qDebug() << "Latest version found:" << m_latestVersion << "(Current:" << m_currentVersion << ")";

    if (latest != current) {
        QString downloadUrl;
        QJsonArray assets = obj["assets"].toArray();
        
        QStringList platformSuffixes;
#ifdef Q_OS_WIN
        platformSuffixes << "-Installer.exe" << ".zip" << ".exe";
#elif defined(Q_OS_MACOS)
        platformSuffixes << ".dmg";
#endif

        bool foundAsset = false;
        if (!assets.isEmpty()) {
            for (const QJsonValue &value : assets) {
                QString assetName = value.toObject()["name"].toString();
                for (const QString &suffix : platformSuffixes) {
                    if (assetName.endsWith(suffix, Qt::CaseInsensitive)) {
                        downloadUrl = value.toObject()["browser_download_url"].toString();
                        foundAsset = true;
                        break;
                    }
                }
                if (foundAsset) break;
            }
            
            // Fallback to the first asset if no platform match found
            if (!foundAsset) {
                downloadUrl = assets[0].toObject()["browser_download_url"].toString();
            }
        } else {
            downloadUrl = obj["html_url"].toString();
        }

        emit updateAvailable(tagName, downloadUrl);
    } else {
        qDebug() << "App is up to date.";
    }
}

void GitHubUpdater::downloadUpdate(const QString &url)
{
    if (url.isEmpty()) return;

    m_isDownloading = true;
    emit isDownloadingChanged();
    m_downloadProgress = 0;
    emit downloadProgressChanged();

    QNetworkRequest request((QUrl(url)));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true); // Follow redirects by default in newer Qt or use simple true
    request.setHeader(QNetworkRequest::UserAgentHeader, "EngineeringSupporter-Updater");

    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::downloadProgress, this, &GitHubUpdater::onDownloadProgress);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        m_isDownloading = false;
        emit isDownloadingChanged();

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred("Download failed: " + reply->errorString());
            return;
        }

        QString fileName = reply->url().fileName();
        if (fileName.isEmpty()) fileName = "update_package";
        
        QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        m_downloadedFilePath = QDir(tempPath).filePath(fileName);

        QFile file(m_downloadedFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            qDebug() << "Downloaded to:" << m_downloadedFilePath;
            emit downloadFinished(m_downloadedFilePath);
        } else {
            emit errorOccurred("Failed to save downloaded file");
        }
    });
}

void GitHubUpdater::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        m_downloadProgress = static_cast<double>(bytesReceived) / bytesTotal;
        emit downloadProgressChanged();
    }
}

void GitHubUpdater::installUpdate()
{
    if (m_downloadedFilePath.isEmpty() || !QFile::exists(m_downloadedFilePath)) {
        emit errorOccurred("No update file to install");
        return;
    }

    QString appPath = QCoreApplication::applicationFilePath();
    QString appDir = QCoreApplication::applicationDirPath();
    qint64 pid = QCoreApplication::applicationPid();

    // アップデーターのパスを取得（実行ファイルと同じディレクトリにあると仮定）
#ifdef Q_OS_WIN
    QString updaterName = "Updater.exe";
#else
    QString updaterName = "Updater";
#endif
    QString updaterPath = QDir(appDir).filePath(updaterName);

    // 開発環境など、同じディレクトリにない場合のフォールバック（ビルドディレクトリなど）
    if (!QFile::exists(updaterPath)) {
        updaterPath = QDir(appDir).filePath("../Updater"); // macOS bundle case
        if (!QFile::exists(updaterPath)) {
            emit errorOccurred("Updater utility not found at: " + updaterPath);
            return;
        }
    }

    qDebug() << "Launching updater:" << updaterPath;
    
    // 引数: <pid> <new_file> <old_file> <restart_cmd>
    QStringList args;
    args << QString::number(pid) 
         << m_downloadedFilePath 
         << appPath 
         << appPath;

    if (QProcess::startDetached(updaterPath, args)) {
        qDebug() << "Updater launched, quitting app...";
        QCoreApplication::quit();
    } else {
        emit errorOccurred("Failed to launch updater");
    }
}
