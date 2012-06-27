#include <QtCore/QCoreApplication>
#include <iostream>

#include <QTextStream>
#include <QFile>
#include <QDateTime>

#include <QFileInfo>
#include <QDebug>
#include <QDir>



void makeFile(QString filepath){
    QFile outfile( filepath);
    outfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream outfilestream(&outfile);
    QDateTime date = QDateTime::currentDateTime();
    outfilestream << date.toString("yyyy/MM/dd,hh:mm:ss.zzz,") << filepath << endl;
    outfile.close();
}


int main(int argc, char *argv[])
{
    QFileInfo exeFI(argv[0]);
    /*
    QString curDir;
    if(argc >= 2){
        curDir = argv[1];
    }
    */

    exeFI.absoluteDir().mkdir("OUTPUT\\2012_05_02");

    makeFile(exeFI.absolutePath() + "\\OUTPUT\\2012_05_02\\050320_44342.txt");
    makeFile(exeFI.absolutePath() + "\\mdg_check_yyyyyy.log");
    makeFile(exeFI.absolutePath() + "\\OUTPUT\\2012_05_02\\ErrorPoint090402_32323.txt");
    makeFile(exeFI.absolutePath() + "\\OUTPUT\\2012_05_02\\" + QString::fromUtf8("重なりエラー統計.csv"));

    return 1;
}
