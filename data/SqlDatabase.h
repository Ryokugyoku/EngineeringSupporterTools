#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QtQml/qqmlregistration.h>

class SqlDatabase : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit SqlDatabase(QObject *parent = nullptr);
    ~SqlDatabase();

    Q_INVOKABLE bool openDatabase(const QString &path);
    Q_INVOKABLE void closeDatabase();

private:
    QSqlDatabase m_db;
};

#endif // SQLDATABASE_H
