#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

class Backend : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    QML_ELEMENT

public:
    explicit Backend(QObject *parent = nullptr);

    QString message() const;
    void setMessage(const QString &message);

    Q_INVOKABLE void updateMessage();

signals:
    void messageChanged();

private:
    QString m_message;
};

#endif // BACKEND_H
