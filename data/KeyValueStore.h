#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H

#include <QObject>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

/**
 * @brief RedisのようなNoSQL（KVS）のシミュレーションクラス
 * 実運用では実際のRedisクライアントや、組み込みKVS（LevelDB, RocksDB等）へのアダプタになります。
 */
class KeyValueStore : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit KeyValueStore(QObject *parent = nullptr);

    Q_INVOKABLE void set(const QString &key, const QVariant &value);
    Q_INVOKABLE QVariant get(const QString &key) const;

private:
    QVariantMap m_data; // メモリ内KVSのシミュレーション
};

#endif // KEYVALUESTORE_H
