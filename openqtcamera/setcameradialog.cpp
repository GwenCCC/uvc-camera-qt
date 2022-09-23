#include "setcameradialog.h"
#include "ui_setcameradialog.h"

setCameraDialog::setCameraDialog(QWidget *parent) : QDialog(parent),
                                                    ui(new Ui::setCameraDialog)
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    ui->setupUi(this);
}

setCameraDialog::~setCameraDialog()
{
    qDebug("[%s] Start! \n", __FUNCTION__);
    delete ui;
}
