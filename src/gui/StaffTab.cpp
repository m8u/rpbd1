#include "gui.h"
#include <QWindow>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QCompleter>
#include <QSpinBox>

#include "../crew_member/CrewMember.h"

#include "../globals.h"


void refill_staff_table(QTableWidget* staff_table_widget) {
    roles = RoleMapper::get_all();
    members = CrewMemberMapper::get_all();
    ships = ShipMapper::get_all();
    staff_table_widget->setRowCount(members.size());
    int row = 0;
    for (auto &member: members) {
        char* birth_date = new char[11];
        strftime(birth_date, 11, "%d.%m.%Y", &member.birth_date);
        char* experience = new char[2];
        sprintf(experience, "%d", member.experience);
        char* salary = new char[16];
        sprintf(salary, "%d", member.salary);

        staff_table_widget->setItem(row, 0,
                                    new QTableWidgetItem(member.first_name.c_str()));
        staff_table_widget->setItem(row, 1,
                                    new QTableWidgetItem(member.last_name.c_str()));
        staff_table_widget->setItem(row, 2,
                                    new QTableWidgetItem(member.patronymic.c_str()));
        staff_table_widget->setItem(row, 3,
                                    new QTableWidgetItem(birth_date));
        auto ship = member.get_ship();
        staff_table_widget->setItem(row, 4,
                                    new QTableWidgetItem(ship != nullptr ? ship->name.c_str() : "-"));
        staff_table_widget->setItem(row, 5,
                                    new QTableWidgetItem(member.role->title.c_str()));
        staff_table_widget->setItem(row, 6,
                                    new QTableWidgetItem(experience));
        staff_table_widget->setItem(row, 7,
                                    new QTableWidgetItem(salary));
        row++;
    }
    staff_table_widget->resizeColumnsToContents();
}

void edit_member_window(QTableWidget* staff_table_widget, CrewMember* member = nullptr) {
    auto *window = new QWidget;
    window->setMinimumSize(500, 200);
    window->setWindowTitle(member == nullptr ? "New member" : "Edit member");

    auto grid_layout = new QGridLayout;
    window->setLayout(grid_layout);

    auto *form_layout = new QFormLayout;

//    roles = RoleMapper::get_all();  ---- ILLEGAL! MUST ALSO UPDATE ALL REFERENCES TO ROLES
    QStringList qstring_roles;
    for (const auto &role: roles) {
        qstring_roles.push_back(QString::fromStdString(role.title));
    }

    auto first_name_field = new QLineEdit;
    auto last_name_field = new QLineEdit;
    auto patronymic_field = new QLineEdit;
    auto birth_date_field = new QDateTimeEdit;
    birth_date_field->setDisplayFormat("dd.MM.yyyy");
    birth_date_field->setCalendarPopup(true);
    auto role_field = new QComboBox;
    role_field->addItems(qstring_roles);
    role_field->setEditable(true);
    auto experience_field = new QSpinBox;
    experience_field->setMaximum(100);
    auto salary_field = new QSpinBox;
    salary_field->setMaximum(INT32_MAX);

    if (member != nullptr) {
        first_name_field->setText(member->first_name.c_str());
        last_name_field->setText(member->last_name.c_str());
        patronymic_field->setText(member->patronymic.c_str());
        auto birth_date = member->birth_date;
        birth_date_field->setDate(QDate(1900+birth_date.tm_year, birth_date.tm_mon+1, birth_date.tm_mday));
        role_field->setCurrentIndex(
                std::find_if(roles.begin(), roles.end(),
                             [=](const Role& found) {return found.id == member->role->id;})
                - roles.begin());
        experience_field->setValue(member->experience);
        salary_field->setValue(member->salary);
    }

    form_layout->addRow("First name:", first_name_field);
    form_layout->addRow("Last name:", last_name_field);
    form_layout->addRow("Patronymic:", patronymic_field);
    form_layout->addRow("Birth date:", birth_date_field);
    form_layout->addRow("Role:", role_field);
    form_layout->addRow("Experience:", experience_field);
    form_layout->addRow("Salary:", salary_field);

    auto apply_button = new QPushButton(member == nullptr ? "Create" : "Apply");
    QPushButton::connect(apply_button, &QPushButton::clicked, [=](){
        tm birth_date{};
        birth_date.tm_year = birth_date_field->date().year() - 1900;
        birth_date.tm_mon = birth_date_field->date().month() - 1;
        birth_date.tm_mday = birth_date_field->date().day();
        CrewMemberMapper::insert(CrewMember(
                member == nullptr ? 0 : member->id,
                first_name_field->text().toStdString(),
                last_name_field->text().toStdString(),
                patronymic_field->text().toStdString(),
                birth_date,
                &roles.at(role_field->currentIndex()),
                experience_field->value(),
                salary_field->value()
        ));
        window->close();
        refill_staff_table(staff_table_widget);
    });

    grid_layout->addLayout(form_layout, 0, 0, 9, 2);
    grid_layout->addWidget(apply_button, 10, 0, 1, 1);

    window->show();
}

