#include "gui.h"

#include <iostream>
#include <vector>
#include <QWindow>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QCompleter>
#include <QSpinBox>
#include <QListWidget>

#include "../cruise/Cruise.h"
#include "../ship/Ship.h"
#include "../cruise/PortEntry.h"

#include "../globals.h"


void refill_cruises_table(QTableWidget* cruises_table_widget) {
    cruises = CruiseMapper::get_all();
    cruises_table_widget->setRowCount(cruises.size());
    int row = 0;
    for (auto &cruise: cruises) {
        cruises_table_widget->setItem(row, 0, new QTableWidgetItem(cruise.departure_port->name.c_str()));
        cruises_table_widget->setItem(row, 1, new QTableWidgetItem(cruise.destination_port->name.c_str()));
        cruises_table_widget->setItem(row, 2, new QTableWidgetItem(cruise.ship->name.c_str()));
        cruises_table_widget->setItem(row, 3, new QTableWidgetItem(cruise.general_cargo_package_type->name.c_str()));
        cruises_table_widget->setItem(row, 4, new QTableWidgetItem(cruise.charterer->name.c_str()));
        row++;
    }
    cruises_table_widget->resizeColumnsToContents();
}

void edit_cruise_window(QTableWidget* cruises_table_widget, Cruise* cruise = nullptr) {
    auto *window = new QWidget;
    window->setMinimumSize(500, 200);
    window->setWindowTitle(cruise == nullptr ? "New cruise" : "Edit cruise");

    auto grid_layout = new QGridLayout;
    window->setLayout(grid_layout);

    auto *form_layout = new QFormLayout;

    QStringList qstring_ports;
    for (const auto &port: ports) {
        qstring_ports.push_back(QString::fromStdString(port.name));
    }
    auto departure_port_field = new QComboBox;
    departure_port_field->addItems(qstring_ports);
    departure_port_field->setEditable(true);
    auto destination_port_field = new QComboBox;
    destination_port_field->addItems(qstring_ports);
    destination_port_field->setEditable(true);

    auto ship_field = new QComboBox;
    for (const auto &ship: ships) {
        ship_field->addItem(QString::fromStdString(ship.name));
    }
    ship_field->setEditable(true);

    auto general_cargo_field = new QComboBox;
    for (const auto &type: general_cargo_package_types) {
        general_cargo_field->addItem(QString::fromStdString(type.name));
    }
    general_cargo_field->setEditable(true);

    auto charterer_field = new QComboBox;
    for (const auto &charterer: charterers) {
        charterer_field->addItem(QString::fromStdString(charterer.name));
    }
    charterer_field->setEditable(true);

    auto port_entries_field = new QGridLayout;
    auto port_entries_list_widget = new QListWidget;
    std::vector<PortEntry>* new_port_entries = new std::vector<PortEntry>();
    auto port_entry_add_button = new QPushButton("+");
    auto port_entry_remove_button = new QPushButton("-");
    port_entries_field->addWidget(port_entries_list_widget, 0, 0, 7, 8);
    port_entries_field->addWidget(port_entry_add_button, 7, 0, 1, 1);
    port_entries_field->addWidget(port_entry_remove_button, 7, 1, 1, 1);
    QPushButton::connect(port_entry_add_button, &QPushButton::clicked, [=]() {
        if (port_entries_field->itemAtPosition(8, 0) != nullptr)
            return;
        auto port_entry_widget = new QWidget;
        auto port_entry_form_layout = new QFormLayout;
        port_entry_form_layout->setParent(port_entry_widget);
        port_entry_widget->setLayout(port_entry_form_layout);
        auto port_field = new QComboBox;
        for (const auto &port: ports) {
            port_field->addItem(port.name.c_str(), QVariant(port.id));
        }
        port_field->setEditable(true);
        auto destination_ts_field = new QDateTimeEdit;
        destination_ts_field->setDisplayFormat("dd.MM.yyyy HH:mm");
        destination_ts_field->setCalendarPopup(true);
        auto departure_ts_field = new QDateTimeEdit;
        departure_ts_field->setDisplayFormat("dd.MM.yyyy HH:mm");
        departure_ts_field->setCalendarPopup(true);
        auto ok_button = new QPushButton("OK");
        QPushButton::connect(ok_button, &QPushButton::clicked, [=]() {
            tm destination_ts{};
            destination_ts.tm_year = destination_ts_field->date().year() - 1900;
            destination_ts.tm_mon = destination_ts_field->date().month() - 1;
            destination_ts.tm_mday = destination_ts_field->date().day();
            tm departure_ts{};
            departure_ts.tm_year = departure_ts_field->date().year() - 1900;
            departure_ts.tm_mon = departure_ts_field->date().month() - 1;
            departure_ts.tm_mday = departure_ts_field->date().day();
            auto new_item = new QListWidgetItem(port_field->currentText());
            new_item->setData(Qt::UserRole, port_field->currentData());
            port_entries_list_widget->addItem(new_item);
            auto p = std::find_if(ports.begin(), ports.end(),
                                  [=](const Port& found) {
                                      return found.id == port_field->currentData().value<int>();
                                  }).operator->();
            new_port_entries->emplace_back(
                    p,
                    destination_ts, departure_ts);
            port_entries_field->removeWidget(port_entry_widget);
            delete port_entry_widget;
        });
        port_entry_form_layout->addRow("Port:", port_field);
        port_entry_form_layout->addRow("Destination:", destination_ts_field);
        port_entry_form_layout->addRow("Departure:", departure_ts_field);
        port_entry_form_layout->addRow(ok_button);
        port_entries_field->addWidget(port_entry_widget, 8, 0, 4, 8);
    });

    QPushButton::connect(port_entry_remove_button, &QPushButton::clicked, [=]() {
        port_entries_list_widget->takeItem(port_entries_list_widget->currentRow());
    });

    if (cruise != nullptr) {
        departure_port_field->setCurrentIndex(std::find_if(
                ports.begin(), ports.end(),
                [=](const Port& found) {return found.id == cruise->departure_port->id;}) - ports.begin());
        destination_port_field->setCurrentIndex(std::find_if(
                ports.begin(), ports.end(),
                [=](const Port& found) {return found.id == cruise->destination_port->id;}) - ports.begin());
        ship_field->setCurrentIndex(std::find_if(
                ships.begin(), ships.end(),
                [=](const Ship& found) {return found.id == cruise->ship->id;}) - ships.begin());
        general_cargo_field->setCurrentIndex(std::find_if(
                general_cargo_package_types.begin(), general_cargo_package_types.end(),
                [=](const GeneralCargoPackageType& found) {return found.id == cruise->general_cargo_package_type->id;})
                                             - general_cargo_package_types.begin());
        charterer_field->setCurrentIndex(std::find_if(
                charterers.begin(), charterers.end(),
                [=](const Charterer& found) {return found.id == cruise->charterer->id;}) - charterers.begin());
        for (const auto &entry: cruise->port_entries) {
            auto new_item = new QListWidgetItem(entry->port->name.c_str());
            new_item->setData(Qt::UserRole, QVariant(entry->port->id));
            port_entries_list_widget->addItem(new_item);
        }
    }

    form_layout->addRow("Departure port:", departure_port_field);
    form_layout->addRow("Destination port:", destination_port_field);
    form_layout->addRow("Ship:", ship_field);
    form_layout->addRow("General cargo:", general_cargo_field);
    form_layout->addRow("Charterer:", charterer_field);
    form_layout->addRow("Port entries:", port_entries_field);

    auto apply_button = new QPushButton(cruise == nullptr ? "Create" : "Apply");
    QPushButton::connect(apply_button, &QPushButton::clicked, [=](){
        for (const auto &entry: *new_port_entries) {
            PortEntryMapper::insert(entry);
        }
        CruiseMapper::insert(Cruise(
                cruise == nullptr ? 0 : cruise->id,
                &ships.at(ship_field->currentIndex()),
                &general_cargo_package_types.at(general_cargo_field->currentIndex()),
                &ports.at(departure_port_field->currentIndex()),
                &ports.at(destination_port_field->currentIndex()),
                {},
                &charterers.at(charterer_field->currentIndex())
        ));

        window->close();
        port_entries = PortEntryMapper::get_all();
        refill_cruises_table(cruises_table_widget);
    });

    grid_layout->addLayout(form_layout, 0, 0, 9, 2);
    grid_layout->addWidget(apply_button, 10, 0, 1, 1);

    window->show();
}

