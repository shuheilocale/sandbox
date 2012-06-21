#include "filetreewidget.h"

#include <QTreeWidget>

FileTreeWidget::FileTreeWidget(QWidget* parent)
    : QTreeWidget(parent)
    , _folderIcon(NULL)
    , _fileIcon(NULL)
    , _rootDir(NULL)
{

    _folderIcon = new QIcon(":/image/Folder.png");
    _fileIcon = new QIcon(":/image/File.png");
    _rootDir = new QDir;

    QTreeWidgetItem* headerItem = new QTreeWidgetItem();
    headerItem->setText(0,QString::fromUtf8("ファイル名"));
    headerItem->setText(1,QString::fromUtf8("相対パス"));
    headerItem->setText(2,QString::fromUtf8("絶対パス"));
    headerItem->setText(3,QString::fromUtf8("拡張子"));
    this->setHeaderItem(headerItem);

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(updateChecks(QTreeWidgetItem*, int)));
}


int FileTreeWidget::openDir(QString path){

    this->clear();
    _rootDir->setPath(path.replace('\\','/'));

    QTreeWidgetItem* rootItem = new QTreeWidgetItem;
    rootItem->setText(0,_rootDir->dirName());
    rootItem->setText(1,"ROOT");
    rootItem->setText(2,path);
    rootItem->setCheckState(0, Qt::Checked);

    addTopLevelItem(rootItem);
    int depth = 0;
    QList<int> depthList;
    crawlDirectory(_rootDir, rootItem, depth, depthList);
    int maxDepth = 0;
    foreach(int dep, depthList){
        maxDepth = qMax(maxDepth, dep);
    }
    _maxDepth = maxDepth;

    this->expandAll();
    this->resizeColumnToContents(0);
    this->collapseAll();

    return _maxDepth;
}

FileTreeWidget::~FileTreeWidget(){
    if(_folderIcon != NULL)
        delete _folderIcon;
    if(_fileIcon != NULL)
        delete _fileIcon;
    if(_rootDir != NULL)
        delete _rootDir;
}

void FileTreeWidget::crawlDirectory(QDir* parentDir, QTreeWidgetItem* parentItem, int depth, QList<int>& depthList ){

    QFileInfoList fileList = parentDir->entryInfoList();

    foreach(QFileInfo fileInfo, fileList){
        depthList.append(depth+1);

        if( fileInfo.fileName() == "." || fileInfo.fileName() == ".." )
            continue;

        QTreeWidgetItem* childItem = new QTreeWidgetItem();
        childItem->setText(0, fileInfo.fileName());
        childItem->setText(1, "ROOT" + fileInfo.filePath().right(fileInfo.filePath().length() - _rootDir->path().length() ));
        childItem->setText(2, fileInfo.filePath());
        childItem->setCheckState(0, Qt::Checked);

        if(fileInfo.isFile())
        {
            childItem->setIcon(0,*_fileIcon);
            childItem->setText(3,fileInfo.suffix());
        }
        else if(fileInfo.isDir())
        {
            childItem->setIcon(0,*_folderIcon);
            QDir* childDir = new QDir(fileInfo.filePath());
            crawlDirectory(childDir, childItem, depth+1, depthList);
        }
        else{
        }
        parentItem->addChild(childItem);

    }

}


void FileTreeWidget::updateChecks(QTreeWidgetItem* item, int column)
{
    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(updateChecks(QTreeWidgetItem*, int)));
    // Checkstate is stored on column 0
    if(column != 0)
        return;
    Qt::CheckState checkState = item->checkState(0);
    if(checkState != Qt::PartiallyChecked && item->childCount()){
        bool allUnchecked = true;
        for(int i = 0; i < item->childCount(); ++i){
            if( item->child(i)->checkState(0) != Qt::Unchecked ){
                allUnchecked = false;
                break;
            }
        }

        // 子が全てUncheckedの場合にチェック付けた場合は、
        // 子を全てチェックするのではなく、親だけチェックする
        // （この状態でもう一回チェックつけたら子もチェックが付く
        if( checkState == Qt::Checked && allUnchecked ){
            parentCheckRecursive(item->parent());
            connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
                    this, SLOT(updateChecks(QTreeWidgetItem*, int)));
            return;
        }else if( checkState == Qt::Unchecked && allUnchecked ){
            item->setCheckState(0,Qt::Checked);
        }
    }

    checksRecursive(item);
    parentCheckRecursive(item->parent());

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(updateChecks(QTreeWidgetItem*, int)));


    if(this->topLevelItem(0)->checkState(0) == Qt::Unchecked){

    }
    else{

    }
}

void FileTreeWidget::checksRecursive(QTreeWidgetItem* parent)
{
    Qt::CheckState checkState = parent->checkState(0);
    for(int i = 0; i < parent->childCount(); ++i)
    {
       parent->child(i)->setCheckState(0, checkState);
       checksRecursive(parent->child(i));
    }
}

