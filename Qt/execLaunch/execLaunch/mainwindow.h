#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

typedef std::vector< std::vector < std::string > > D2String;
typedef std::vector< std::string >                 D1String;

struct RowPat{
    D1String pattern;
    int row;
};

struct Para{
    std::string para;
    int row;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_exec_clicked();

    void on_pushButton_exepath_clicked();

    void on_pushButton_param_clicked();

    void on_pushButton_logpath_clicked();

    void on_pushButton_addparam_2_clicked();

    void on_pushButton_addparam_pre_clicked();

private:
    Ui::MainWindow *ui;
    void createPattern ( const std::vector< RowPat >& pats, std::vector< Para>& paramList);
};

#endif // MAINWINDOW_H
