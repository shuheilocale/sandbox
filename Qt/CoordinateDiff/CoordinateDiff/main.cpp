#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QDir>
#include <vector>
#include "util.h"

QString g_max = "0.0";

//#define _DEBUG_

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifndef _DEBUG_
    if(argc != 3){
        dump("引数が違います");
        return -1;
    }
#endif
    dump("開始");

    QString xpPath, sevenPath;
#ifndef _DEBUG_
    QString arg1(QString::fromUtf8(argv[1]));
    if( -1 != arg1.indexOf("xp")){
        xpPath = arg1;
        sevenPath = QString::fromUtf8(argv[2]);
    }
    else{
        xpPath = QString::fromUtf8(argv[2]);
        sevenPath = arg1;
    }
#else
    xpPath = f8("D:\\Programming\\Qt\\CoordinateDiff\\CoordinateDiff-build-desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug\\data\\xp");
    sevenPath = f8("D:\\Programming\\Qt\\CoordinateDiff\\CoordinateDiff-build-desktop-Qt_4_8_1_for_Desktop_-_MinGW__Qt_SDK__Debug\\data\\7");
#endif


    QDir xpFolder(xpPath);
    QFileInfoList xpFileList = xpFolder.entryInfoList(QDir::NoFilter, QDir::Name);

    QDir sevenFolder(sevenPath);
    QFileInfoList sevenFileList = sevenFolder.entryInfoList(QDir::NoFilter, QDir::Name);

    if(xpFileList.size() != sevenFileList.size()){
        dump("ファイル数が違います");
    }

    OString diffList;

    QFileInfoList xpOnlyList;
    // xpフォルダの各ファイルを、7フォルダ内のファイルから探して、
    // 差分を比較する。7フォルダ内に無かったら、そのファイルを記録しておく。
    foreach(QFileInfo xpFile, xpFileList){
        if(xpFile.fileName() == "." || xpFile.fileName() == "..")
            continue;

        bool xpOnly = true;
        foreach(QFileInfo sevenFile, sevenFileList){
            if(xpFile.fileName() == sevenFile.fileName()){

                // 差分を作成
                unsigned int bef = diffList.size();
                int ret = makeDiff( xpFile, sevenFile, diffList, ret);
                if(bef != diffList.size()){
                    dump("差分あり：" + xpFile.fileName());
                }


                // -1 xpファイルオープン失敗
                // -2　7ファイルオープン失敗
                // -3　xp・7ファイルに書かれている行数が違う
                if(ret != 1){
                    if(ret == -1){
                        dump("xpファイルオープン失敗:"+xpFile.fileName());
                    }
                    else if(ret == -2){
                        dump("7ファイルオープン失敗:"+sevenFile.fileName());
                    }
                    else if(ret == -3){
                        dump("xpファイルと7ファイルの行数が違う:"+xpFile.fileName() +", "+ sevenFile.fileName());
                    }
                }

                xpOnly = false;
                break;
            }
        }
        if(xpOnly)
            xpOnlyList.append(xpFile);
    }

    QFileInfoList sevenOnlyList;
    // 7フォルダ内にしかないファイルも探す
    foreach(QFileInfo sevenFile, sevenFileList){
        bool sevenOnly = true;
        foreach(QFileInfo xpFile, xpFileList){
            if(xpFile.fileName() == sevenFile.fileName()){
                sevenOnly = false;
                break;
            }
        }
        if(sevenOnly)
            sevenOnlyList.append(sevenFile);
    }

    int ret = output(diffList, "diff.csv");
    if(ret != 1){
        if(ret == -1){
            dump("全ファイル差分なし");
        }
        else if(ret == -2){
            dump("出力ファイル生成失敗");
        }
    }

    // 片方にないファイル群を出力
    if(!xpOnlyList.isEmpty()){
        dump("xpフォルダにしか存在しないファイル");
        foreach(QFileInfo file, xpOnlyList ){
            dump(file.fileName());
        }
    }

    if(!sevenOnlyList.isEmpty()){
        dump("7フォルダにしか存在しないファイル");
        foreach(QFileInfo file, sevenOnlyList ){
            dump(file.fileName());
        }
    }

    dump("終了");
    return a.exec();
}


