#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filetreewidget.h"
#include "filterwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QToolBar *_folderToolBar;
    QDockWidget *_dock;
    FilterWidget *_filterWidget;

    QAction *_open;
    QAction *_save;
    QAction *_print;

    int _fontSize;

    FileTreeWidget* _ftWidget;

    void createAction();
    void setHeaderFooter(QPainter& painter, const QRect& page, const QString& header, const QString& footer);

private slots:
    void openDir();
    void saveFileList();
    void printFileList();
    void printPreview( QPrinter* printer );
};

#endif // MAINWINDOW_H
