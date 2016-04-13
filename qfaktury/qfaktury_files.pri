HEADERS += src/ui/BillDialog.h \
    src/ui/ChangeAmountDialog.h \
    src/ui/ComboBoxCurrentTextRW.h \
    src/ui/CommodityDialog.h \
    src/ui/CommodityVisualData.h \
    src/ui/CommodityListDialog.h \
    src/ui/CommodityListGrossDialog.h \
    src/ui/CorrectiveInvoiceDialog.h \
    src/ui/CorrectiveInvoiceGrossDialog.h \
    src/ui/CounterpartyDialog.h \
    src/ui/CounterpartyInfoDialog.h \
    src/ui/CounterpartyTypeDelegate.h \
    src/ui/CounterpartyTypeDialog.h \
    src/ui/CustomPaymentDialog.h \
    src/ui/DuplicateDialog.h \
    src/ui/EmailDialog.h \
    src/ui/InvoiceDialog.h \
    src/ui/InvoiceNumberFormatEditDialog.h \
    src/ui/InvoiceNumberFormatExamplesDialog.h \
    src/ui/InvoiceGrossDialog.h \
    src/ui/MainWindow.h \
    src/ui/PhoneDialog.h \
    src/ui/SettingsDialog.h \
    src/CustomPaymData.h \
    src/model/Database.h \
    src/model/models_data/UnitData.h \
    src/model/models_data/PhoneData.h \
    src/model/models_data/PaymentTypeData.h \
    src/model/models_data/InvoiceTypeData.h \
    src/model/models_data/InvoiceData.h \
    src/model/models_data/EmailData.h \
    src/model/models_data/CurrencyData.h \
    src/model/models_data/CountryData.h \
    src/model/models_data/CounterpartyTypeData.h \
    src/model/models_data/CounterpartyData.h \
    src/model/models_data/CommodityTypeData.h \
    src/model/models_data/CommodityData.h \
    src/model/models/ModelVat.h \
    src/model/models/ModelUnit.h \
    src/model/models/ModelPhone.h \
    src/model/models/ModelPaymentType.h \
    src/model/models/ModelInvoiceWithCommodities.h \
    src/model/models/ModelInvoiceType.h \
    src/model/models/ModelInvoice.h \
    src/model/models/ModelEmail.h \
    src/model/models/ModelCurrency.h \
    src/model/models/ModelCountry.h \
    src/model/models/ModelCounterpartyType.h \
    src/model/models/ModelCounterparty.h \
    src/model/models/ModelCommodityType.h \
    src/model/models/ModelCommodity.h \
    src/SettingsGlobal.h \
    src/Money_t.h \
    src/ui/InvoiceDialogImpl.h \
    src/model/models_data/InvoiceNumberFormat_t.h \
    src/model/models_data/InvoiceWithCommodities.h \
    src/CommodityDelegate.h \
    src/DecVal.h \
    src/InvoiceComposer.h \
    $$PWD/src/mustache.h



SOURCES += src/main.cpp \
    src/ui/BillDialog.cpp \
    src/ui/ChangeAmountDialog.cpp \
    src/ui/ComboBoxCurrentTextRW.cpp \
    src/ui/CommodityDialog.cpp \
    src/ui/CommodityListDialog.cpp \
    src/ui/CommodityListGrossDialog.cpp \
    src/ui/CorrectiveInvoiceDialog.cpp \
    src/ui/CorrectiveInvoiceGrossDialog.cpp \
    src/ui/CounterpartyDialog.cpp \
    src/ui/CounterpartyInfoDialog.cpp \
    src/ui/CounterpartyTypeDelegate.cpp \
    src/ui/CounterpartyTypeDialog.cpp \
    src/ui/CustomPaymentDialog.cpp \
    src/ui/DuplicateDialog.cpp \
    src/ui/EmailDialog.cpp \
    src/ui/InvoiceDialog.cpp \
    src/ui/InvoiceNumberFormatEditDialog.cpp \
    src/ui/InvoiceNumberFormatExamplesDialog.cpp \
    src/ui/InvoiceGrossDialog.cpp \
    src/ui/MainWindow.cpp \
    src/ui/PhoneDialog.cpp \
    src/ui/SettingsDialog.cpp \
    src/Money_t.cpp \
    src/CustomPaymData.cpp \
    src/model/Database.cpp \
    src/model/models/ModelCommodity.cpp \
    src/model/models/ModelCommodityType.cpp \
    src/model/models/ModelCountry.cpp \
    src/model/models/ModelEmail.cpp \
    src/model/models/ModelPhone.cpp \
    src/model/models/ModelUnit.cpp \
    src/model/models/ModelVat.cpp \
    src/model/models/ModelInvoice.cpp \
    src/model/models/ModelCounterparty.cpp \
    src/model/models/ModelCounterpartyType.cpp \
    src/model/models/ModelCurrency.cpp \
    src/model/models/ModelPaymentType.cpp \
    src/model/models/ModelInvoiceType.cpp \
    src/model/models/ModelInvoiceWithCommodities.cpp \
    src/ui/InvoiceDialogImpl.cpp \
    src/model/models_data/InvoiceNumberFormat_t.cpp \
    src/SettingsGlobal.cpp \
    src/CommodityDelegate.cpp \
    src/DecVal.cpp \
    src/InvoiceComposer.cpp \
    src/model/models_data/CounterpartyData.cpp \
    $$PWD/src/mustache.cpp
