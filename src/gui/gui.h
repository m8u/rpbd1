#ifndef RPBD1_GUI_H
#define RPBD1_GUI_H

#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLayout>

QWidget* cruises_widget();
QWidget* ships_widget();
QWidget* staff_widget(QTabWidget* tab_widget);
QWidget* charterers_widget();

#endif //RPBD1_GUI_H
