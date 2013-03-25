#ifndef CORRECTIVEINVOICEDIALOG_H
#define CORRECTIVEINVOICEDIALOG_H

#include <QString>
#include <QObject>
#include <QDomDocument>

#include "InvoiceDialog.h"

/**
 * @brief
 *
 */
class CorrectiveInvoiceDialog: public QDialog
{
    Q_OBJECT
public:
/**
 * @brief
 *
 * @param
 * @param db
 */
    CorrectiveInvoiceDialog(QWidget *parent, Database *db);
    /**
     * @brief
     *
     */
    virtual ~CorrectiveInvoiceDialog();

protected slots:
    /**
     * @brief
     *
     */
    //void printInvoice();
    /**
     * @brief
     *
     */
    void canQuit();
protected:
    /**
     * @brief
     *
     * @param bool
     */
    void setIsEditAllowed(bool);
    /**
     * @brief
     *
     */
    void calculateDiscount();
    /**
     * @brief
     *
     */
    void calculateSum();
    /**
     * @brief
     *
     * @param int
     */
    void calculateOneDiscount(const int);

    /**
     * @brief
     *
     */
    //void makeInvoiceSummAll();
    /**
     * @brief
     *
     */
    //void makeInvoiceSumm();
private:
    /**
     * @brief
     *
     */
    void createOriginalInv();
    /**
     * @brief
     *
     */
    //void makeBeforeCorrProducts();

    /**
     * @brief
     *
     */
    void init_();

    /**
     * @brief
     *
     */
    //void makeBeforeCorrSumm();
    /**
     * @brief
     *
     * @param short
     */
    void makeInvoceProductsTitle(const short);
private:
    Ui::InvoiceDialog *ui_;
    Database *db_;
    //bool editMode, isEdit; /**< TODO */
    double origDiscTotal, origNettTotal, origGrossTotal, diffTotal; /**< TODO */
    //InvoiceData invData; /**< TODO */
    QComboBox *reasonCombo; /**< TODO */
    QLabel *labelReason1; /**< TODO */
};
#endif
