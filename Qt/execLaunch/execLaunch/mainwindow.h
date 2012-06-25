#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QMessageBox>

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


struct Path{
    QFileInfo* exe;
    QDir* output;
    QDir* win;
    QDir* ignore;
    QDir* target;
    QFileInfo* param;
};

/*
struct PathDeleter{
    Path* p;
    PathDeleter(Path* path):
        p(path){}
    ~PathDeleter(){
        if(p->exe != NULL)
            delete p->exe;
        if(p->output != NULL)
            delete p->output;
        if(p->win != NULL)
            delete p->win;
        if(p->ignore != NULL)
            delete p->ignore;
        if(p->target != NULL)
            delete p->target;
        if(p->param != NULL)
            delete p->param;

        p->exe = NULL;
        p->output = NULL;
        p->win = NULL;
        p->ignore = NULL;
        p->target = NULL;
        p->param = NULL;
    }

};
*/

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
    void on_pushButton_addparam_clicked();

private:
    QMessageBox _msg;
    void msg(QString str){
        _msg.setText(str);
        _msg.exec();
    }

    Path _path;
    std::vector < std::vector< Para > > _paramList;
    Ui::MainWindow *ui;
    void createPattern ( const std::vector< RowPat >& pats, std::vector< Para> params, unsigned depth);
    bool isXXXXXX_YYYYY(QString str);
};

#endif // MAINWINDOW_H
