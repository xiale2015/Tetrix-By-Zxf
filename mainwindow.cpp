#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qpainter.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    newgame_Button_1();
    stoporcon_Button_1();

}

MainWindow::~MainWindow()
{
    delete ui;
}
/*------------------------------------------------声明变量与设置------------------------------------------------*/

//●方块(fk)的图形量化
int fk1[4][4]={{1,1,1,1},{0,0,0,0},{0,0,0,0},{0,0,0,0}}; //横
int fk2[4][4]={{0,1,0,0},{0,1,1,1},{0,0,0,0},{0,0,0,0}}; //竖折
int fk3[4][4]={{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}; //横反折
int fk4[4][4]={{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}; //田
int fk5[4][4]={{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}; //上楼阶梯
int fk6[4][4]={{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}; //倒T
int fk7[4][4]={{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}; //下楼阶梯
int fk[4][4]={0}; //空数组 当前方块
int nfk[4][4]={0};    //下一个方块(有歧义?旋转的中间方块)
//int xy_fk[16][2];     //解决方块旋转出界问题  4*4个坐标，2个维度（x与y）

int _board[46][24]={0};   //对棋盘颜色有无判断的量化，0->没有 //23*2 12*2
int _fkboard[6][6];   //方块周围是否有障碍物判断的量化

//●创建棋盘的数据

const int area_K = 20; //窗口宽（K）（废弃中。。。）
const int area_G = 40; //窗口高（G）（废弃中。。。）
const int cell_size = 40 ; //像素尺寸 20改成40了 有BUG留意一下是不是这里

//●分数存储
long score=0;

//●计时器相关
int shuaxin; //刷新界面 单位ms
int fkdown;  //方块向下移动刷新

//●随机生成
int SRnum; //存储随机数
int flag=0;//判断是否要下一个方块

//●存储键盘的值
int keynum;

//●移动过程的伪坐标运算变量声明
int pos_x=0,pos_y=0;

//●绘画图形中绘制坐标和各种循环用
int i2,j2;

//●用来判定固定方块时的下边界
int ap_book=0;

//●判断游戏进程 -1初始化 0暂停 1继续
int YON=-1;



int ac_book=0;

/*------------------------------------------------声明函数------------------------------------------------*/

//★绘制事件
void MainWindow :: paintEvent(QPaintEvent *)
{

    QPainter painter(this);




//--------------------------------------------------------------------------------------------
//--●UI绘制

    painter.setBrush(QBrush(QColor(255,255,255),Qt::SolidPattern)); //设置画笔（填充）
    painter.drawRect(0,0,800,1000); //窗口底色
    painter.setBrush(QBrush(QColor(109,109,109),Qt::SolidPattern)); //设置画笔（填充）
    painter.setPen(QPen(QColor(109,109,109))); //设置钢笔（边框）109
    painter.drawRect(30,50,480,880); //棋盘底色
    painter.drawRect(540,170,240,290);//下一个预览
    painter.drawRect(540,480,240,260); //分数显示
    painter.setFont(QFont("黑体",18)); //设置字体
    painter.setPen(QPen(QColor(255,255,255))); //设置字体颜色
    painter.drawText(550,150,240,260,Qt::AlignBottom,QStringLiteral("旋转方块:↑\n左右移动：← →\n加速下落：↓\n悔棋不是真君子\n没有下一块预览\n才刺激")); //绘制字体
    painter.drawText(480,380,240,350,Qt::AlignCenter,QStringLiteral("分数:\n\n")+QString::number(score,10)); //绘制字体


//--●棋盘线条-------------------------------------------------------------
/*
    for(i2=0;i2<=23;i2++)
    {
        if(i2==0 || i2==23) painter.setPen((QPen(QColor(255,255,255),4)));
        else            painter.setPen((QPen(QColor(255,255,255),2)));
        painter.drawLine(30,50+i2*cell_size,510,50+i2*cell_size);
    }

    for(j2=0;j2<=12;j2++)
    {
        if (j2==0 || j2==12) painter.setPen((QPen(QColor(255,255,255),4)));
        else               painter.setPen((QPen(QColor(255,255,255),2)));
         painter.drawLine(30+j2*cell_size,50,30+j2*cell_size,970);

    }
*/
//---------------------------------------------------------------------
//--●计算坐标（调试作用）-------------------------------------------------------------
    //painter.drawPoint(50,70); // 1,1   -> 50+(1-1)*40,70+(1-1)*40
    //painter.drawPoint(50,110);// 2,1   -> 50+(1-1)*40,70+(2-1)*40
    //painter.drawPoint(90,150);//3,2    -> ★50+(j-1)*40,70+(i-1)*40  //i(行)与j（列）倒置★
    //painter.drawPoint(90,110);//2,2    ->j-1和i-1就是数组的序号


    //笔记QString::number -> http://www.aiuxian.com/article/p-985596.html

//--------------------------------------------------------------------------------------------

//--●将方块钉死在棋盘上
int i,j;
for(i=0;i<22;i++)
 for(j=0;j<12;j++)
 {
   if (_board[i][j]==1)
     {

       painter.setPen(QPen(QColor(255,255,255),4));
       painter.setBrush(QBrush(QColor(255,206,101)));
       painter.drawRect(j*40+30,i*40+50,40,40);

     }
    else
      {
       painter.setPen(QPen(QColor(109,109,109),Qt::SolidPattern));
       painter.setBrush(QBrush(QColor(109,109,109),Qt::SolidPattern)); //设置画笔（填充）
       painter.drawRect(j*40+30,i*40+50,40,40);
      }


}


//--●绘制方块


     for(i2=0;i2<4;i2++)
    {
         for(j2=0;j2<4;j2++)
            {
             if (fk[i2][j2]==1)
             {

                painter.setPen(QPen(QColor(255,255,255),4));
                painter.setBrush(QBrush(QColor(255,204,204)));

                painter.drawRect(190+j2*40+pos_x*40,50+i2*40+pos_y*40,40,40);
                //备份旧版painter.drawRect(190+j*40+pos_x*40,50+i*40+pos_y*40,40,40);
                //调试：
                //painter.drawRect(190+j*40,250+i*40,40,40);


             }
            }
     }
     painter.setPen(QPen(QColor(255,255,255),Qt::SolidPattern)); //边框
     painter.setBrush(QBrush(QColor(255,255,255),Qt::SolidPattern)); //设置画笔（填充）
     painter.drawRect(20,40,490,130); //顶端遮罩



     /*
//--●计时器事件/刷新游戏


     SRnum=rand()%7+1;         //生成随机数
     //fkmove();
     if (flag==1){MainWindow::srandnum();flag=0;}  //判断随机数
*/
     //QTimer *timer = new QTimer(this);
     //connect(timer,SIGNAL(timeout()),this,SLOT(update()));
     //timer -> start(500);



     if (flag==1)
        {SRnum=rand()%7+1;         //生成随机数
         MainWindow::srandnum();   //判断随机数
         flag=0;}
     if(YON==1 && fkGD() )fkmove();           //判断移动

//--●游戏失败
     int abc=0;
     for(int zzdd=0;zzdd<12;zzdd++)
         if(_board[3][zzdd]==1)
         {

            abc++;
            break;
         }
     if(abc==1)
     {

         on_newgame_Button_clicked();
         killTimer(fkdown);
         killTimer(shuaxin);
         QMessageBox::information(this,"哈哈哈哈","Game Over!重新来吧！");
         shuaxin=startTimer(500);
         fkdown=startTimer(600);


     }







//调试作用
     /*
 painter.drawText(100,100,40,40,Qt::AlignCenter,QString::number(keynum,10));
int zcc,zdd;
 for(zcc=0;zcc<23;zcc++)
     for(zdd=0;zdd<12;zdd++)
 painter.drawText(zdd*40+30,zcc*40+50,40,40,Qt::AlignCenter,QString::number(_board[zcc][zdd],10));
*/


 if(!fkGD()&&ac_book==1)
 {
     int i,j,bx2,by2;//bx2 by2 计算要绘画的棋盘角标

     for(i=0;i<4;i++)
         for(j=0;j<4;j++)
         {
             bx2=(190+j*40+pos_x*40-30)/40;
             by2=(50+i*40+pos_y*40-50)/40;
             if(fk[i][j]==1)
             _board[by2][bx2]= fk[i][j];

         }

     flag=1;
     pos_x=0;
     pos_y=0;
     update();
     ac_book=0;

 }
 //--●清除方块
     QCfk();
}



//★计时器事件/刷新游戏
void MainWindow::timerEvent(QTimerEvent *t)
{
     //QTimer *timer = new QTimer(this);
     //connect(timer,SIGNAL(timeout()),this,SLOT(update()));
     //timer -> start(500);

        if(t ->timerId()==shuaxin) {update();} //比上面的方法优化不好。。。
        else if(t ->timerId()==fkdown)
        {
         if(ap_book != 1 &&  YON ==1 && fkGD() )
         pos_y +=1;
         update();}//方块自动向下



    //if(t ->timerId()==fkdown)  {pos_y +=1;update();}//方块自动向下
}


//★随机生成

void MainWindow::srandnum()
{
    int i,j;

    switch(SRnum) //判断随机数 获取要生成方块的信息
    {
        case 1://方块1
                {
                     for(i=0;i<4;i++)
                     {
                         for(j=0;j<4;j++)
                         {
                             fk[i][j]=fk1[i][j];
                         }
                     }

                }break;
        case 2://方块2
    {
         for(i=0;i<4;i++)
         {
             for(j=0;j<4;j++)
             {
                 fk[i][j]=fk2[i][j];
             }
         }

    }break;
        case 3://方块3
    {
         for(i=0;i<4;i++)
         {
             for(j=0;j<4;j++)
             {
                 fk[i][j]=fk3[i][j];
             }
         }

    }break;
        case 4://方块4
    {
         for(i=0;i<4;i++)
         {
             for(j=0;j<4;j++)
             {
                 fk[i][j]=fk4[i][j];
             }
         }

    }break;
        case 5://方块5
    {
         for(i=0;i<4;i++)
         {
             for(j=0;j<4;j++)
             {
                 fk[i][j]=fk5[i][j];
             }
         }

    }break;
        case 6://方块6
    {
         for(i=0;i<4;i++)
         {
             for(j=0;j<4;j++)
             {
                 fk[i][j]=fk6[i][j];
             }
         }

    }break;
        case 7://方块7
    {
         for(i=0;i<4;i++)
         {
             for(j=0;j<4;j++)
             {
                 fk[i][j]=fk7[i][j];
             }
         }

    }break;

    }
}
//★键盘判定
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if      (event -> key() == Qt::Key_Up)    {keynum = 1;ac_book=0;fkGD();}
    else if (event -> key() == Qt::Key_Down)  {keynum = 2;ac_book=0;fkGD();}
    else if (event -> key() == Qt::Key_Left)  {keynum = 3;ac_book=0;fkGD();}
    else if (event -> key() == Qt::Key_Right) {keynum = 4;ac_book=0;fkGD();}


}
//★方块移动
//吐槽：偶要死了 花了好几个小时原来才发现两个数组换算不能写在一个循环里，分开后发现我写的不是旋转90°，然后又数学归纳了一下公式,最后发现这样旋转。。。。 欲哭无泪.jpg
void MainWindow::fkmove()
{
    int w,v,i,j,mark=-1,book=0;



    if (keynum == -1) {fkdown =startTimer(600);keynum = 0; }
    else if(keynum == 1) //↑
    {
        for(w=0;w<4;w++) //90度转置矩阵
        {
            for(v=0;v<4;v++)
            {
                //nfk[v][w]=fk[w][v];  ←错误纪念

                nfk[3-v][w]=fk[w][v]; //旋转会跳 暂无解决方法



            }
        }

        for(w=0;w<4;w++) //重写数组 换成fk
        {
            for(v=0;v<4;v++)
            {
               fk[w][v]=nfk[w][v];
            }
        }
        for(j=0;j<4;j++) //判断边界
        {
            for(i=0;i<4;i++)
           {
           if(fk[i][j]==1)
           {
            mark = j ;
            break;
           }
           }
            if(mark != -1) break;
        }
        //防止旋转后出界
        while(190+pos_x*40 < 30-mark*40) {pos_x +=1;}
        while(190+40*4+pos_x*40>510){pos_x -=1;}      //加上+(3-mark)*40会溢出 暂无解决方块

        keynum=-1;

    }
    else if (keynum == 2) //↓
    {
       fkdown =startTimer(200);
       keynum = -1; //防止计时器不断重启

    }

    else if (keynum==3) //←
    {
      for(j=0;j<4;j++) //判断边界
      {
          for(i=0;i<4;i++)
         {
         if(fk[i][j]==1)
         {
          mark = j ;
          break;
         }
         }
          if(mark != -1) break;
      }

         if(190+pos_x*40 != 30-mark*40) //方块移动
       {
            //fkboard();
            //for(i=1;i<6;i++) if(_fkboard[i][0]==1) {book=1;break;}
            //if(book=0)
            pos_x -=1;

       }
         keynum=-1;
    }
    else if (keynum==4) //→
    {

             for(j=3;j>=0;j--) //判断边界
             {
                 for(i=0;i<4;i++)
                {
                if(fk[i][j]==1)
                {
                 mark = j ;
                 break;
                }
                }
                 if(mark != -1) break;
             }

                if(190+40*4+pos_x*40 != 510+(3-mark)*40) //方块移动
              {
                    pos_x +=1;

              }
            keynum=-1;

        }




}

// /////////////////////////////////////////////////////////////////////////////
//★方块边界的判断(?废弃中)
/*
void MainWindow::fkboard()
{
int i,j,bx,by;

//坐标逆运算求棋盘数组的下标
bx=(190+j2*40+pos_x*40-30)/40-1;
by=(50+i2*40+pos_y*40-50)/40-1;

//存储边界的数组初始化为1
for(i=0;i<6;i++)
    for(j=0;j<4;j++)
        _fkboard[i][j]=0;

//判断棋盘上方块周围的障碍物

//for(i=0;i<6;i++)  {if(_board[bx-1][by+i-1]  != 0){_fkboard[0][i]=1;} } //2
for(i=1;i<6;i++)  {if(_board[bx+i-1][by-1]  != 0){_fkboard[i][0]=1;} } //3
for(i=0;i<6;i++)  {if(_board[bx+4][by+i-1]  != 0){_fkboard[5][i]=1;} } //4
for(i=1;i<=4;i++) {if(_board[bx+i-1][by+4]  != 0){_fkboard[i][5]=1;} } //1

//判断行为
//for(i=1;i<=4;i++)


 * 上述图示
222222
300001
300001
300001
300001
344444

}
*/
// ////////////////////////////////////////////////////////////////////////////////

bool MainWindow::fkGD() //布尔变量 true 移动 false 固定
{

   int i,j,bx2,by2;//●bx2 by2 是对应的棋盘坐标转换的棋盘下标
   if(ac_book==0)
   {
       for(i=3;i>=0;i--)
       {
           for(j=0;j<4;j++)
           {
               bx2=(190+j*40+pos_x*40-30)/40;
               by2=(50+i*40+(pos_y+1)*40-50)/40;
               if(_board[by2][bx2]== fk[i][j] && _board[by2][bx2]==1 && fk[i][j]==1) {ac_book++;break;}

           }

        if(ac_book==1)break;


       }

   }




   if(ac_book==1)return false;
   else       return true;


}



//★开始游戏
void MainWindow::on_newgame_Button_clicked()
{
    int i,j;
    score=0;
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            fk[i][j]=0;
        }
    }

    for(i=0;i<12;i++)
    {
        _board[22][i]=1;
        _board[23][i]=1;
    }
    for(i=0;i<22;i++)
        for(j=0;j<12;j++)
            _board[i][j]=0;


    srand((unsigned int)(time(NULL))); //使用系统时间作为种子随机数字
    pos_x=0;
    pos_y=0;
    flag =1;
    shuaxin=startTimer(500);   //刷新界面 第二种方案
    fkdown =startTimer(600);  //刷新方块下移
    YON = 1   ;                //游戏进程开始
    ac_book=0;
    stoporcon_Button ->setText("暂停");




}

//★暂停与开始
void MainWindow::on_stoporcon_Button_clicked() //暂停 游戏进程为0，继续 进程为1，开始
{
    keynum=0;
    if(YON==0){stoporcon_Button ->setText("暂停");YON=1;}
    else if(YON==1) {stoporcon_Button ->setText("继续");fkdown=startTimer(1000); YON=0;}


}
//★按钮的初始化设置
void MainWindow::newgame_Button_1() //新游戏按钮的函数内容
{
    newgame_Button = new QPushButton(this);//创建按钮界面
    newgame_Button -> setFocusPolicy(Qt::NoFocus);//失去焦点
    newgame_Button -> setGeometry(QRect(590,790,151,71));  //设置坐标和大小
    newgame_Button ->setFont(QFont("黑体",25));//设置字体
    newgame_Button ->setText("新游戏");//设置名称

    connect(newgame_Button,SIGNAL(clicked()),this,SLOT(on_newgame_Button_clicked()));//连接槽函数

}

void MainWindow::stoporcon_Button_1() //暂停或继续戏按钮的函数内容
{
    stoporcon_Button = new QPushButton(this);
    stoporcon_Button ->setFocusPolicy(Qt::NoFocus);
    stoporcon_Button ->setGeometry(QRect(590,870,151,71));
    stoporcon_Button ->setFont(QFont("黑体",25));
    stoporcon_Button ->setText("不可用");

    connect(stoporcon_Button,SIGNAL(clicked()),this,SLOT(on_stoporcon_Button_clicked()));

}

//★游戏得分
void MainWindow::QCfk()
{
    int i,j,Smark=0,sum=0,p,q,tfk[24][12]={0},cdbook=0; //Smark-计算那一行满了没有 sum计算得分 tfk->temp fk
    for(i=3;i<=21;i++)
    {
        Smark=0;
        for(j=0;j<12;j++)
        {

        if(_board[i][j]==1) Smark++;
        if(Smark==12)      {sum++;p=i;q=j;break;}

        }
        if(Smark==12) break;
    }
   if(Smark==12)
   {
       /*
       for(j=0;j<12;j++)
       {
        _board[p][j]=0;
        }
        */

       for(i=p-1;i>=3;i--)
           for(j=0;j<12;j++)
           {
               //tfk[i+1][j]=_board[i][j];
               _board[i+1][j]=_board[i][j];
       }



      /* for(i=3;i<p+1;i++)
           for(j=0;j<q;j++)
           {
               _board[i][j]=tfk[i][j];
           }
*/
       score+=sum;
       sum=0;
       update();


   }




}
