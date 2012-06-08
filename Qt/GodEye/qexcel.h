#ifndef EXCEL_H
#define EXCEL_H
#include <QAxObject>

class QExcel
{

    enum State{
        OPEN_EXCEL      = 0x0000,
        OPEN_WORKBOOKS  = 0x0001,
        OPEN_WORKBOOK   = 0x0002,
        OPEN_SHEETS     = 0x0004,
        OPEN_SHEET      = 0x0008,
        CLOSE           = 0x0010
    };


public:
    QExcel();
    ~QExcel(){};

    bool Open(QString src);
    bool Close();


    QString CellValue(int sheetIdx, int x, int y);
    QString CellValue(int x, int y); // OpenSheet‚ÌŒã‚ÉŒÄ‚Ô
    bool    OpenSheet(int sheetIdx);


protected:
    QAxObject* _excel;
    QAxObject* _workbooks;
    QAxObject* _workbook;
    QAxObject* _sheets;
    QAxObject* _sheet;
    qint16 state;

};



#endif // EXCEL_H
