#include "projections.h"
#include "ui_projections.h"

Projections::Projections(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Projections)
{
    ui->setupUi(this);
}

Projections::~Projections()
{
    delete ui;
}

