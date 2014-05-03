#include <QTest>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <QTime>
#include <QMap>

#include "Database.h"
#include "SettingsGlobal.h"
#include "CommodityData.h"
#include "CommodityTypeData.h"
#include "PaymentTypeData.h"
#include "UnitData.h"
#include "../../TestsCommon/TestsCommon.h"
#include "../../TestsCommon/InvoiceDialogPublic.h"
#include "../../TestsCommon/GuiUserAddCommodity.h"
#include "../../TestsCommon/GuiUserAddNewCommodity.h"
#include "../../TestsCommon/GuiUserAddCounterparty.h"
#include "../../TestsCommon/Counterparty_t.h"
#include "ModelCommodity.h"
#include "ModelCounterparty.h"
#include "ModelCountry.h"
#include "ModelInvoice.h"
#include "ModelInvoiceWithCommodities.h"
#include "Money_t.h"
#include "CounterpartyTypeData.h"


class InvoiceDialogTest : public QObject
{
    Q_OBJECT
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testGUI_InitialState();
    void testGUI_AddDeleteCommodities();
    void testGUI_AddDeleteCommodities_data();

private:
    void addCommodityInThread(InvoiceDialogPublic *idp, const CommodityData &cd, const int netValIndex);
    void addCounterpartyInThread(InvoiceDialogPublic *idp, const Counterparty_t &counterparty);
    void addNewCommodityInThread(InvoiceDialogPublic *idp, const CommodityData &cd);
    void checkLastInsertedCommodity(const CommodityData &cd);
    Money_t computeNetVal(const CommodityData &cd, const int netValIndex) const;
    void startUserThread(GuiUser *guiUser, QThread *thread, QPushButton *buttonStart) const;

    CommodityData createNewCommodity(const int invoiceIndex, const int commodityIndex) const;
    Counterparty_t createNewCounterparty(const int invoiceIndex) const;

private:
    Database *db_;
};


void InvoiceDialogTest::initTestCase()
{
    TestsCommon::setAppData();
    SettingsGlobal s;
    s.setFirstRun(true);
    qsrand(QTime::currentTime().msec());
    TestsCommon::removeDBFile();
    db_ = new Database();
}


void InvoiceDialogTest::cleanupTestCase()
{
    delete db_;
}


