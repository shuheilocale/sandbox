#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProcess>
#include <QFileDialog>

#include <string>
#include <QTextStream>
#include <QMessageBox>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ヘッダ作成
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(0, 30);
    ui->tableWidget->setColumnWidth(1, ui->tableWidget->width()-30);
    QStringList head;
    head.append(QString::fromUtf8("行目"));
    head.append(QString::fromUtf8("パラメタパタン"));
    ui->tableWidget->setHorizontalHeaderLabels(head);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_exec_clicked()
{
    ///////////
    // 前処理
    ///////////

    // パラメタの組み合わせを作成
    // 組み合わせを元にパラメタファイルを書き換え
  /*  std::vector< RowPat > pats;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        RowPat pat;
        pat.row = ui->tableWidget->item(i,0)->text().toInt();
        QStringList patline = ui->tableWidget->item(i,1)->text().split(",");
        foreach(QString str, patline){
            pat.pattern.push_back(str.toStdString());
        }
        pats.push_back(pat);
    }

    std::vector< std::vector < Para > > paramList;
    for(int i = 0; i < ui->tableWidget_pre->rowCount(); i++){
        std::vector< Para > params;
        for(int j = 0; j < ui->tableWidget_pre->columnCount(); j++){
            Para pat;
            pat.row = ui->tableWidget_pre->horizontalHeaderItem(j)->text().toInt();
            pat.para = ui->tableWidget_pre->item(i,j)->text().toStdString();
            params.push_back(pat);
        }
        paramList.push_back(params);
    }
*/
    _paramList.clear();

    std::vector< RowPat > pats;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        RowPat pat;
        pat.row = ui->tableWidget->item(i,0)->text().toInt();
        QStringList patline = ui->tableWidget->item(i,1)->text().split(",");
        foreach(QString str, patline){
            pat.pattern.push_back(str.toStdString());
        }
        pats.push_back(pat);
    }

    std::vector< Para> params;
    createPattern ( pats, params, 0);


    QFile paramFile(ui->lineEdit_parampath->text());

    if (!paramFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in( &paramFile );
    QStringList orgList;
    while (!in.atEnd()) {
        orgList.append(in.readLine());
    }
    paramFile.close();

    //paramFile.rename("hogehoge");

    // パラメタの数
    for(unsigned int i = 0; i < _paramList.size(); i++){
        QStringList curList(orgList);

        //行数
        for(unsigned int j = 0; j < _paramList[i].size(); j++){
            curList[_paramList[i][j].row-1] = QString::fromStdString( _paramList[i][j].para );
        }


        QFile outFile(ui->lineEdit_parampath->text() + QString::number(i));
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        }

        QTextStream out(&outFile);
        foreach(QString str, curList){
            out << str << endl;
        }
        outFile.close();
    }

    ui->lineEdit_where_pre->setText("");

    QMessageBox msg;
    msg.setText(QString::fromUtf8("完了"));
    msg.exec();

    return;


    ///////////
    // 実行
    ///////////
    QProcess proc;
    QStringList arg;
    arg.append("%CD%");
    proc.execute(ui->lineEdit_exepath->text(), arg);

    ///////////
    // 後処理
    ///////////
    ui->lineEdit_where->setEnabled(true);


}



// いらない
void MainWindow::on_pushButton_addparam_2_clicked()
{}


// exeパス選択
void MainWindow::on_pushButton_exepath_clicked()
{
    QString strFName = QFileDialog::getOpenFileName(
            this,
            QString::fromUtf8("実行ファイル"),
            ".",
            tr( "exec (*.exe *.bat *.cmd)" ) );
    if ( !strFName.isEmpty() )
    {
        ui->lineEdit_exepath->setText(strFName);
    }
}


// パラメタファイルパス
void MainWindow::on_pushButton_param_clicked()
{
    QString strFName = QFileDialog::getOpenFileName(
            this,
            QString::fromUtf8("パラメタファイル"),
            ".",
            tr( "param (*.prm)" ) );
    if ( !strFName.isEmpty() )
    {
        ui->lineEdit_parampath->setText(strFName);
    }
}

