#include "SqlDatabase.h"
#include <QSqlError>
#include <QDebug>

SqlDatabase::SqlDatabase(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

SqlDatabase::~SqlDatabase()
{
    closeDatabase();
}

bool SqlDatabase::openDatabase(const QString &path)
{
    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        qWarning() << "Failed to open SQLite database:" << m_db.lastError().text();
        return false;
    }
    qDebug() << "SQLite database opened:" << path;
    return true;
}

void SqlDatabase::closeDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}