void InvoiceDialogTest::testGUI_InitialState()
{
    SettingsGlobal s;
    InvoiceDialogPublic invoiceDialog(0, db_, InvoiceTypeData::VAT);

    QCOMPARE(invoiceDialog.windowTitle(), trUtf8("Nowy dokument - %1 [*]").arg(InvoiceTypeData::name(InvoiceTypeData::VAT)));
    QVERIFY( ! invoiceDialog.ui()->lineEditInvNumFormat->text().isEmpty());
    QCOMPARE(invoiceDialog.ui()->dateEditDateOfIssuance->date(), QDate::currentDate());
    QCOMPARE(invoiceDialog.ui()->comboBoxInvoiceType->currentText(), InvoiceTypeData::name(InvoiceTypeData::VAT));
    QVERIFY( ! invoiceDialog.ui()->lineEditInvNumber->text().isEmpty());
    QVERIFY( ! invoiceDialog.ui()->pushButtonMoreInfo->isEnabled());

    QCOMPARE(invoiceDialog.ui()->tableWidgetCommodities->rowCount(), 0);
    const int maxCols = invoiceDialog.ui()->tableWidgetCommodities->columnCount();
    QVERIFY(invoiceDialog.ui()->tableWidgetCommodities->horizontalHeaderItem(0));
    QCOMPARE(maxCols, CommodityVisualFields::DISCOUNT - CommodityVisualFields::ID + 1);

    for(int header = 0; header < maxCols; ++header)
    {
        QCOMPARE(invoiceDialog.ui()->tableWidgetCommodities->horizontalHeaderItem(header)->text(), CommodityVisualData::header((CommodityVisualFields::Field)header));
    }

    QCOMPARE(invoiceDialog.ui()->checkBoxDiscount->isChecked(), false);
    QCOMPARE(invoiceDialog.ui()->spinBoxDiscount->value(), 0);

    QCOMPARE(invoiceDialog.ui()->dateEditDateOfSell->date(), QDate::currentDate());
    QCOMPARE(invoiceDialog.ui()->dateEditDayOfPayment->date(), QDate::currentDate());
    QCOMPARE(invoiceDialog.ui()->tableWidgetCommodities->rowCount(), 0);
    QVERIFY(invoiceDialog.ui()->comboBoxReasonOfCorrection->currentText().isEmpty());
    QCOMPARE(invoiceDialog.ui()->comboBoxPayment->currentText(), PaymentTypeData::name(PaymentTypeData::CASH));
    QCOMPARE(invoiceDialog.ui()->comboBoxCurrency->currentText(), CurrencyData::codeName((CurrencyData::Currencies)s.value(s.DEFAULT_CURRENCY).value<int>()));

    QLocale locale;
    QCOMPARE(invoiceDialog.ui()->labelSumNetVal->text(), locale.toString(0.0, 'f', 2));
    QCOMPARE(invoiceDialog.ui()->labelDiscountVal->text(), locale.toString(0.0, 'f', 2));
    QCOMPARE(invoiceDialog.ui()->labelSumGrossVal->text(), locale.toString(0.0, 'f', 2));

    QVERIFY(invoiceDialog.ui()->lineEditAdditionalText->text() == s.value(s.ADDIT_TEXT).toString());
}



