#ifndef FILERWIDGET_H
#define FILERWIDGET_H

#include <QWidget>
#include <QtGui>

class FilterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FilterWidget(QWidget *parent = 0);
    void setEnabledButton(bool enable){
        if(_pushButton)
            _pushButton->setEnabled(enable);
    }
    void setDepth(int depth){
        _depthSlider->setMaximum(depth+1);
        _depthSlider->setSliderPosition(depth+1);
    }
    
signals:
    void useFilter(int depth, QStringList ext);
public slots:
private slots:
    void clickExtFilter();
    void updateDepth(int depth);

private:

    QLabel *_depthLabel;
    QLabel *_depthCurrent;
    QSlider *_depthSlider;
    QGroupBox *_depthGroup;

    QLabel *_extLabel;
    QLineEdit *_extLineEdit;
    QGroupBox *_extGroup;

    QPushButton *_pushButton;


};

#endif // FILERWIDGET_H