void show_cruise_description(QTableWidget* cruises_table_widget, QFormLayout* cruise_desc_layout) {
    if (cruise_desc_layout->rowCount() > 0) {
        for (int row = 0; row < 7; row++) {
            cruise_desc_layout->removeRow(0);
        }
    }

    int selected_row = cruises_table_widget->currentRow();

    auto remove_button = new QPushButton("Remove");
    auto edit_button = new QPushButton("Edit...");
    QPushButton::connect(remove_button, &QPushButton::clicked, [=](){
        cruises_table_widget->clearSelection();
        CruiseMapper::remove(cruises.at(selected_row));
        refill_cruises_table(cruises_table_widget);
        remove_button->setDisabled(true);
        edit_button->setDisabled(true);
    });
    QPushButton::connect(edit_button, &QPushButton::clicked, [=](){
        edit_cruise_window(cruises_table_widget, &cruises.at(selected_row));
        show_cruise_description(cruises_table_widget, cruise_desc_layout);
    });
    auto buttons_layout = new QGridLayout;
    buttons_layout->addWidget(edit_button, 0, 1, 1, 1);
    buttons_layout->addWidget(remove_button, 0, 0, 1, 1);

    cruise_desc_layout->addRow("Departure port:",
                               new QLabel(cruises.at(selected_row).departure_port->name.c_str()));
    cruise_desc_layout->addRow("Destination port:",
                               new QLabel(cruises.at(selected_row).destination_port->name.c_str()));
    cruise_desc_layout->addRow("Ship:",
                               new QLabel(cruises.at(selected_row).ship->name.c_str()));               // TODO: hyperlink
    cruise_desc_layout->addRow("General cargo:",
                               new QLabel(cruises.at(selected_row).general_cargo_package_type->name.c_str()));
    cruise_desc_layout->addRow("Charterer:",
                               new QLabel(cruises.at(selected_row).charterer->name.c_str()));          // TODO: hyperlink
    auto current_cruise_port_entries = cruises.at(selected_row).port_entries;
    std::string port_entries_list;
    for (int i = 0; i < current_cruise_port_entries.size(); i++) {
        port_entries_list += current_cruise_port_entries.at(i)->port->name;
        if (current_cruise_port_entries.at(i)->destination_ts_actual != nullptr
        && current_cruise_port_entries.at(i)->departure_ts_actual == nullptr) {
            port_entries_list += " <----";
        } else if (current_cruise_port_entries.at(i)->departure_ts_actual != nullptr
        && i+1 < current_cruise_port_entries.size() && current_cruise_port_entries.at(i+1)->destination_ts_actual == nullptr) {
            port_entries_list += " ---->";
        }
        port_entries_list += "\n";
    }
    cruise_desc_layout->addRow("Port entries:",
                               new QLabel(port_entries_list.c_str()));
    cruise_desc_layout->addRow(buttons_layout);
}