void InvoiceDialogTest::testGUI_AddDeleteCommodities()
{
    QFETCH(QList<CommodityData>, lcd);
    QFETCH(Counterparty_t, counterparty);
    QFETCH(QString, invoiceNumber);
    QFETCH(InvoiceTypeData::Type, invoiceType);
    QFETCH(QList<int>, netValIndices);
    QFETCH(Money_t::val_t, discount);
    QFETCH(Money_t, totalNetVal);
    QFETCH(Money_t, totalDiscountVal);
    QFETCH(Money_t, totalGrossVal);

    const int precision = 2;
    InvoiceDialogPublic invD(0, db_, invoiceType);
    invD.show();
    for(int i = 0; i < lcd.size(); ++i)
    {
        addNewCommodityInThread(&invD, lcd.at(i));
        QVERIFY(db_->modelCommodity()->submitAll());
        checkLastInsertedCommodity(lcd.at(i));

        addCommodityInThread(&invD, lcd.at(i), netValIndices.at(i));

        QTableWidgetItem *item = 0;
        invD.ui()->tableWidgetCommodities->selectRow(i);
        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::ID);
        QVERIFY(item != NULL);
        QCOMPARE(item->text().toLongLong(), lcd.at(i).field(CommodityFields::ID_COMMODITY).toLongLong());

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::NAME);
        QVERIFY(item != NULL);
        QCOMPARE(item->text(), lcd.at(i).field(CommodityFields::NAME).toString());

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::QUANTITY);
        QVERIFY(item != NULL);
        QCOMPARE(item->text().toDouble(), lcd.at(i).field(CommodityFields::QUANTITY).value<Money_t::val_t>().get_d());

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::UNIT);
        QVERIFY(item != NULL);
        QCOMPARE(item->text(), UnitData::name((UnitData::Name)lcd.at(i).field(CommodityFields::UNIT_ID).toInt()));

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::PKWIU);
        QVERIFY(item != NULL);
        QCOMPARE(item->text(), lcd.at(i).field(CommodityFields::PKWIU).toString());

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::NET);
        QVERIFY(item != NULL);
        QCOMPARE(item->text(), lcd.at(i).field((CommodityFields::Field)(CommodityFields::NET1 + netValIndices.at(i))).value<Money_t>().toString(precision));

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::TYPE);
        QVERIFY(item != NULL);
        QCOMPARE(item->text(), CommodityTypeData::name((CommodityTypeData::CommodityType)lcd.at(i).field(CommodityFields::TYPE_ID).toInt()));

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::VAT);
        QVERIFY(item != NULL);
        QCOMPARE(item->text().toDouble(), lcd.at(i).field(CommodityFields::VAT).value<Money_t::val_t>().get_d());

        item = invD.ui()->tableWidgetCommodities->item(i, CommodityVisualFields::DISCOUNT);
        QVERIFY(item != NULL);
        QCOMPARE(item->text(), QString("0"));


        QCOMPARE(invD.ui()->labelSumNetVal->text(), totalNetVal.toString(precision));
        QCOMPARE(invD.ui()->labelDiscountVal->text(), totalDiscountVal.toString(precision));
        QCOMPARE(invD.ui()->labelSumGrossVal->text(), totalGrossVal.toString(precision));
    }

    QCOMPARE(invD.ui()->tableWidgetCommodities->rowCount(), lcd.size());

    addCounterpartyInThread(&invD, counterparty);
    QVERIFY(db_->modelCounterparty()->submitAll());

    QVERIFY(invD.ui()->pushButtonMoreInfo->isEnabled());

    const int indNewCounterparty = invD.ui()->comboBoxCounterparties->findText(counterparty.name);
    QVERIFY(indNewCounterparty != -1);
    invD.ui()->comboBoxCounterparties->setCurrentIndex(indNewCounterparty);
    QCOMPARE(invD.ui()->comboBoxCounterparties->currentIndex(), indNewCounterparty);

    QTest::mouseClick(invD.ui()->pushButtonSave, Qt::LeftButton);
    QTest::mouseClick(invD.ui()->pushButtonClose, Qt::LeftButton);

    QVERIFY(db_->modelInvoice()->submitAll());

    for(int i = 0; i < lcd.size(); ++i)
    {
        QSqlQuery queryCommod(db_->modelCommodity()->query());
        queryCommod.exec(QString("SELECT quantity FROM commodity WHERE name = '%1'").arg(lcd.at(i).field(CommodityFields::NAME).toString()));
        if(!queryCommod.isActive())
        {
            qDebug() << "InvoiceDialogTest::testGUI_AddDeleteCommodities_data(): first SQL query failed. Reason" << queryCommod.lastError().text() << "\nQuery: " << queryCommod.lastQuery();
        }
        QVERIFY(queryCommod.isActive());
        QVERIFY(queryCommod.next());
        QCOMPARE(queryCommod.value(0).value<Money_t::val_t>().get_d(), 0.0);
    }

    QSqlQuery queryInvoice(db_->modelInvoice()->query());
    queryInvoice.exec(QString("SELECT inv_number FROM invoice WHERE counterparty_id = (SELECT id_counterparty FROM counterparty WHERE name = '%1')").arg(counterparty.name));
    if(!queryInvoice.isActive())
    {
        qDebug() << "InvoiceDialogTest::testGUI_AddDeleteCommodities(): second SQL query failed. Reason" << queryInvoice.lastError().text() << "\nQuery: " << queryInvoice.lastQuery();
    }
    QVERIFY(queryInvoice.isActive());
    QVERIFY(queryInvoice.next());
    QCOMPARE(queryInvoice.value(0).toString(), invoiceNumber);

    queryInvoice.exec(QString("SELECT invoice_type FROM invoice JOIN invoice_type ON invoice.type_id = invoice_type.id_invoice_type WHERE invoice.counterparty_id = (SELECT id_counterparty FROM counterparty WHERE name = '%1')").arg(counterparty.name));
    if(!queryInvoice.isActive())
    {
        qDebug() << "InvoiceDialogTest::testGUI_AddDeleteCommodities(): third SQL query failed. Reason" << queryInvoice.lastError().text() << "\nQuery: " << queryInvoice.lastQuery();
    }
    QVERIFY(queryInvoice.isActive());
    QVERIFY(queryInvoice.next());
    QCOMPARE(queryInvoice.value(0).toString(), InvoiceTypeData::name(invoiceType));
}


