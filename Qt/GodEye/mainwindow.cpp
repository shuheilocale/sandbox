#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    int sheet = ui->sheet->toPlainText().toInt();
    int x = ui->xxx->toPlainText().toInt();
    int y = ui->yyy->toPlainText().toInt();
    e.Open("d:\\ohyoh.xls");
    ui->textEdit->setText( e.CellValue(sheet,x,y) );
    e.Close();
}

void MainWindow::on_pushButton_2_clicked()
{
    e.Open("d:\\ohyoh.xls");
    e.OpenSheet(1);
}

void MainWindow::on_pushButton_3_clicked()
{
    int x = ui->xxx->toPlainText().toInt();
    int y = ui->yyy->toPlainText().toInt();
    ui->textEdit->setText( e.CellValue(x,y) );
}

void MainWindow::on_pushButton_4_clicked()
{
    e.Close();
}
