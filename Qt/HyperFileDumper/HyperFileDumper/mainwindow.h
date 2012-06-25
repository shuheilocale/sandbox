#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filetreewidget.h"
#include "filterwidget.h"
#include "settingswindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QToolBar *_folderToolBar;
    QStatusBar *_statusBar;
    QDockWidget *_dock;
    FilterWidget *_filterWidget;

    QAction *_open;
    QAction *_save;
    QAction *_print;
    QAction *_settings;
    QAction *_info;

    SettingsWindow *_settingsDlg;

    int _fontSize;
    bool _relative;
    QString _font;

    FileTreeWidget* _ftWidget;

    void createAction();
    void setHeaderFooter(QPainter& painter, const QRect& page, const QString& header, const QString& footer);

private slots:
    void openDir();
    void saveFileList();
    void printFileList();
    void printPreview( QPrinter* printer );
    void showInfoDlg();
    void showSettingsDlg();


public slots:
    void updateStatusBar(QString str);
    void changePrintParam(bool relative, QString font){
        _relative = relative;
        _font = font;
        qDebug() << _font;
    }
};

#endif // MAINWINDOW_H