void InvoiceDialogTest::testGUI_AddDeleteCommodities_data()
{
    QTest::addColumn<QList<CommodityData> >("lcd");
    QTest::addColumn<Counterparty_t>("counterparty");
    QTest::addColumn<QString>("invoiceNumber");
    QTest::addColumn<InvoiceTypeData::Type>("invoiceType");
    QTest::addColumn<QList<int> >("netValIndices");
    QTest::addColumn<Money_t::val_t>("discount");
    QTest::addColumn<Money_t>("totalNetVal");
    QTest::addColumn<Money_t>("totalDiscountVal");
    QTest::addColumn<Money_t>("totalGrossVal");

    QList<CommodityData> lcd;
    QString invNum;
    InvoiceTypeData::Type invType;
    QList<int> netValIndices;
    Money_t totalNetVal, totalDiscountVal, totalGrossVal;
    Money_t::val_t discount;

    SettingsGlobal s;

    const int maxInvoices = qrand() % 4 + 2;
    for(int invoice = 0; invoice < maxInvoices; ++invoice)
    {
        const int maxCommodPerInvoice = qrand() % 2 + 2;
        lcd.clear();
        for(int commod = 0; commod < maxCommodPerInvoice; ++commod)
        {
            const CommodityData cd = createNewCommodity(invoice, commod);
            lcd.append(cd);

            const int netValIndex = qrand() % 4;
            netValIndices.append(netValIndex);
            totalNetVal += computeNetVal(cd, netValIndex);
        }

        invType = InvoiceTypeData::VAT;

        invNum = db_->modelInvoice()->generateInvoiceNumber(
                    *(InvoiceNumberFormat_t::Parse(s.value(s.DEFAULT_INV_NUM_FORMAT).toString()).get()),
                                         invNum,
                                         QDate::currentDate(),
                                         QDate::currentDate(),
                                         invType);

        QTest::newRow(qPrintable(QString("%1").arg(invoice))) << lcd << createNewCounterparty(invoice) << invNum << invType << netValIndices << discount << totalNetVal << totalDiscountVal << totalGrossVal;
    }
}


void InvoiceDialogTest::addCommodityInThread(InvoiceDialogPublic *idp, const CommodityData &cd, const int netValIndex)
{
    GuiUserAddCommodity userAddCommod(idp, cd, netValIndex);
    QThread threadCommodity;
    threadCommodity.setObjectName("threadCommodity");
    startUserThread(&userAddCommod, &threadCommodity, idp->ui()->pushButtonAddCommodity);
    while(!threadCommodity.isFinished())
    {
        QTest::qWait(200);
    }
}


void InvoiceDialogTest::addCounterpartyInThread(InvoiceDialogPublic *idp, const Counterparty_t &counterparty)
{
    GuiUserAddCounterparty userAddNewCounterp(idp, counterparty);
    QThread threadCounterparty;
    threadCounterparty.setObjectName("threadCounterparty");
    startUserThread(&userAddNewCounterp, &threadCounterparty, idp->ui()->pushButtonAddCounterparty);
    while(!threadCounterparty.isFinished())
    {
        QTest::qWait(200);
    }
}


void InvoiceDialogTest::addNewCommodityInThread(InvoiceDialogPublic *idp, const CommodityData &cd)
{
    GuiUserAddNewCommodity userAddNewCommod(idp, cd);
    QThread threadNewCommodity;
    threadNewCommodity.setObjectName("threadNewCommodity");
    startUserThread(&userAddNewCommod, &threadNewCommodity, idp->ui()->pushButtonAddNewCommodity);
    while(!threadNewCommodity.isFinished())
    {
        QTest::qWait(200);
    }
}


