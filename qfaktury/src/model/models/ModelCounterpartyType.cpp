#include "ModelCounterpartyType.h"

ModelCounterpartyType::ModelCounterpartyType(QObject *parent) :
    QSqlTableModel(parent, QSqlDatabase::database())
{
    setTable("counterparty_type");
}

void ModelCounterpartyType::setMyCompanyVisibility(const bool yes)
{
    if(yes)
    {
        setFilter("");
    }
    else
    {
        setFilter(QString("id_counterparty_type != %1").arg(CounterpartyTypeData::MY_COMPANY + 1));
    }
    select();
}
