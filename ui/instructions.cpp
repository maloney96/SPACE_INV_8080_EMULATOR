#include "instructions.h"
#include "ui_instructions.h"

Instructions::Instructions(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Instructions)
{
    ui->setupUi(this);
    this->setAutoFillBackground(false);
    //  TODO:  Figure out why the background is staying with SpaceSky
    this->setStyleSheet("background-image: none;");
}

Instructions::~Instructions()
{
    delete ui;
}
