#include "gui.h"

#include <QWindow>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QCompleter>
#include <QSpinBox>
#include <QListWidget>

#include "../ship/Ship.h"
#include "../crew_member/CrewMember.h"

#include "../globals.h"


void show_ship_description(QTableWidget* ships_table_widget, QFormLayout* ship_desc_layout);

void refill_ships_table(QTableWidget* ships_table_widget) {
    ships = ShipMapper::get_all();
    ships_table_widget->setRowCount(ships.size());
    int row = 0;
    for (auto &ship: ships) {
        auto current_cruise = ship.get_current_cruise();
        char* current_cruise_text = new char[128];
        if (current_cruise != nullptr) {
            sprintf(current_cruise_text, "%s -> %s",
                    current_cruise->departure_port->name.c_str(), current_cruise->destination_port->name.c_str());
        } else {
            sprintf(current_cruise_text, "-");
        }
        char* carry_capacity = new char[6];
        sprintf(carry_capacity, "%.2f", ship.carry_capacity);
        char* location = new char[32];
        sprintf(location, "%.5f, %.5f", ship.location[0], ship.location[1]);
        char* overhaul_start_date = new char[11];
        strftime(overhaul_start_date, 11, "%d.%m.%Y", &ship.overhaul_start_date);

        ships_table_widget->setItem(row, 0, new QTableWidgetItem(ship.name.c_str()));
        ships_table_widget->setItem(row, 1, new QTableWidgetItem(current_cruise_text));
        ships_table_widget->setItem(row, 2, new QTableWidgetItem(carry_capacity));
        ships_table_widget->setItem(row, 3, new QTableWidgetItem(ship.homeport->name.c_str()));
        ships_table_widget->setItem(row, 4, new QTableWidgetItem(ship.purpose->name.c_str()));
        ships_table_widget->setItem(row, 5,  new QTableWidgetItem(location));
        ships_table_widget->setItem(row, 6, new QTableWidgetItem(overhaul_start_date));
        row++;
    }
    ships_table_widget->resizeColumnsToContents();
}

