#include "qexcel.h"
#include <QtCore>
#include <QMessageBox>


QExcel::QExcel():
    _excel(NULL)
  , _workbooks(NULL)
  , _workbook(NULL)
  , _sheet(NULL)
{
}

bool QExcel::Open(QString src){

    _excel = new QAxObject( "Excel.Application", 0 );
    _workbooks = _excel->querySubObject( "Workbooks" );
    _workbook = _workbooks->querySubObject( "Open(const QString&)", src );
    _sheets = _workbook->querySubObject( "Worksheets" );
    int count = _sheets->dynamicCall("Count()").toInt();
    if (0 >= count)
        return false;

/*
    QAxObject *sheets = _workbook->querySubObject( "Worksheets" );
    int count = sheets->dynamicCall("Count()").toInt();
    if (0 >= count)
    {
      //   excelState = Error;
         return false;
    }
    //else
    QStringList sheetsList;
    for (int i=1; i<=count; i++)
    {
        QAxObject* sheet = sheets->querySubObject( "Item( int )", i );
        QString name = sheet->dynamicCall("Name()").toString();
        QMessageBox msgBox;
        msgBox.setText(name);
        msgBox.exec();
        sheetsList.append(name);
    }
*/
    return true;
}

bool QExcel::Close(){
    _workbook->dynamicCall( "Close()" );
    _excel->dynamicCall( "Quit()" );
    delete _excel;
    _excel = NULL;
    return true;
}

QString QExcel::CellValue(int sheetIdx, int x, int y){

    if( sheetIdx < 1 || x < 1 || y < 1)
        return NULL;

    QAxObject* sheet = _sheets->querySubObject( "Item( int )", sheetIdx );
    QAxObject* cell = sheet->querySubObject("Cells(int,int)", y, x);
    QString ret = cell->property("Value").toString();
    return ret;
}

QString QExcel::CellValue(int x, int y){

    if( x < 1 || y < 1 || _sheet == NULL)
        return NULL;

    QAxObject* cell = _sheet->querySubObject("Cells(int,int)", y, x);
    QString ret = cell->property("Value").toString();
    return ret;
}

bool QExcel::OpenSheet(int sheetIdx){
    if( sheetIdx < 1 )
        return false;
    _sheet = _sheets->querySubObject( "Item( int )", sheetIdx );

    return true;
}