void InvoiceDialogTest::checkLastInsertedCommodity(const CommodityData &cd)
{
    QSqlQuery query(db_->modelCommodity()->query());
    query.exec(QString("SELECT name, abbreviation, pkwiu, type_id, unit_id, net1, net2, net3, net4, vat, quantity FROM commodity WHERE id_commodity = (SELECT MAX(id_commodity) FROM commodity)"));
    if(!query.isActive())
    {
        const QString errMsg(QString("InvoiceDialogTest::testGUI_AddDeleteCommodities(): first SQL query failed. Reason: %1\nQuery: %2").arg(query.lastError().text()).arg(query.lastQuery()));
        QFAIL(qPrintable(errMsg));
    }
    const int precision = 2;
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), cd.field(CommodityFields::NAME).toString());
    QCOMPARE(query.value(1).toString(), cd.field(CommodityFields::ABBREV).toString());
    QCOMPARE(query.value(2).toString(), cd.field(CommodityFields::PKWIU).toString());
    QCOMPARE(query.value(3).toInt() - 1, cd.field(CommodityFields::TYPE_ID).toInt()); //-1 because SQL starts from 1
    QCOMPARE(query.value(4).toInt() - 1, cd.field(CommodityFields::UNIT_ID).toInt()); //-1 because SQL starts from 1
    QCOMPARE(query.value(5).toString(), cd.field(CommodityFields::NET1).value<Money_t>().toString(precision));
    QCOMPARE(query.value(6).toString(), cd.field(CommodityFields::NET2).value<Money_t>().toString(precision));
    QCOMPARE(query.value(7).toString(), cd.field(CommodityFields::NET3).value<Money_t>().toString(precision));
    QCOMPARE(query.value(8).toString(), cd.field(CommodityFields::NET4).value<Money_t>().toString(precision));
    QCOMPARE(query.value(9).toString(), QString("%1").arg(cd.field(CommodityFields::VAT).value<Money_t::val_t>().get_d()));
    QCOMPARE(query.value(10).toString(), QString("%1").arg(cd.field(CommodityFields::QUANTITY).value<Money_t::val_t>().get_d()));
}


Money_t InvoiceDialogTest::computeNetVal(const CommodityData &cd, const int netValIndex) const
{
    Money_t ret;
    const Money_t::val_t quantity(cd.field(CommodityFields::QUANTITY).value<Money_t::val_t>());

    switch(netValIndex)
    {
    case CommodityFields::NET1:
        ret = cd.field(CommodityFields::NET1).value<Money_t>() * quantity;
        break;
    case CommodityFields::NET2:
        ret = cd.field(CommodityFields::NET2).value<Money_t>() * quantity;
        break;
    case CommodityFields::NET3:
        ret = cd.field(CommodityFields::NET3).value<Money_t>() * quantity;
        break;
    case CommodityFields::NET4:
        ret = cd.field(CommodityFields::NET4).value<Money_t>() * quantity;
        break;
    }

    return ret;
}


void InvoiceDialogTest::startUserThread(GuiUser *guiUser, QThread *thread, QPushButton *buttonStart) const
{
    guiUser->moveToThread(thread);
    connect(thread, SIGNAL(started()), guiUser, SLOT(process()));
    connect(guiUser, SIGNAL(finished()), thread, SLOT(quit()));
    thread->start();
    QTest::mouseClick(buttonStart, Qt::LeftButton);
}