void edit_ship_window(QTableWidget* ships_table_widget, QFormLayout* ship_desc_layout, Ship* ship = nullptr) {
    auto *window = new QWidget;
    window->setMinimumSize(500, 200);
    window->setWindowTitle(ship == nullptr ? "New ship" : "Edit ship");

    auto grid_layout = new QGridLayout;
    window->setLayout(grid_layout);

    auto *form_layout = new QFormLayout;

    QStringList qstring_purposes;
    for (const auto &purpose: ship_purposes) {
        qstring_purposes.push_back(QString::fromStdString(purpose.name));
    }
    QStringList qstring_ports;
    for (const auto &port: ports) {
        qstring_ports.push_back(QString::fromStdString(port.name));
    }

    auto name_field = new QLineEdit;
    auto carry_capacity_field = new QDoubleSpinBox;
    carry_capacity_field->setMaximum(100);
    auto homeport_field = new QComboBox;
    homeport_field->addItems(qstring_ports);
    homeport_field->setEditable(true);
    auto purpose_field = new QComboBox;
    purpose_field->addItems(qstring_purposes);
    purpose_field->setEditable(true);

    auto crew_field = new QGridLayout;
    auto crew_list_widget = new QListWidget;
    auto crew_add_button = new QPushButton("+");
    auto crew_remove_button = new QPushButton("-");
    crew_field->addWidget(crew_list_widget, 0, 0, 7, 8);
    crew_field->addWidget(crew_add_button, 7, 0, 1, 1);
    crew_field->addWidget(crew_remove_button, 7, 1, 1, 1);
    QPushButton::connect(crew_add_button, &QPushButton::clicked, [=]() {
        if (crew_field->itemAtPosition(7, 2) != nullptr)
            return;
        auto member_selection_combobox = new QComboBox;
        member_selection_combobox->setEditable(true);
        auto crew_list_items = crew_list_widget->findItems("*", Qt::MatchWildcard);
        char* member_summary = new char[256];
        for (const auto &member: members) {
            auto list_item = std::find_if(
                    crew_list_items.begin(), crew_list_items.end(),
                    [=](QListWidgetItem* found) {
                        return found->data(Qt::UserRole).value<int>() == member.id;
            });
            if (list_item == crew_list_items.end()) {
                sprintf(member_summary, "%.1s. %.1s. %s (%s)",
                        member.first_name.c_str(),
                        member.patronymic.c_str(),
                        member.last_name.c_str(),
                        member.role->title.c_str());
                member_selection_combobox->addItem(member_summary, QVariant(member.id));
            }
        }
        if (member_selection_combobox->count() == 0)
            return;
        QComboBox::connect(member_selection_combobox, &QComboBox::activated, [=]() {
            int member_id = member_selection_combobox->currentData().value<int>();
            auto new_item = new QListWidgetItem(member_selection_combobox->currentText());
            new_item->setData(Qt::UserRole, QVariant(member_id));
            crew_list_widget->addItem(new_item);
            crew_field->removeWidget(member_selection_combobox);
            delete member_selection_combobox;
        });
        crew_field->addWidget(member_selection_combobox, 7, 2, 1, 6);
    });

    QPushButton::connect(crew_remove_button, &QPushButton::clicked, [=]() {
        crew_list_widget->takeItem(crew_list_widget->currentRow());
    });

    auto overhaul_start_date_field = new QDateTimeEdit;
    overhaul_start_date_field->setDisplayFormat("dd.MM.yyyy");
    overhaul_start_date_field->setCalendarPopup(true);

    if (ship != nullptr) {
        name_field->setText(ship->name.c_str());
        carry_capacity_field->setValue(ship->carry_capacity);
        homeport_field->setCurrentIndex(std::find_if(
                ports.begin(), ports.end(),
                [=](const Port& found) {return found.id == ship->homeport->id;})
                    - ports.begin());
        purpose_field->setCurrentIndex(
                std::find_if(ship_purposes.begin(), ship_purposes.end(),
                             [=](const ShipPurpose& found) {return found.id == ship->purpose->id;})
                - ship_purposes.begin());
        char* member_summary = new char[256];
        for (const auto &member: ship->crew) {
            sprintf(member_summary, "%.1s. %.1s. %s (%s)",                                           // TODO: функцию для этого запилить ради всяго святого
                    member->first_name.c_str(),
                    member->patronymic.c_str(),
                    member->last_name.c_str(),
                    member->role->title.c_str());
            auto new_item = new QListWidgetItem(member_summary);
            new_item->setData(Qt::UserRole, QVariant(member->id));
            crew_list_widget->addItem(new_item);
        }
        auto overhaul_start_date = ship->overhaul_start_date;
        overhaul_start_date_field->setDate(QDate(
                1900+overhaul_start_date.tm_year,
                overhaul_start_date.tm_mon+1,
                overhaul_start_date.tm_mday));
    }

    form_layout->addRow("Name:", name_field);
    form_layout->addRow("Carry capacity:", carry_capacity_field);
    form_layout->addRow("Homeport:", homeport_field);
    form_layout->addRow("Purpose:", purpose_field);
    form_layout->addRow("Crew:", crew_field);
    form_layout->addRow("Overhaul start date:", overhaul_start_date_field);

    auto apply_button = new QPushButton(ship == nullptr ? "Create" : "Apply");
    QPushButton::connect(apply_button, &QPushButton::clicked, [=](){
        tm overhaul_start_date{};
        overhaul_start_date.tm_year = overhaul_start_date_field->date().year() - 1900;
        overhaul_start_date.tm_mon = overhaul_start_date_field->date().month() - 1;
        overhaul_start_date.tm_mday = overhaul_start_date_field->date().day();
        std::vector<CrewMember*> crew;
        for (const auto &list_item: crew_list_widget->findItems("*", Qt::MatchWildcard)) {
            crew.push_back(std::find_if(
                    members.begin(), members.end(),
                    [=](CrewMember found) {return found.id == list_item->data(Qt::UserRole).value<int>();}).operator->());
        }
        float location[2];
        location[0] = ship == nullptr ? 0 : ship->location[0];
        location[1] = ship == nullptr ? 0 : ship->location[1];
        ShipMapper::insert(Ship(
                ship == nullptr ? 0 : ship->id,
                name_field->text().toStdString(),
                carry_capacity_field->value(),
                &ports.at(homeport_field->currentIndex()),
                &ship_purposes.at(purpose_field->currentIndex()),
                crew,
                location,
                overhaul_start_date
        ));
        members = CrewMemberMapper::get_all();

        window->close();
        refill_ships_table(ships_table_widget);
        ships_table_widget->selectRow(ships_table_widget->rowCount()-1);
        show_ship_description(ships_table_widget, ship_desc_layout);                                                                            // TODO: сделать также везде
    });

    grid_layout->addLayout(form_layout, 0, 0, 9, 2);
    grid_layout->addWidget(apply_button, 10, 0, 1, 1);

    window->show();
}

