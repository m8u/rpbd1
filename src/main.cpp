#include <windows.h>
#include <iostream>

#include <QApplication>
#include <QMainWindow>
#include "gui/gui.h"

#include "db/db.h"

using namespace std;


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;
    window.setMinimumSize(800, 600);

    auto *central_widget = new QWidget;
    window.setCentralWidget(central_widget);
    auto *central_layout = new QGridLayout();
    central_widget->setLayout(central_layout);

    auto *tab_widget = new QTabWidget;
    // WARNING!!! TABS CREATION ORDER SHOULD BE EXACTLY LIKE THIS
    auto *charterers_tab = charterers_widget();
    auto *staff_tab = staff_widget(tab_widget);
    auto *ships_tab = ships_widget();
    auto *cruises_tab = cruises_widget();

    tab_widget->addTab(cruises_tab, "Cruises");
    tab_widget->addTab(ships_tab, "Ships");
    tab_widget->addTab(staff_tab, "Staff");
    tab_widget->addTab(charterers_tab, "Charterers");

    central_layout->addWidget(tab_widget, 0, 0, 15, 16);

    window.show();

    return QApplication::exec();
}