void show_member_description(QTabWidget* tab_widget, QTableWidget* staff_table_widget, QFormLayout* member_desc_layout) {
    int selected_row = staff_table_widget->currentRow();
    if (selected_row == -1)
        return;

    if (member_desc_layout->rowCount() > 0) {
        for (int row = 0; row < 9; row++) {
            member_desc_layout->removeRow(0);
        }
    }

    auto remove_button = new QPushButton("Remove");
    auto edit_button = new QPushButton("Edit...");
    QPushButton::connect(remove_button, &QPushButton::clicked, [=](){
        staff_table_widget->clearSelection();
        CrewMemberMapper::remove(members.at(selected_row));
        refill_staff_table(staff_table_widget);
        remove_button->setDisabled(true);
        edit_button->setDisabled(true);
    });
    QPushButton::connect(edit_button, &QPushButton::clicked, [=](){
        edit_member_window(staff_table_widget, &members.at(selected_row));
        show_member_description(tab_widget, staff_table_widget, member_desc_layout);
    });
    auto buttons_layout = new QGridLayout;
    buttons_layout->addWidget(edit_button, 0, 1, 1, 1);
    buttons_layout->addWidget(remove_button, 0, 0, 1, 1);

    char* birth_date = new char[11];
    strftime(birth_date, 11, "%d.%m.%Y",
             &members.at(selected_row).birth_date);
    char* experience = new char[2];
    sprintf(experience, "%d", members.at(selected_row).experience);
    char* salary = new char[16];
    sprintf(salary, "%d", members.at(selected_row).salary);

    member_desc_layout->addRow("First name:",
                               new QLabel(members.at(selected_row).first_name.c_str()));
    member_desc_layout->addRow("Last name:",
                               new QLabel(members.at(selected_row).last_name.c_str()));
    member_desc_layout->addRow("Patronymic:",
                               new QLabel(members.at(selected_row).patronymic.c_str()));
    member_desc_layout->addRow("Birth date:",
                               new QLabel(birth_date));
    auto ship = members.at(selected_row).get_ship();
    char* ship_label_text = new char[256];
    auto ship_label = new QLabel;
    if (ship != nullptr) {
        sprintf(ship_label_text, "<a href=\"/\">%s</a>", ship->name.c_str());
        ship_label->setText(ship_label_text);
        ship_label->setTextFormat(Qt::RichText);
    } else {
        ship_label->setText("-");
    }
    QLabel::connect(ship_label, &QLabel::linkActivated, [=]() {
        tab_widget->setCurrentIndex(1);
        auto l = (QGridLayout*) tab_widget->currentWidget()->layout();
        auto ships_table_widget = (QTableWidget*) l->itemAtPosition(0, 0)->widget();
        int row = std::find_if(ships.begin(), ships.end(),
                               [=](const Ship& found) {return found.id == ship->id;}) - ships.begin();
        ships_table_widget->setCurrentCell(row, 0);
        ships_table_widget->cellClicked(row, 0);
    });
    member_desc_layout->addRow("Ship:", ship_label);
    member_desc_layout->addRow("Role:",
                               new QLabel(members.at(selected_row).role->title.c_str()));
    member_desc_layout->addRow("Experience:",
                               new QLabel(experience));
    member_desc_layout->addRow("Salary:",
                               new QLabel(salary));
    member_desc_layout->addRow(buttons_layout);
}

QWidget* staff_widget(QTabWidget* tab_widget) {
    auto *staff_widget = new QWidget;
    auto *staff_tab_layout = new QGridLayout;
    staff_widget->setLayout(staff_tab_layout);

    auto *staff_table_widget = new QTableWidget(0, 8);
    staff_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    staff_table_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    staff_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
    staff_table_widget->setHorizontalHeaderLabels(
            QStringList({"First name", "Last name", "Patronymic",
                         "Birth date", "Ship", "Role", "Experience", "Salary"}));
    refill_staff_table(staff_table_widget);

    auto *new_button = new QPushButton("New member...");
    QPushButton::connect(new_button, &QPushButton::clicked, [=](){
        edit_member_window(staff_table_widget);
    });

    auto *member_desc_layout = new QFormLayout;

    QTabWidget::connect(tab_widget, &QTabWidget::currentChanged, [=](int index) {
        if (index == 2) {
            refill_staff_table(staff_table_widget);
            show_member_description(tab_widget, staff_table_widget, member_desc_layout);
        }
    });

    QTableWidget::connect(staff_table_widget, &QTableWidget::cellClicked, [=](){
        show_member_description(tab_widget, staff_table_widget, member_desc_layout);
    });

    staff_tab_layout->addWidget(staff_table_widget, 0, 0, 10, 5);
    staff_tab_layout->addWidget(new_button, 11, 0, 1, 5);
    staff_tab_layout->addLayout(member_desc_layout, 0, 6, 6, 5);

    return staff_widget;
}
