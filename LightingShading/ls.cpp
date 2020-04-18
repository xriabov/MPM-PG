#include "ls.h"
#include "ui_ls.h"

LS::LS(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LS)
{
    ui->setupUi(this);
}

LS::~LS()
{
    delete ui;
}

