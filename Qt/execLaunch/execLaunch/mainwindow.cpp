#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProcess>
#include <QFileDialog>

#include <string>
#include <QTextStream>
#include <QMessageBox>

#include <QDebug>
#include <QDateTime>

//#define _SELF_

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ヘッダ作成
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setColumnWidth(0, 30);
    ui->tableWidget->setColumnWidth(1, ui->tableWidget->width()-35);
    QStringList head;
    head.append(QString::fromUtf8("行目"));
    head.append(QString::fromUtf8("パラメタパタン"));
    ui->tableWidget->setHorizontalHeaderLabels(head);

    _path.exe    = new QFileInfo;
    _path.param  = new QFileInfo;
    _path.output = new QDir;
    _path.target = new QDir;
    _path.ignore = new QDir;
    _path.win    = new QDir;

    QFile::exists("C:\\Program Files (x86)") ? ui->radioButton_7->setChecked(true) : ui->radioButton_XP->setChecked(true);

    ui->pushButton_exec->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete _path.exe;
    delete _path.param;
    delete _path.output;
    delete _path.target;
    delete _path.ignore;
    delete _path.win;
    delete ui;
}

// 実行
void MainWindow::on_pushButton_exec_clicked()
{

    //初期化
    // パスに全部入れる
    _path.exe->setFile(ui->lineEdit_exepath->text());
    _path.param->setFile(ui->lineEdit_parampath->text());
    _path.output->setPath(_path.exe->dir().path() + "\\OUTPUT");
    _path.target->setPath(_path.exe->dir().path() + QString::fromUtf8("\\MDGCHK\\指定エレメント"));
    _path.ignore->setPath(_path.exe->dir().path() + QString::fromUtf8("\\MDGCHK\\除外エレメント"));
    if(ui->radioButton_XP->isChecked())
        _path.win->setPath(_path.exe->dir().path() + "\\OUTPUT\\WinXP");
    else
        _path.win->setPath(_path.exe->dir().path() + "\\OUTPUT\\Win7");

  //  PathDeleter pd(&_path);


    ///////////
    // 前処理
    ///////////
    _paramList.clear();

    // パラメタテーブルからデータ構造に変換
    std::vector< RowPat > pats;
    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        bool ok;
        RowPat pat;
        pat.row = ui->tableWidget->item(i,0)->text().toInt(&ok);
        if(!ok)
            continue;
        QStringList patline = ui->tableWidget->item(i,1)->text().split(",");
        foreach(QString str, patline){
            pat.pattern.push_back(str.toStdString());
        }
        pats.push_back(pat);
    }

    // パタン作成　（_paramList）
    std::vector< Para> params;
    createPattern ( pats, params, 0);


    // パラメータファイルを保持しておく
    QFile paramFile(_path.param->absoluteFilePath());
    qDebug() << "inParam : " << _path.param->absoluteFilePath();
    if (!paramFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        msg(QString::fromUtf8("パラメタファイルが開けません"));
        return;
    }

    QTextStream in( &paramFile );
    QStringList orgList;
    while (!in.atEnd()) {
        orgList.append(in.readLine());
    }
    paramFile.close();

    ///////////
    // 実行
    ///////////
    // パラメタの数だけ実行
    bool first = true;
    for(unsigned int i = 0; i < _paramList.size(); i++){
        QStringList curList(orgList);
        QString patname;

        //　パラメタファイルの作成
        for(unsigned int j = 0; j < _paramList[i].size(); j++){
            curList[_paramList[i][j].row-1] = QString::fromStdString( _paramList[i][j].para );

            // パタン名
            patname += QString::fromStdString( _paramList[i][j].para ) + ",";
        }
        patname = patname.left(patname.length()-1);

        QFile outFile(_path.param->absoluteFilePath());
        qDebug() << "outParam : " << _path.param->absoluteFilePath();
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            msg(QString::fromUtf8("パラメタファイルに書き込めません"));
        }

        QTextStream out(&outFile);
        foreach(QString str, curList){
            out << str << endl;
        }
        outFile.close();

#ifndef _SELF_
        // exe実行
        QProcess proc;
        QStringList arg;
        arg.append("%CD%");
        proc.execute(_path.exe->absoluteFilePath(), arg);
#else
        msg(QString::fromUtf8("exeを実行してください。\nexe完了後にOKを押してください。"));
#endif

        ///////////
        // 後処理
        ///////////

        // OUTPUTファイルを移動
        QFileInfoList dateDirList = _path.output->entryInfoList(QDir::Dirs);
        QDir dateDir;
        foreach(QFileInfo fileInfo, dateDirList){
            if(fileInfo.fileName().contains(QRegExp("[0-9]{4}_[0-9]{2}_[0-9]{2}"))){
                dateDir.cd(fileInfo.filePath());
                break;
            }
        }


        // winパスを作る
        _path.output->mkdir(_path.win->dirName());
        // パタンディレクトリを作る(_日付)
        patname += "_" + dateDir.dirName();
        _path.win->mkdir(patname);

        QFileInfoList dateDirFileList = dateDir.entryInfoList(QDir::Files);
        foreach(QFileInfo fileInfo, dateDirFileList){
            QFile file(fileInfo.absoluteFilePath());

            // 初回でかつ、XXXXXX_YYYYYファイルなら
            if(first && isXXXXXX_YYYYY(fileInfo.fileName()) ){
                file.copy(_path.ignore->absolutePath() + "\\" + fileInfo.fileName());
                file.copy(_path.target->absolutePath() + "\\" + fileInfo.fileName());
            }
            file.rename(_path.win->absolutePath() + "\\" + patname + "\\" + fileInfo.fileName());
        }
        first = false;

        // 日付フォルダを消す
        _path.output->rmdir(dateDir.dirName());


        // ログファイル
        QFileInfoList execDirList = _path.exe->absoluteDir().entryInfoList(QDir::Files);
        foreach(QFileInfo fileInfo, execDirList){
            if(fileInfo.suffix() == "log"){
                QFile file(fileInfo.absoluteFilePath());
                file.rename(_path.win->absolutePath() + "\\" + patname + "\\" + fileInfo.fileName());
                break;
            }
        }
    }

    // パラメタファイルを元に戻す
    QFile outFile(_path.param->absoluteFilePath());
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        msg(QString::fromUtf8("パラメタファイルに書き込めません"));
    }
    QTextStream out(&outFile);
    foreach(QString str, orgList){
        out << str << endl;
    }
    outFile.close();

    // 完了
    msg(QString::fromUtf8("完了"));

    return;

}

bool MainWindow::isXXXXXX_YYYYY(QString str){
    if(str.length() != QString("xxxxxx_yyyyy.txt").length())
        return false;

    QRegExp regExp("[0-9]{6}_[0-9]{5}.txt");
    if( -1 == regExp.indexIn(str) )
        return false;
    else
        return true;
}



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



// パラメタ追加
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
     ui->pushButton_exec->setEnabled(true);
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
