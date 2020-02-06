#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//●预处理需要的东西
#include <QLabel> //可能废弃
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qrect.h>
#include <qfont.h>
#include <qtimer.h>
#include <qline.h>
#include <stdlib.h>
#include <ctime>
#include <QKeyEvent>
#include <qpushbutton.h>
#include <qmessagebox.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void fkmove();                   //方块行为
    bool  fkGD();                     //方块固定判定
    void fkboard();                  //判断方块周围是否畅通无阻
    void QCfk();                    //清除方块








private:
    Ui::MainWindow *ui;

    void keyPressEvent(QKeyEvent *event);  //键盘事件
    void srandnum();                 //生成随机数
    void newgame_Button_1();         //新游戏按钮内容
    void stoporcon_Button_1();        //暂停或继续按钮内容

    QPushButton * newgame_Button;//定义新游戏按钮
    QPushButton * stoporcon_Button;//定义暂停or继续游戏按钮









protected:
    //●声明函数

    void paintEvent (QPaintEvent *); //使用QPainte绘制棋盘，代号X
    void timerEvent(QTimerEvent *t);  //使用计时器







private slots:
    //●声明函数
    void on_newgame_Button_clicked(); //新游戏
    void on_stoporcon_Button_clicked(); //暂停或继续


};

#endif // MAINWINDOW_H






/* 学习笔记
 * ###Qlabel 标签类 继承QFrame###
 * 定义类方法：QLabel *label =new QLabel(this); // *[name]
 * https://blog.csdn.net/xgbing/article/details/7762331
 * ###QPainter： https://blog.csdn.net/hao_zong_yin/article/details/73252571
 *            ： https://www.cnblogs.com/lsgxeva/p/7821550.html
 *            ：
 *
 *
 */
/*笔记
 * public函数
 *QObject::QObject(QObject *parent = nullptr)：构造函数
 *QObject::~QObject()：析构函数
 */
