#include "Backend.h"
#include <QDateTime>

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_message("Initial message from C++")
{
}

QString Backend::message() const
{
    return m_message;
}

void Backend::setMessage(const QString &message)
{
    if (m_message == message)
        return;
    m_message = message;
    emit messageChanged();
}

void Backend::updateMessage()
{
    setMessage("Updated at: " + QDateTime::currentDateTime().toString());
}
