#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

typedef  std::vector< std::string > OString;

QString f8(char * str){
    return QString::fromUtf8(str);
}

void dump(QString str){
    QTextStream out(stdout);
    out << QString::fromUtf8(str.toAscii()) << endl;

    QFile outfile("exec.log");
    outfile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    QTextStream outfilestream(&outfile);
    QDateTime date = QDateTime::currentDateTime();
    outfilestream << date.toString("yyyy/MM/dd,hh:mm:ss.zzz,") << QString::fromUtf8(str.toAscii()) << endl;
    outfile.close();
}

int calcBase(const QString& str1, const QString& str2 ){
    int pos1 = str1.length() - str1.indexOf(".") -1;
    int pos2 = str2.length() - str2.indexOf(".") -1;
    int base = qMax(pos1,pos2);
    return base;
}

qulonglong toULL(const QString& str1, const int& base){
    int scale = str1.indexOf(".");
    int ogbase = str1.length() - str1.indexOf(".") -1;
    qulonglong ull = str1.left(scale).toULongLong();
    qulonglong pow=1;
    for(int i =0; i < base; i++){
        pow *= 10;
    }
    ull *= pow;
    if(ogbase == base)
        ull += str1.right(base).toULongLong();
    else{
        qulonglong fl = str1.right(ogbase).toULongLong();
        for(int i = 0; i < base - ogbase; i++){
            fl *= 10;
        }
        ull += fl;
    }
    return ull;
}

QString diffMax(const QString& str1, const QString& str2){
    int base = calcBase(str1, str2);
    qulonglong ull1 = toULL(str1, base);
    qulonglong ull2 = toULL(str2, base);
    if(ull1 > ull2)
        return str1;
    else
        return str2;
}

QString toStr(qulonglong ull, int base){
    QString ret = QString("%1").arg(ull);
    if(ret.length() > base){
        ret.insert( ret.length() - base ,".");
    }
    else{
        int u = ret.length();
        for(int i = 0; i < base - u; i++){
            ret.push_front("0");
        }
        ret.insert( ret.length() - base ,".");
        ret.push_front("0");
    }
    return ret;
}

int makeDiff(QFileInfo xpFileInfo, QFileInfo sevenFileInfo, OString& diffList, int& ret){

    extern QString g_max;

    // xp読み込み
    QFile xpFile( xpFileInfo.absoluteFilePath() );
    if (!xpFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    std::vector< QStringList > xpData;
    while (!xpFile.atEnd()) {
        QString line = xpFile.readLine();
        QStringList list = line.split(" ");

        QStringList coordinate;
        foreach(QString coordstr , list){
            coordinate.append(coordstr.trimmed());
        }

        xpData.push_back(coordinate);
    }
    xpFile.close();

    // 7読み込み
    QFile sevenFile( sevenFileInfo.absoluteFilePath() );
    if (!sevenFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return -2;

    std::vector< QStringList > sevenData;
    while (!sevenFile.atEnd()) {
        QString line = sevenFile.readLine();
        QStringList list = line.split(" ");

        QStringList coordinate;
        foreach(QString coordstr , list){
             coordinate.append(coordstr.trimmed());
        }
        sevenData.push_back(coordinate);
    }
    sevenFile.close();

    // diff
    if(xpData.size() != sevenData.size())
        return -3;



    for(unsigned int i = 0; i < xpData.size(); i++){

        // x, y, z座標でなければ飛ばす
        if(xpData[i].size() != 3)
            continue;

        bool eqX = xpData[i][0] == sevenData[i][0] ;
        bool eqY = xpData[i][1] == sevenData[i][1] ;
        bool eqZ = xpData[i][2] == sevenData[i][2] ;

        // 差分無し
        if(eqX && eqY && eqZ)
            continue;

        // 差分あり
        QString x("0.0"),y("0.0"),z("0.0");
        if(!eqX){
            int base = calcBase(xpData[i][0], sevenData[i][0]);
            qulonglong xpULL = toULL(xpData[i][0], base);
            qulonglong sevenULL = toULL(sevenData[i][0], base);
            qulonglong abs;
            if(xpULL > sevenULL){
                abs = xpULL - sevenULL;
            }
            else{
                abs = sevenULL - xpULL;
            }
            x = toStr(abs, base);
            g_max = diffMax(g_max, x);
        }

        if(!eqY){
            int base = calcBase(xpData[i][1], sevenData[i][1]);
            qulonglong xpULL = toULL(xpData[i][1], base);
            qulonglong sevenULL = toULL(sevenData[i][1], base);
            qulonglong abs;
            if(xpULL > sevenULL){
                abs = xpULL - sevenULL;
            }
            else{
                abs = sevenULL - xpULL;
            }
            y = toStr(abs, base);
            g_max = diffMax(g_max, y);
        }

        if(!eqZ){
            int base = calcBase(xpData[i][2], sevenData[i][2]);
            qulonglong xpULL = toULL(xpData[i][2], base);
            qulonglong sevenULL = toULL(sevenData[i][2], base);
            qulonglong abs;
            if(xpULL > sevenULL){
                abs = xpULL - sevenULL;
            }
            else{
                abs = sevenULL - xpULL;
            }
            z = toStr(abs, base);
            g_max = diffMax(g_max, z);
        }

        diffList.push_back( xpData[i][0].toStdString() );
        diffList.push_back( xpData[i][1].toStdString() );
        diffList.push_back( xpData[i][2].toStdString() );
        diffList.push_back( sevenData[i][0].toStdString() );
        diffList.push_back( sevenData[i][1].toStdString() );
        diffList.push_back( sevenData[i][2].toStdString() );
        diffList.push_back( x.toStdString() );
        diffList.push_back( y.toStdString() );
        diffList.push_back( z.toStdString() );


    }

    return 1;
}


int output(const OString& diffList, const QString& outpath){

    if(diffList.empty())
        return -1;

    // ダンプ
    if(!diffList.empty()){
        QFile output(outpath);
        if (!output.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return -2;
        }

        else{
            QTextStream out(&output);

            bool fi = true;
            for(unsigned int i = 0; i < diffList.size(); i++){
                if(!(i%9)){
                    out << endl;
                }
                out << QString::fromStdString(diffList[i]) << ",";

                if(i == 8){
                    fi =false;
                    extern QString g_max;
                    out << QString::fromUtf8("MAX,") << g_max;
                }

            }
        }

        output.close();
    }


    return 1;
}

#endif // UTIL_H