void FileTreeWidget::parentCheckRecursive(QTreeWidgetItem* item){

    if(item == NULL)
        return;

    Qt::CheckState check = item->child(0)->checkState(0);

    bool isPartiallyChecked = false;
    // 子にChecked,Uncheckedが混じっていたら親はPartiallyChecked
    if(check == Qt::PartiallyChecked){
        isPartiallyChecked = true;
    }
    if(!isPartiallyChecked){
        for(int cnt = 1; cnt < item->childCount(); cnt++){
            if( check != item->child(cnt)->checkState(0) ){
                isPartiallyChecked = true;
                break;
            }
        }
    }

    // PartiallyChecked
    if(isPartiallyChecked)
        item->setCheckState(0, Qt::PartiallyChecked);

    // Checked, Unchecked
    else{
      //  if(check == Qt::Checked)
            item->setCheckState(0, check);
    }
    parentCheckRecursive(item->parent());

}


// ファイル出力
void FileTreeWidget::dump(QFile& file){

    QTextStream out(&file);
    foreach(QString strline, this->toListText()){
        out << strline << endl;
    }

    return;
}



// プレビュー
QStringList FileTreeWidget::toListText(){

    QStringList out;

    if( this->topLevelItem(0)->checkState(0) != Qt::Unchecked )
        out.append( this->topLevelItem(0)->text(ftw::path) + QString(" = %1").arg(this->topLevelItem(0)->text(ftw::name)) );

    this->toListTextRecursive(this->topLevelItem(0), out);

    return out;
}

void FileTreeWidget::toListTextRecursive(QTreeWidgetItem* parent, QStringList& out){

    // 子チェック
    bool nothing = true;
    for(int cnt = 0; cnt < parent->childCount(); cnt++ ){
        if( parent->child(cnt)->checkState(0) != Qt::Unchecked ){
            nothing = false;
            break;
        }
    }
    if(nothing)
        return;

    out.append("");

    // 親のパスを出力
    out.append(parent->text(ftw::path) );

    // 子のパスを出力
    for(int cnt = 0; cnt < parent->childCount(); cnt++ ){
        if( parent->child(cnt)->checkState(0) == Qt::Unchecked )
            continue;

        QFileInfo file(parent->child(cnt)->text(ftw::abs_path));
        if(file.isDir())
            out.append( ftw::indent + "<DIR> " + parent->child(cnt)->text(ftw::name) );
        else
            out.append( ftw::indent + parent->child(cnt)->text(ftw::name) );
    }

    // 子の中にディレクトリが存在したら再帰
    for(int cnt = 0; cnt < parent->childCount(); cnt++){
        if(parent->child(cnt)->childCount() && parent->child(cnt)->checkState(0) != Qt::Unchecked ){
            this->toListTextRecursive(parent->child(cnt), out);
        }
    }
}

void FileTreeWidget::doFiltering(int depth, QStringList extList){
    disconnect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(updateChecks(QTreeWidgetItem*, int)));

    // 階層フィルタをカマス
    this->depthFilterRecursive(this->topLevelItem(0), 0, depth);

    // 拡張子フィルタをカマス
    this->extFilterRecursive(this->topLevelItem(0), extList);

    // かました後に、ディレクトリのチェックの種類を設定し直す
    updateCheckStateRecursive(this->topLevelItem(0));

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
            this, SLOT(updateChecks(QTreeWidgetItem*, int)));
}

void FileTreeWidget::depthFilterRecursive(QTreeWidgetItem *parent, int curDepth, const int &depth){

    for(int i = 0; i < parent->childCount(); ++i)
    {
        if( curDepth+1 >= depth )
            parent->child(i)->setCheckState(0, Qt::Unchecked);
        depthFilterRecursive(parent->child(i), curDepth+1, depth);
    }
}

void FileTreeWidget::extFilterRecursive(QTreeWidgetItem* parent, const QStringList& extList)
{
    for(int i = 0; i < parent->childCount(); ++i)
    {
        foreach(QString ext, extList){
            if(ext == parent->child(i)->text(ftw::ext)){
                parent->child(i)->setCheckState(0, Qt::Unchecked);
                break;
            }
        }
        extFilterRecursive(parent->child(i), extList);
    }
}

void FileTreeWidget::updateCheckStateRecursive(QTreeWidgetItem* parent){

    // 枝の末端までたどる
    // 末端だったら１つ上の親のチェックを更新
    bool hasChild = false;
    for(int i = 0; i < parent->childCount(); ++i){
        if( parent->child(i)->childCount() ){
            hasChild = true;
            updateCheckStateRecursive(parent->child(i));
        }
    }

    // 自分のチェック状態を更新
    Qt::CheckState check = parent->child(0)->checkState(0);

    bool isPartiallyChecked = false;
    // 子にChecked,Uncheckedが混じっていたら親はPartiallyChecked
    if(check == Qt::PartiallyChecked){
        isPartiallyChecked = true;
    }
    if(!isPartiallyChecked){
        for(int cnt = 1; cnt < parent->childCount(); cnt++){
            if( check != parent->child(cnt)->checkState(0) ){
                isPartiallyChecked = true;
                break;
            }
        }
    }

    // PartiallyChecked
    if(isPartiallyChecked)
        parent->setCheckState(0, Qt::PartiallyChecked);

    // Checkedのときだけ。Uncheckedのときは、親はそのままにしとく
    else if(check == Qt::Checked){

            parent->setCheckState(0, check);
    }

}

