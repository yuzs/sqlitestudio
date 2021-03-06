#include "sqlitecopy.h"
#include "sqlitequerytype.h"

#include <parser/statementtokenbuilder.h>

SqliteCopy::SqliteCopy()
{
    queryType = SqliteQueryType::Copy;
}

SqliteCopy::SqliteCopy(const SqliteCopy& other) :
    SqliteQuery(other), onConflict(other.onConflict), database(other.database), table(other.table), file(other.file), delimiter(other.delimiter)
{
}

SqliteCopy::SqliteCopy(SqliteConflictAlgo onConflict, const QString &name1, const QString &name2, const QString &name3, const QString &delim)
    : SqliteCopy()
{
    this->onConflict = onConflict;

    if (!name2.isNull())
    {
        database = name1;
        table = name2;
    }
    else
        table = name1;

    file = name3;
    delimiter = delim;
}

SqliteStatement* SqliteCopy::clone()
{
    return new SqliteCopy(*this);
}

QStringList SqliteCopy::getTablesInStatement()
{
    return getStrListFromValue(table);
}

QStringList SqliteCopy::getDatabasesInStatement()
{
    return getStrListFromValue(database);
}

TokenList SqliteCopy::getTableTokensInStatement()
{
    return getObjectTokenListFromNmDbnm();
}

TokenList SqliteCopy::getDatabaseTokensInStatement()
{
    return getDbTokenListFromNmDbnm();
}

QList<SqliteStatement::FullObject> SqliteCopy::getFullObjectsInStatement()
{
    QList<FullObject> result;

    FullObject fullObj = getFullObjectFromNmDbnm(FullObject::TABLE);
    if (fullObj.isValid())
        result << fullObj;

    fullObj = getFirstDbFullObject();
    if (fullObj.isValid())
        result << fullObj;

    return result;
}

TokenList SqliteCopy::rebuildTokensFromContents()
{
    StatementTokenBuilder builder;
    builder.withTokens(SqliteQuery::rebuildTokensFromContents());
    builder.withKeyword("COPY").withSpace();
    if (onConflict != SqliteConflictAlgo::null)
        builder.withKeyword("OR").withSpace().withKeyword(sqliteConflictAlgo(onConflict)).withSpace();

    if (!database.isNull())
        builder.withOther(database).withSpace();

    builder.withOther(table).withSpace().withKeyword("FROM").withSpace().withString(file);

    if (!delimiter.isNull())
        builder.withSpace().withKeyword("USING").withSpace().withKeyword("DELIMITERS").withSpace().withString(delimiter);

    builder.withOperator(";");

    return builder.build();
}
