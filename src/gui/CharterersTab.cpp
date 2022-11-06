#include "gui.h"
#include <QWindow>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>

#include "../charterer/Charterer.h"

#include "../globals.h"


void refill_charterers_table(QTableWidget* charterers_table_widget) {
    charterers = ChartererMapper::get_all();
    charterers_table_widget->setRowCount(charterers.size());
    int row = 0;
    for (const auto &charterer: charterers) {
        charterers_table_widget->setItem(row, 0,
                                         new QTableWidgetItem(charterer.name.c_str()));
        charterers_table_widget->setItem(row, 1,
                                         new QTableWidgetItem(charterer.address.c_str()));
        charterers_table_widget->setItem(row, 2,
                                         new QTableWidgetItem(charterer.phone_number.c_str()));
        charterers_table_widget->setItem(row, 3,
                                         new QTableWidgetItem(charterer.fax.c_str()));
        charterers_table_widget->setItem(row, 4,
                                         new QTableWidgetItem(charterer.email.c_str()));
        charterers_table_widget->setItem(row, 5,
                                         new QTableWidgetItem(charterer.bankdetails_bank_name.c_str()));
        charterers_table_widget->setItem(row, 6,
                                         new QTableWidgetItem(charterer.bankdetails_city.c_str()));
        charterers_table_widget->setItem(row, 7,
                                         new QTableWidgetItem(charterer.bankdetails_TIN.c_str()));
        charterers_table_widget->setItem(row, 8,
                                         new QTableWidgetItem(charterer.bankdetails_account_number.c_str()));
        row++;
    }
    charterers_table_widget->resizeColumnsToContents();
}

void edit_charterer_window(QTableWidget* charterers_table_widget, Charterer* charterer = nullptr) {
    auto *window = new QWidget;
    window->setMinimumSize(500, 200);
    window->setWindowTitle(charterer == nullptr ? "New charterer" : "Edit charterer");

    auto grid_layout = new QGridLayout;
    window->setLayout(grid_layout);

    auto *form_layout = new QFormLayout;

    auto name_field = new QLineEdit;
    auto address_field = new QLineEdit;
    auto phone_number_field = new QLineEdit;
    auto fax_field = new QLineEdit;
    auto email_field = new QLineEdit;
    auto bank_name = new QLineEdit;
    auto bank_city = new QLineEdit;
    auto bank_tin = new QLineEdit;
    auto bank_account_number = new QLineEdit;

    if (charterer != nullptr) {
        name_field->setText(charterer->name.c_str());
        address_field->setText(charterer->address.c_str());
        phone_number_field->setText(charterer->phone_number.c_str());
        fax_field->setText(charterer->fax.c_str());
        email_field->setText(charterer->email.c_str());
        bank_name->setText(charterer->bankdetails_bank_name.c_str());
        bank_city->setText(charterer->bankdetails_city.c_str());
        bank_tin->setText(charterer->bankdetails_TIN.c_str());
        bank_account_number->setText(charterer->bankdetails_account_number.c_str());
    }

    form_layout->addRow("Name", name_field);
    form_layout->addRow("Address", address_field);
    form_layout->addRow("Phone number", phone_number_field);
    form_layout->addRow("Fax", fax_field);
    form_layout->addRow("Email", email_field);
    form_layout->addRow("Bank name", bank_name);
    form_layout->addRow("City", bank_city);
    form_layout->addRow("TIN", bank_tin);
    form_layout->addRow("Account number", bank_account_number);

    auto apply_button = new QPushButton(charterer == nullptr ? "Create" : "Apply");
    QPushButton::connect(apply_button, &QPushButton::clicked, [=](){
        ChartererMapper::insert(Charterer(
                charterer == nullptr ? 0 : charterer->id,
                name_field->text().toStdString(),
                address_field->text().toStdString(),
                phone_number_field->text().toStdString(),
                fax_field->text().toStdString(),
                email_field->text().toStdString(),
                bank_name->text().toStdString(),
                bank_city->text().toStdString(),
                bank_tin->text().toStdString(),
                bank_account_number->text().toStdString()
        ));
        window->close();
        refill_charterers_table(charterers_table_widget);
    });

    grid_layout->addLayout(form_layout, 0, 0, 9, 2);
    grid_layout->addWidget(apply_button, 10, 0, 1, 1);

    window->show();
}