void show_ship_description(QTableWidget* ships_table_widget, QFormLayout* ship_desc_layout) {
    if (ship_desc_layout->rowCount() > 0) {
        for (int row = 0; row < 9; row++) {
            ship_desc_layout->removeRow(0);
        }
    }

    int selected_row = ships_table_widget->currentRow();

    auto remove_button = new QPushButton("Remove");
    auto edit_button = new QPushButton("Edit...");
    QPushButton::connect(remove_button, &QPushButton::clicked, [=](){
        ships_table_widget->clearSelection();
        ShipMapper::remove(ships.at(selected_row));
        refill_ships_table(ships_table_widget);
        remove_button->setDisabled(true);
        edit_button->setDisabled(true);
    });
    QPushButton::connect(edit_button, &QPushButton::clicked, [=](){
        edit_ship_window(ships_table_widget, ship_desc_layout, &ships.at(selected_row));
        show_ship_description(ships_table_widget, ship_desc_layout);
    });
    auto buttons_layout = new QGridLayout;
    buttons_layout->addWidget(edit_button, 0, 1, 1, 1);
    buttons_layout->addWidget(remove_button, 0, 0, 1, 1);

    char* carry_capacity = new char[6];
    sprintf(carry_capacity, "%.2f", ships.at(selected_row).carry_capacity);

    std::string crew_list;
    char* member_summary = new char[256];
    if (!ships.at(selected_row).crew.empty()) {
        for (const auto &member: ships.at(selected_row).crew) {
            sprintf(member_summary, "%.1s. %.1s. %s (%s)\n",
                    member->first_name.c_str(),
                    member->patronymic.c_str(),
                    member->last_name.c_str(),
                    member->role->title.c_str());
            crew_list += member_summary;
        }
    } else {
        crew_list = "-";
    }

    char* location = new char[32];
    sprintf(location, "%.5f, %.5f",
            ships.at(selected_row).location[0], ships.at(selected_row).location[1]);

    char* overhaul_start_date = new char[11];
    strftime(overhaul_start_date, 11, "%d.%m.%Y",
             &ships.at(selected_row).overhaul_start_date);

    ship_desc_layout->addRow("Name:",
                             new QLabel(ships.at(selected_row).name.c_str()));
    auto current_cruise = ships.at(selected_row).get_current_cruise();
    char* current_cruise_label_text = new char[128];
    auto current_cruise_label = new QLabel;
    if (current_cruise != nullptr) {
        sprintf(current_cruise_label_text, "<a href="">%s -> %s</a>",
                current_cruise->departure_port->name.c_str(), current_cruise->destination_port->name.c_str());
        current_cruise_label->setText(current_cruise_label_text);
    } else {
        current_cruise_label->setText("-");
    }
    ship_desc_layout->addRow("Current cruise:", new QLabel("-"));
    ship_desc_layout->addRow("Carry capacity:", new QLabel(carry_capacity));
    ship_desc_layout->addRow("Homeport:", new QLabel(ships.at(selected_row).homeport->name.c_str()));
    ship_desc_layout->addRow("Purpose:", new QLabel(ships.at(selected_row).purpose->name.c_str()));
    ship_desc_layout->addRow("Crew:", new QLabel(crew_list.c_str()));
    ship_desc_layout->addRow("Location:", new QLabel(location));
    ship_desc_layout->addRow("Overhaul start date:", new QLabel(overhaul_start_date));
    ship_desc_layout->addRow(buttons_layout);
}

QWidget* ships_widget() {
    auto *ships_widget = new QWidget;
    auto *ships_tab_layout = new QGridLayout;
    ships_widget->setLayout(ships_tab_layout);

    auto *ships_table_widget = new QTableWidget(0, 6);
    ships_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ships_table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    ships_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ships_table_widget->setHorizontalHeaderLabels(
            QStringList({"Name", "Current cruise", "Carry capacity", "Homeport", "Purpose",
                         "Location", "Overhaul start date"}));

    ports = PortMapper::get_all();
    ship_purposes = ShipPurposeMapper::get_all();
    refill_ships_table(ships_table_widget);

    auto *new_button = new QPushButton("New ship...");

    auto *ship_desc_layout = new QFormLayout;

    QPushButton::connect(new_button, &QPushButton::clicked, [=](){
        edit_ship_window(ships_table_widget, ship_desc_layout);
    });

    QTableWidget::connect(ships_table_widget, &QTableWidget::cellClicked, [=](){
        show_ship_description(ships_table_widget, ship_desc_layout);
    });

    ships_tab_layout->addWidget(ships_table_widget, 0, 0, 10, 5);
    ships_tab_layout->addWidget(new_button, 11, 0, 1, 5);
    ships_tab_layout->addLayout(ship_desc_layout, 0, 6, 6, 5);

    return ships_widget;
}
