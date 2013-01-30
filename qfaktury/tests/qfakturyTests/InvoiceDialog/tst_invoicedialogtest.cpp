#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>

#include "InvoiceDialog.h"
#include "Database.h"

class InvoiceDialogTest : public QObject
{
    Q_OBJECT
    
public:
    InvoiceDialogTest();
    
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testGUI_InitialState();
    void test_generateInvoiceNumber();
    void test_compatibilityWithOldGenerateInvoiceNumber();

private:
    QString generateInvoiceNumberOldVer(InvoiceTypeData::Type invType);
    QString numbersCount(int in, int x);
};

InvoiceDialogTest::InvoiceDialogTest()
{
}

void InvoiceDialogTest::initTestCase()
{
    QCoreApplication::setApplicationName("QFaktury");
    QCoreApplication::setOrganizationName("www.e-linux.pl");
    QCoreApplication::setOrganizationDomain("www.e-linux.pl");
    QCoreApplication::setApplicationVersion(APP_VERSION);

    const QString dbFilename(QString("%1-%2.db3").arg(QCoreApplication::applicationName()).arg(APP_VERSION));
    if(QFile::exists(dbFilename))
    {
        QDir dir;
        dir.remove(dbFilename);
    }
}

void InvoiceDialogTest::cleanupTestCase()
{
}

void InvoiceDialogTest::testGUI_InitialState()
{
    Database db;
    InvoiceDialog invoiceDialog(0, &db);
    QCOMPARE(invoiceDialog.windowTitle(), InvoiceTypeData::InvoiceTypeToString(InvoiceTypeData::VAT));
    QCOMPARE(invoiceDialog.comboBoxInvoiceType->currentText(), InvoiceTypeData::InvoiceTypeToString(InvoiceTypeData::VAT));
    QCOMPARE(invoiceDialog.dateEditDateOfIssuance->date(), QDate::currentDate());
    QCOMPARE(invoiceDialog.dateEditDateOfSell->date(), QDate::currentDate());
    QCOMPARE(invoiceDialog.dateEditDayOfPayment->date(), QDate::currentDate());
    QCOMPARE(invoiceDialog.tableWidgetCommodities->rowCount(), 0);
    QCOMPARE(invoiceDialog.checkBoxDiscount->isChecked(), false);
    QCOMPARE(invoiceDialog.spinBoxDiscount->value(), 0);

    QLocale locale;
    QCOMPARE(invoiceDialog.labelSumNetVal->text(), locale.toString(0.0, 'f', 2));
    QCOMPARE(invoiceDialog.labelDiscountVal->text(), locale.toString(0.0, 'f', 2));
    QCOMPARE(invoiceDialog.labelSumGrossVal->text(), locale.toString(0.0, 'f', 2));
}


void InvoiceDialogTest::test_generateInvoiceNumber()
{
    //InvoiceDialog::InvoiceNumChosenPeriod week = InvoiceDialog::WEEK;
    QFAIL("implement me");
}

void InvoiceDialogTest::test_compatibilityWithOldGenerateInvoiceNumber()
{
    QFAIL("implement me");
}


QString InvoiceDialogTest::generateInvoiceNumberOldVer(InvoiceTypeData::Type invType)
{//old code - for checking compatibility with previous versions
    QString tmp, prefix, suffix;

    if(invType == InvoiceTypeData::PRO_FORMA)
    {
        tmp = sett().value("fpro").toString();
    }
    else
    {
        tmp = sett().value("fvat").toString();
    }

    prefix = sett().value("prefix").toString();

    QStringList one1 = tmp.split("/");
    one1[0] = one1[0].remove(prefix);

    int nr = one1[0].toInt() + 1;
    QString lastInvoice = prefix + numbersCount(nr, 0);

    if (sett().value("day") .toBool())
        lastInvoice += "/" + QDate::currentDate().toString("dd");

    if (false) //sett().value("month") .toBool()
        lastInvoice += "/" + QDate::currentDate().toString("MM");

    if (sett().value("year") .toBool()) {
        if (!sett().value("shortYear") .toBool())
            lastInvoice += "/" + QDate::currentDate().toString("yy");
        else
            lastInvoice += "/" + QDate::currentDate().toString("yyyy");
    }
    suffix = sett().value("sufix").toString();
    lastInvoice += suffix;

    return lastInvoice;
}


QString InvoiceDialogTest::numbersCount(int in, int x) {//old code - for checking compatibility with previous versions
    QString tmp2, tmp = sett().numberToString(in);
    tmp2 = "";
    int incr = x - tmp.length();
    for (int i = 0; i < incr; ++i)
        tmp2 += "0";
    return tmp2 + tmp;
}

QTEST_MAIN(InvoiceDialogTest)

#include "tst_invoicedialogtest.moc"
