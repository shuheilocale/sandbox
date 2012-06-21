#include "filterwidget.h"
#include <QtGui>

FilterWidget::FilterWidget(QWidget *parent) :
    QWidget(parent)
{
    // 階層アイテム
    _depthGroup = new QGroupBox(QString::fromUtf8("階層フィルタ"),this);
    _depthLabel = new QLabel(QString::fromUtf8("チェックを外す階層(ROOT=0)を指定する."),_depthGroup);
    _depthCurrent = new QLabel(QString::fromUtf8("フィルタ無効"),_depthGroup);
    _depthSlider = new QSlider(Qt::Horizontal, _depthGroup);
    _depthSlider->setMaximum(0);
    _depthSlider->setMinimum(0);

    // 拡張子アイテム
    _extGroup = new QGroupBox(QString::fromUtf8("拡張子フィルタ"),this);
    _extLabel = new QLabel(QString::fromUtf8("チェックを外す拡張子(ドット無し)を指定する.\n※複数の場合はカンマ区切り"),_extGroup);
    _extLineEdit = new QLineEdit(_extGroup);


    _pushButton = new QPushButton(QString::fromUtf8("フィルタ実行"),this);
    _pushButton->setEnabled(false);


    // 階層
    QVBoxLayout *depthLayout = new QVBoxLayout(this);
    depthLayout->setSpacing(10);
    depthLayout->setMargin(10);
    depthLayout->addWidget(_depthLabel);
    depthLayout->addWidget(_depthCurrent);
    depthLayout->addWidget(_depthSlider);
    _depthGroup->setLayout(depthLayout);
    _depthGroup->setFixedHeight(100);

    // 拡張子
    QVBoxLayout *extLayout = new QVBoxLayout(this);
    extLayout->setSpacing(10);
    extLayout->setMargin(10);
    extLayout->addWidget(_extLabel);
    extLayout->addWidget(_extLineEdit);
    _extGroup->setLayout(extLayout);
    _extGroup->setFixedHeight(100);

    // 全体
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setMargin(10);
    layout->addWidget(_depthGroup);
    layout->addWidget(_extGroup);
    layout->addWidget(_pushButton);
    this->setLayout(layout);

    connect(_pushButton, SIGNAL(clicked()), this, SLOT(clickExtFilter()));
    connect(_depthSlider, SIGNAL(sliderMoved(int)), this, SLOT(updateDepth(int)));

}

void FilterWidget::clickExtFilter(){
    QString plainExt = _extLineEdit->text();
    QStringList extList = plainExt.split(",", QString::SkipEmptyParts);
    foreach(QString ext, extList){
        ext = ext.trimmed();
    }

    emit useFilter(this->_depthSlider->sliderPosition(), extList);
}

void FilterWidget::updateDepth(int depth){
    QString msg;
    if(depth == _depthSlider->maximum()){
        msg = QString::fromUtf8("フィルタ無効");
    }
    else if(depth == _depthSlider->minimum()){
        msg = QString::fromUtf8("全てのチェックを外す");
    }
    else{
        msg = QString::fromUtf8("階層") + QString::number(depth) + QString::fromUtf8("以深のチェックを外す");
    }
    _depthCurrent->setText(msg);
}
