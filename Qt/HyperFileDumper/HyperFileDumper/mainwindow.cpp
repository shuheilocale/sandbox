#include "mainwindow.h"
#include "filetreewidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  , _fontSize(12)
{

    _ftWidget = new FileTreeWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(_ftWidget);
    this->setLayout(layout);
    setCentralWidget(_ftWidget);
    this->resize(1000,600);

    createAction();

    _folderToolBar = addToolBar("Folder");
    _folderToolBar->addAction(_open);
    _folderToolBar->addAction(_save);
    _folderToolBar->addAction(_print);

    _filterWidget = new FilterWidget;
    _dock = new QDockWidget(QString::fromUtf8("フィルタ"));
    _dock->setWidget(_filterWidget);
    addDockWidget(Qt::LeftDockWidgetArea, _dock);


    connect(_filterWidget, SIGNAL(useFilter(int, QStringList)), _ftWidget, SLOT(doFiltering(int, QStringList)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::createAction(){

    _open = new QAction(QIcon(":/image/Open.png"), tr("&Open..."), this);
    _open->setShortcuts(QKeySequence::Open);
    _open->setStatusTip(tr("Open root dir..."));
    connect(_open, SIGNAL(triggered()), this, SLOT(openDir()));

    _save = new QAction(QIcon(":/image/Save.png"), tr("&Save..."), this);
    _save->setShortcuts(QKeySequence::Save);
    _save->setStatusTip(tr("Save file list..."));
    _save->setEnabled(false);
    connect(_save, SIGNAL(triggered()), this, SLOT(saveFileList()));

    _print = new QAction(QIcon(":/image/Print.png"), tr("&Print..."), this);
    _print->setShortcuts(QKeySequence::Print);
    _print->setStatusTip(tr("Print file list..."));
    _print->setEnabled(false);
    connect(_print, SIGNAL(triggered()), this, SLOT(printFileList()));
}

void MainWindow::openDir(){
    QString dirPath = QFileDialog::getExistingDirectory(this,QString::fromUtf8("ROOTフォルダを開く"));

    if ( !dirPath.isEmpty() )
    {
        int depth = _ftWidget->openDir(dirPath);
        _filterWidget->setDepth(depth);
    }else{
        _save->setEnabled(false);
        _print->setEnabled(false);
    }
    _save->setEnabled(true);
    _print->setEnabled(true);
    _filterWidget->setEnabledButton(true);
}

void MainWindow::saveFileList(){
    QString savePath = QFileDialog::getSaveFileName(
                this,QString::fromUtf8("ROOTフォルダを開く"),
                "",
                "text file (*.txt);;All Files (*)");

    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }
    _ftWidget->dump(file);

    file.close();

}

// プレビュー
void MainWindow::printFileList(){
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer,this);
    preview.setWindowFlags ( Qt::Window );
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
    //QPrinter printer(QPrinter::ScreenResolution);//プリンター描画のためのペイントデバイス
    //QPrintDialog printDialog(&printer, this);//印刷するかどうかのダイアログ表示
}

void MainWindow::printPreview( QPrinter* printer ){

    QPainter painter( printer );
    painter.begin(printer);

    int      w = printer->pageRect().width();
    int      h = printer->pageRect().height();


    QFont    font(QString::fromUtf8("ＭＳ ゴシック"), _fontSize);
    painter.setFont( font );
    font = QFont(font, painter.device());

    QStringList textList = _ftWidget->toListText();

    // 一番長い文字を取得
    int maxLenIdx = 0;
    for(int i = 0; i < textList.size(); i++){
        if (textList[maxLenIdx].length() != qMax( textList[maxLenIdx].length(), textList[i].length() )){
            maxLenIdx = i;
        }
    }

    int margine = 300;

    QFontMetricsF fontMetrics(font);
    QRectF rect = fontMetrics.boundingRect(textList[maxLenIdx]);


    qreal scale =  w / (rect.width()+ margine*2);
    painter.save();
    if( scale > 1.0f )
        scale = 1.0f;
    painter.scale(scale, scale);
    painter.setBrush(QBrush(Qt::black));

    int y = margine;
    int yStep = rect.height();
    int pageSum = 1;

    // 総ページ数計算
    int pageH = margine;
    for( int i = 0; i < textList.size(); i++){
        if( h/scale < pageH + yStep + margine){
            pageSum++;
            pageH = margine;
        }
        pageH += yStep;
    }

    QRect page( 0, 0, w/scale, h/scale );
    int pageNum = 1;
    this->setHeaderFooter(painter, page, _ftWidget->root(), QString("%1/%2").arg(QString::number(pageNum)).arg(QString::number(pageSum)));

    foreach(QString text, textList){

        painter.drawText(margine, y, text);

        y += yStep;

        if( y + margine > h/scale){
            printer->newPage();
            pageNum++;
            this->setHeaderFooter(painter, page, _ftWidget->root(), QString("%1/%2").arg(QString::number(pageNum)).arg(QString::number(pageSum)));
            y = margine;
        }
    }



    painter.end();
}

void MainWindow::setHeaderFooter(QPainter& painter, const QRect& page, const QString& header, const QString& footer){

    painter.drawText( page, Qt::AlignTop    | Qt::AlignHCenter, header );
    painter.drawText( page, Qt::AlignBottom | Qt::AlignHCenter, footer );
/*    painter.drawText( page, Qt::AlignBottom | Qt::AlignRight,
                      QDateTime::currentDateTime().toString( Qt::DefaultLocaleShortDate ) );
*/
}

