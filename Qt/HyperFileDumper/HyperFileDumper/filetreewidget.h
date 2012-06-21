#ifndef FILETREEWIDGET_H
#define FILETREEWIDGET_H

#include <QTreeWidget>
#include <QtGui>
#include <QIcon>
#include <QDir>

namespace ftw{
    enum ColumnIndex{
        name = 0,
        path = 1,
        abs_path = 2,
        ext = 3
    };
    static const QString indent = "    ";
}

class FileTreeWidget : public QTreeWidget
{

    Q_OBJECT

public:

    FileTreeWidget(QWidget* parent);
    ~FileTreeWidget();

    int openDir(QString path);
    void dump(QFile& file);
    QStringList toListText();
    QString root(){ return _rootDir->dirName(); }

    QIcon* _folderIcon;
    QIcon* _fileIcon;
    QDir* _rootDir;


public slots:
    void crawlDirectory(QDir* parent, QTreeWidgetItem* parentNode, int depth, QList<int>& depthList);
    void updateChecks(QTreeWidgetItem* item, int column);
    void doFiltering(int depth, QStringList extList);

private:

    void checksRecursive(QTreeWidgetItem* parent);
    void parentCheckRecursive(QTreeWidgetItem* parent);
    void toListTextRecursive(QTreeWidgetItem* parent, QStringList& out);
    void extFilterRecursive(QTreeWidgetItem* parent, const QStringList& extList);
    void depthFilterRecursive(QTreeWidgetItem* parent, int curDepth, const int& depth);
    void updateCheckStateRecursive(QTreeWidgetItem* parent);

    int _maxDepth;



};

#endif // FILETREEWIDGET_H
