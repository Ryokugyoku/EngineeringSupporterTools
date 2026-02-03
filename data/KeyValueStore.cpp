#include "KeyValueStore.h"

KeyValueStore::KeyValueStore(QObject *parent) : QObject(parent)
{
}

void KeyValueStore::set(const QString &key, const QVariant &value)
{
    m_data.insert(key, value);
}

QVariant KeyValueStore::get(const QString &key) const
{
    return m_data.value(key);
}