// ログファイル選択
void MainWindow::on_pushButton_logpath_clicked()
{
    QString strFName = QFileDialog::getOpenFileName(
            this,
            QString::fromUtf8("ログファイル"),
            ".",
            tr( "log (*.*)" ) );
    if ( !strFName.isEmpty() )
    {
        ui->lineEdit_parampath->setText(strFName);
    }
}

void MainWindow::on_pushButton_addparam_pre_clicked()
{

    if( ui->lineEdit_what_pre->text().isEmpty() )
        return;

    QStringList whats = ui->lineEdit_what_pre->text().split(",");
    QStringList wheres = ui->lineEdit_where_pre->text().split(",");

    ui->tableWidget_pre->setColumnCount(wheres.size());
    ui->tableWidget_pre->setHorizontalHeaderLabels(wheres);
    ui->tableWidget_pre->setRowCount(ui->tableWidget_pre->rowCount()+1);
    for(int i = 0; i< whats.size(); i++){
        ui->tableWidget_pre->setColumnWidth(i,30);
        QTableWidgetItem* item = new QTableWidgetItem();
        item->setText(whats[i]);
        ui->tableWidget_pre->setItem(ui->tableWidget_pre->rowCount()-1,i,item);
    }

    ui->lineEdit_where_pre->setEnabled(false);
    ui->lineEdit_what_pre->setText("");
}


/*
  出力
  ・ログファイル
　  →OUTPUT/ *** .log
  　→使った後にどうする？
  ・OUTPUTフォルダ
    →0.0.0_YYYYMMDD にリネームして、どこにおいておく？
  ・パラメタファイル
  　→使ったあとに、どうする？
  ・除外エレメント
    →OUTPUTフォルダ/除外エレメント/　に OUTPUT内のファイルを入れる
  ・指定エレメント
  　→OUTPUTフォルダ/指定エレメント/　に　OUTPUT内のファイルを入れる

*/

void MainWindow::on_pushButton_addparam_clicked()
{
    ui->tableWidget->setColumnWidth(1, ui->tableWidget->width()-50);

     QString where = ui->lineEdit_where->text();
     QString what = ui->lineEdit_what->text();

     ui->tableWidget->setRowCount(ui->tableWidget->rowCount()+1);

     QTableWidgetItem* itemWhere = new QTableWidgetItem();
     itemWhere->setText(where);
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, itemWhere);

     QTableWidgetItem* itemWhat = new QTableWidgetItem();
     itemWhat->setText(what);
     ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, itemWhat);

     ui->lineEdit_what->setText("");
     ui->lineEdit_where->setText("");
     ui->lineEdit_where->setFocus();
}

// テスト用
void MainWindow::on_pushButton_clicked()
{
    _paramList.clear();

    std::vector< RowPat > pats;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        RowPat pat;
        pat.row = ui->tableWidget->item(i,0)->text().toInt();
        QStringList patline = ui->tableWidget->item(i,1)->text().split(",");
        foreach(QString str, patline){
            pat.pattern.push_back(str.toStdString());
        }
        pats.push_back(pat);
    }

    std::vector< Para> params;
    createPattern ( pats, params, 0);


    QMessageBox msg;
    QString str(QString::fromUtf8("行目,　値\n"));
    for(unsigned int i = 0; i < _paramList.size(); i++){
        for(unsigned int j = 0; j < _paramList[i].size(); j++){
            str += QString::number(_paramList[i][j].row) + "," + QString::fromStdString(_paramList[i][j].para) + "\n";
        }
        str += "----\n";
    }
    msg.setText(str);
    msg.exec();
}

void MainWindow::createPattern( const std::vector< RowPat >& pats , std::vector< Para> params , unsigned depth){

    // 下の行がある
    if( pats.size() > depth){

        // 各行のパタン
        for(unsigned int j = 0; j < pats[depth].pattern.size(); j++){
            std::vector< Para > para = params;
            Para p;
            p.para = pats[depth].pattern[j];
            p.row = pats[depth].row;
            para.push_back(p);
            createPattern( pats, para, depth+1);
        }

    }
    // 一番下の行
    else{
        _paramList.push_back(params);
    }

}