CommodityData InvoiceDialogTest::createNewCommodity(const int invoiceIndex, const int commodityIndex) const
{
    CommodityData cd;
    SettingsGlobal s;
    QVariant v;
    const QStringList vatRates(s.value(s.keyName(s.VAT_RATES)).toString().split("|"));
    static qlonglong commodityIDNum = 1;//SQL starts from 1

    cd.setField(CommodityFields::ABBREV, QString("abbrev_%1").arg(invoiceIndex));
    cd.setField(CommodityFields::ID_COMMODITY, commodityIDNum++);
    cd.setField(CommodityFields::NAME, QString("name_%1_%2").arg(invoiceIndex).arg(commodityIndex));

    v.setValue(Money_t(QString("%1%2%3").arg(commodityIndex).arg(s.decimalPointStr()).arg(11)));
    cd.setField(CommodityFields::NET1, v);

    v.setValue(Money_t(QString("%1%2%3").arg(commodityIndex).arg(s.decimalPointStr()).arg(12)));
    cd.setField(CommodityFields::NET2, v);

    v.setValue(Money_t(QString("%1%2%3").arg(commodityIndex).arg(s.decimalPointStr()).arg(13)));
    cd.setField(CommodityFields::NET3, v);

    v.setValue(Money_t(QString("%1%2%3").arg(commodityIndex).arg(s.decimalPointStr()).arg(14)));
    cd.setField(CommodityFields::NET4, v);

    cd.setField(CommodityFields::PKWIU, QString("pkwiu_%1_%2").arg(invoiceIndex).arg(commodityIndex));

    v.setValue(Money_t::val_t(qrand() % 1000 + 1.234));
    cd.setField(CommodityFields::QUANTITY, v);

    v.setValue(Money_t::val_t(vatRates[qrand() % vatRates.size()].toDouble()));
    cd.setField(CommodityFields::VAT, v);

    const CommodityTypeData::CommodityType type = CommodityTypeData::GOODS;
    cd.setField(CommodityFields::TYPE_ID, type);

    const UnitData::Name unit = UnitData::Name(qrand() % (UnitData::PACKAGE - UnitData::UNIT + 1));
    cd.setField(CommodityFields::UNIT_ID, unit);

    return cd;
}


Counterparty_t InvoiceDialogTest::createNewCounterparty(const int invoiceIndex) const
{
    const int base = 10;
    Counterparty_t counterparty;

    counterparty.accountNumber = QString("%1-%2-%3-%4-%5-%6-%7")
            .arg(qrand() % 100, 2, base, QChar('0'))
            .arg(qrand() % 10000, 4, base, QChar('0'))
            .arg(qrand() % 10000, 4, base, QChar('0'))
            .arg(qrand() % 10000, 4, base, QChar('0'))
            .arg(qrand() % 10000, 4, base, QChar('0'))
            .arg(qrand() % 10000, 4, base, QChar('0'))
            .arg(qrand() % 10000, 4, base, QChar('0'));
    counterparty.city = QString("city_%1").arg(invoiceIndex);

    const QStringList countries(db_->modelCountry()->stringList());
    counterparty.country = QString("%1").arg(countries.at(qrand() % (countries.size())));

    counterparty.email = QString("%1@test.pl").arg(invoiceIndex);
    counterparty.name = QString("name_%1").arg(invoiceIndex);
    counterparty.phone = QString("%1").arg(qrand(), 10, base, QChar('1'));
    counterparty.postalCode = QString("postalCode_%1").arg(invoiceIndex);
    counterparty.street = QString("street_%1").arg(invoiceIndex);
    counterparty.taxID = QString("%1-%2-%3-%4")
            .arg(qrand()%1000, 3, base, QChar('0'))
            .arg(qrand()%1000, 3, base, QChar('0'))
            .arg(qrand()%100, 2, base, QChar('0'))
            .arg(qrand()%100, 2, base, QChar('0'));
    counterparty.type = CounterpartyTypeData::name( (qrand() % 2)? CounterpartyTypeData::COMPANY : CounterpartyTypeData::OFFICE);
    counterparty.www = QString("http://www.row%1.com").arg(invoiceIndex);

    return counterparty;
}

QTEST_MAIN(InvoiceDialogTest)

#include "tst_invoicedialogtest.moc"
