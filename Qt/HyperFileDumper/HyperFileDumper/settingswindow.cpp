#include "settingswindow.h"
#include <QMessageBox>

SettingsWindow::SettingsWindow(QWidget *parent, bool relate, QString font) :
    QDialog(parent)
  , _relate(relate)
  , _font(font)
{

    _pathGroup = new QGroupBox(QString::fromUtf8("パス表示"),this);
    _pathLabel = new QLabel(QString::fromUtf8("印刷・保存する際に表示するパスを選択してください"),_pathGroup);
    _absRB = new QRadioButton(QString::fromUtf8("絶対パス"),_pathGroup);
    _rltRB = new QRadioButton(QString::fromUtf8("相対パス"),_pathGroup);

    QVBoxLayout *pathLayout = new QVBoxLayout(this);
    pathLayout->setSpacing(10);
    pathLayout->setMargin(10);
    pathLayout->addWidget(_pathLabel);
    pathLayout->addWidget(_absRB);
    pathLayout->addWidget(_rltRB);
    _pathGroup->setLayout(pathLayout);
    _pathGroup->setFixedHeight(100);
    _pathGroup->setFixedWidth(300);



    _fontGroup = new QGroupBox(QString::fromUtf8("フォント"),this);
    _fontLabel = new QLabel(QString::fromUtf8("印刷するフォントを選択してください"),_pathGroup);
    _fontList = new QListWidget(_pathGroup);

    QVBoxLayout *fontLayout = new QVBoxLayout(this);
    fontLayout->setSpacing(10);
    fontLayout->setMargin(10);
    fontLayout->addWidget(_fontLabel);
    fontLayout->addWidget(_fontList);
    _fontGroup->setLayout(fontLayout);
    _fontGroup->setFixedWidth(300);
    _fontGroup->setFixedHeight(300);

    _buttonGroup = new QGroupBox(this);
    _pushButton = new QPushButton(QString::fromUtf8("決定"),this);
    _cancelButton = new QPushButton(QString::fromUtf8("キャンセル"),this);

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(_pushButton);
    buttonLayout->addWidget(_cancelButton);
    _buttonGroup->setLayout(buttonLayout);
    _buttonGroup->setFixedWidth(300);


    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setMargin(10);
    layout->addWidget(_pathGroup);
    layout->addWidget(_fontGroup);
    layout->addWidget(_buttonGroup);
    this->setLayout(layout);


    if(_relate){
        _rltRB->setDown(true);
        _rltRB->setChecked(true);
    }else{
        _absRB->setDown(true);
        _absRB->setChecked(true);
    }

    QFontDatabase database;
    foreach (QString family, database.families()) {
        const QStringList styles = database.styles(family);
        if (styles.isEmpty())
            continue;

        QListWidgetItem *familyItem = new QListWidgetItem(_fontList);
        familyItem->setText(family);
        QFont font(family);
        familyItem->setFont(font);

        if(_font == family)
            _fontList->setCurrentRow(_fontList->count()-1);
    }

    connect(_pushButton, SIGNAL(clicked()), SLOT(onOk()));
    connect(_cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));

}

void SettingsWindow::onOk(){
    _relate = _rltRB->isChecked();
    _font = _fontList->currentItem()->text();

    emit changeParam(_relate, _font);
    this->close();
}
void SettingsWindow::onCancel(){
    this->close();
}


void SettingsWindow::setParam(bool relate, QString font){
    _relate = relate;
    _font = font;

    for(int i = 0; i < _fontList->count(); i++ ){
        if( _fontList->item(i)->text() == font ){
            _fontList->setCurrentRow(i);
            break;
        }
    }

}
