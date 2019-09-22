#include "window.h"
#include "ui_window.h"
#include <cstdio>

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);
    this->ui->widget->setFocus();
    connect (ui->posX, SIGNAL (valueChanged(double)), ui->widget, SLOT (posX_changed(double)));
    connect (ui->posY, SIGNAL (valueChanged(double)), ui->widget, SLOT (posY_changed(double)));
    connect (ui->posZ, SIGNAL (valueChanged(double)), ui->widget, SLOT (posZ_changed(double)));
}


Window::~Window()
{
    delete ui;
}