void show_charterer_description(QTableWidget* charterers_table_widget, QFormLayout* charterer_desc_layout) {
    if (charterer_desc_layout->rowCount() > 0) {
        for (int row = 0; row < 10; row++) {
            charterer_desc_layout->removeRow(0);
        }
    }

    int selected_row = charterers_table_widget->currentRow();

    auto address_label = new QLabel(charterers.at(selected_row).address.c_str());
    address_label->setWordWrap(true);

    auto remove_button = new QPushButton("Remove");
    auto edit_button = new QPushButton("Edit...");
    QPushButton::connect(remove_button, &QPushButton::clicked, [=](){
        charterers_table_widget->clearSelection();
        ChartererMapper::remove(charterers.at(selected_row));
        refill_charterers_table(charterers_table_widget);
        remove_button->setDisabled(true);
        edit_button->setDisabled(true);
    });
    QPushButton::connect(edit_button, &QPushButton::clicked, [=](){
        edit_charterer_window(charterers_table_widget, &charterers.at(selected_row));
        show_charterer_description(charterers_table_widget, charterer_desc_layout);
    });
    auto buttons_layout = new QGridLayout;
    buttons_layout->addWidget(edit_button, 0, 1, 1, 1);
    buttons_layout->addWidget(remove_button, 0, 0, 1, 1);

    charterer_desc_layout->addRow("Name:",
                                  new QLabel(charterers.at(selected_row).name.c_str()));
    charterer_desc_layout->addRow("Address:", address_label);
    charterer_desc_layout->addRow("Phone number:",
                                  new QLabel(charterers.at(selected_row).phone_number.c_str()));
    charterer_desc_layout->addRow("Fax:",
                                  new QLabel(charterers.at(selected_row).fax.c_str()));
    charterer_desc_layout->addRow("Email:",
                                  new QLabel(charterers.at(selected_row).email.c_str()));
    charterer_desc_layout->addRow("Bank name:",
                                  new QLabel(charterers.at(selected_row).bankdetails_bank_name.c_str()));
    charterer_desc_layout->addRow("City:",
                                  new QLabel(charterers.at(selected_row).bankdetails_city.c_str()));
    charterer_desc_layout->addRow("TIN:",
                                  new QLabel(charterers.at(selected_row).bankdetails_TIN.c_str()));
    charterer_desc_layout->addRow("Account number:",
                                  new QLabel(charterers.at(selected_row).bankdetails_account_number.c_str()));
    charterer_desc_layout->addRow(buttons_layout);
}

QWidget* charterers_widget() {
    auto *charterers_widget = new QWidget;
    auto *charterers_tab_layout = new QGridLayout;
    charterers_widget->setLayout(charterers_tab_layout);

    auto *charterers_table_widget = new QTableWidget(0, 9);
    charterers_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    charterers_table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    charterers_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    charterers_table_widget->setHorizontalHeaderLabels(
            QStringList({"Name", "Address", "Phone number", "Fax", "Email",
                         "Bank name", "City", "TIN", "Account number"}));
    refill_charterers_table(charterers_table_widget);

    auto *new_button = new QPushButton("New charterer...");
    QPushButton::connect(new_button, &QPushButton::clicked, [=](){
        edit_charterer_window(charterers_table_widget);
    });

    auto *charterer_desc_layout = new QFormLayout;

    QTableWidget::connect(charterers_table_widget, &QTableWidget::cellClicked, [=](){
        show_charterer_description(charterers_table_widget, charterer_desc_layout);
    });

    charterers_tab_layout->addWidget(charterers_table_widget, 0, 0, 10, 5);
    charterers_tab_layout->addWidget(new_button, 11, 0, 1, 5);
    charterers_tab_layout->addLayout(charterer_desc_layout, 0, 6, 6, 5);

    return charterers_widget;
}