QWidget* cruises_widget() {
    auto *cruises_widget = new QWidget;
    auto *cruises_tab_layout = new QGridLayout;
    cruises_widget->setLayout(cruises_tab_layout);

    auto *cruises_table_widget = new QTableWidget(0, 5);
    cruises_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cruises_table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    cruises_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    cruises_table_widget->setHorizontalHeaderLabels(
            QStringList({"Departure port", "Destination port", "Ship", "General cargo", "Charterer"}));

    port_entries = PortEntryMapper::get_all();
    general_cargo_package_types = GeneralCargoPackageTypeMapper::get_all();
    refill_cruises_table(cruises_table_widget);

    auto *new_button = new QPushButton("New cruise...");
    QPushButton::connect(new_button, &QPushButton::clicked, [=](){
        edit_cruise_window(cruises_table_widget);
    });

    auto *cruise_desc_layout = new QFormLayout;

    QTableWidget::connect(cruises_table_widget, &QTableWidget::cellClicked, [=](){
        show_cruise_description(cruises_table_widget, cruise_desc_layout);
    });

    cruises_tab_layout->addWidget(cruises_table_widget, 0, 0, 10, 5);
    cruises_tab_layout->addWidget(new_button, 11, 0, 1, 5);
    cruises_tab_layout->addLayout(cruise_desc_layout, 0, 6, 6, 5);

    return cruises_widget;
}
