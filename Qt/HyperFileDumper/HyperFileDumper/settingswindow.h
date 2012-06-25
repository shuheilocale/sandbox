#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QtGui>


class SettingsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsWindow(QWidget *parent = 0, bool relate = true, QString font = QString::fromUtf8("ＭＳ ゴシック"));
    void setParam(bool relate, QString font);

signals:
    void changeParam(bool relative, QString font);
    
public slots:
    void onOk();
    void onCancel();

private:

    bool _relate;
    QString _font;

    QGroupBox *_pathGroup;
    QLabel *_pathLabel;
    QRadioButton *_absRB;
    QRadioButton *_rltRB;

    QGroupBox *_fontGroup;
    QLabel *_fontLabel;
    QListWidget *_fontList;

    QGroupBox *_buttonGroup;
    QPushButton *_pushButton;
    QPushButton *_cancelButton;
};

#endif // SETTINGSWINDOW_H
