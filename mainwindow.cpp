#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

   /* QPalette palette(this->palette());
    palette.setColor(QPalette::Background, Qt::black);
    this->setPalette(palette);*/

    //初始化各个成员变量。
    STATE_FLAG.IsOver = 0;
    STATE_FLAG.IsStart = 0;
    STATE_FLAG.result = 0;
    map_flag = NULL;
    map_XOffset = 5;
    map_YOffset = 64;
    time_XOffset = 0;
    time_YOffset = 27;

    this->time_cost = 0;
    this->timer = new QTimer(this);
    connect(this->timer,SIGNAL(timeout()),this,SLOT(on_timeChange()));

    /*start:
     * 1.默认为初级,每个方格25*25，上方留出一定区域用于显示时间、剩余雷数等信息
     * 2.初始话随机地图，和记录是否访问过的地图 */
    setFixedSize(defaultMap[0][1] * 25 + map_XOffset * 2 , defaultMap[0][0] * 25 + map_YOffset + 5);
    GenerateGlobalMap(defaultMap[0][0],defaultMap[0][1],defaultMap[0][2]);
    /*end*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPixmap bmpblock;
    QPixmap bmpdigital;
    QPixmap bmpemoji;
    //false可以互换位置
    //if(false == bmpblock.load("E:\mineWrapper\mine_sweeping-master_OK!!/src/block.bmp") || false == bmpdigital.load("E:\mineWrapper\mine_sweeping-master_OK!!/src/digital_32_25.bmp") || false == bmpemoji.load("E:\mineWrapper\mine_sweeping-master_OK!!/src/emoji.bmp"))
//    if(false == bmpblock.load("../src/block.bmp") || false == bmpdigital.load("../src/digital_32_25.bmp") || false == bmpemoji.load("../src/emoji.bmp"))

    if(false == bmpblock.load("../src/block__.png") || false == bmpdigital.load("../src/digital_32_25_.png") || false == bmpemoji.load("../src/we.png"))
    {
        cout << "error:load bmp failed!" << endl;
    }
    else
    {
        cout << "info:load bmp success!" << endl;
    }
    QPainter painter(this);

    /*绘制格子的处理流程***************************************************************************Start*/
    for(int i = 0; i < this->raw; i ++)
    {
        for(int j = 0; j < this->column; j ++)
        {
            //没有左击过的方格的绘制
            if(0 == map_flag[i][j])
            {
                painter.drawPixmap(j * 25 + map_XOffset, i * 25 + this->map_YOffset, bmpblock, 9 * 25, 0, 25, 25);
            }
            //左击过的方格绘制
            else if(1 == map_flag[i][j])
            {
                if(m.map[i][j] < 9)
                {
                    painter.drawPixmap(j * 25 + map_XOffset, i * 25 + this->map_YOffset, bmpblock, m.map[i][j] * 25, 0, 25, 25);
                }

                else if(m.map[i][j] == '*')
                {
                    painter.drawPixmap(j * 25 + map_XOffset, i * 25 + this->map_YOffset, bmpblock, 11 * 25, 0, 25, 25);
                }
            }
            //插小旗的绘制
            else if(2 == map_flag[i][j])
            {
                painter.drawPixmap(j * 25 + map_XOffset, i * 25 + this->map_YOffset, bmpblock, 12 * 25, 0, 25, 25);
            }
            //插错旗:3
            else
            {
                painter.drawPixmap(j * 25 + map_XOffset, i * 25 + this->map_YOffset, bmpblock, 13 * 25, 0, 25, 25);
            }
        }
    }



    //游戏已结束
    if(1 == STATE_FLAG.IsOver)
    {
        //游戏失败,所有未翻开的雷全部翻开
        if(0 == STATE_FLAG.result)
        {
            for(int i = 0; i < this->raw; i ++)
            {
                for(int j = 0; j < this->column; j ++)
                {
                    if(0 == map_flag[i][j] && m.map[i][j] == '*')
                    {
                        painter.drawPixmap(j * 25 + map_XOffset, i * 25 + this->map_YOffset, bmpblock, 10 * 25, 0, 25, 25);
                    }
                }
            }
            // 表情切换为哭
            painter.drawPixmap((this->column * 25 + 2 * map_XOffset) / 2 - 15, this->time_YOffset, bmpemoji, 2 * 31, 0, 31, 31);

        }
        //游戏胜利，切换表情，点击锁定
        else
        {
            painter.drawPixmap((this->column * 25 + 2 * map_XOffset) / 2 - 15, this->time_YOffset, bmpemoji, 1 * 31, 0, 31, 31);
        }

    }
    else
    {
        painter.drawPixmap((this->column * 25 + 2 * map_XOffset) / 2 - 15, this->time_YOffset, bmpemoji, 0, 0, 31, 31);
    }
    /*绘制格子的处理流程***************************************************************************End*/

    /*绘制剩余雷数的统计处理流程********************************************************************Start*/
    int gtemp,stemp,btemp,mine_leftTemp;
    //gtemp,stemp,btemp分别为剩余雷数值的个十百位的数值
    mine_leftTemp = mine_Left < 1 ? 0 : mine_Left;
    btemp = mine_leftTemp / 100;
    stemp = mine_leftTemp / 10 - btemp * 10;
    gtemp = mine_leftTemp - btemp * 100 - stemp * 10;
    cout << "mineLeftNum: " << this->mine_Left << "  btemp: " << btemp << "  stemp: " << stemp << "  gtemp:" << gtemp << endl;
    painter.drawPixmap(0 + map_XOffset, this->time_YOffset, bmpdigital, btemp * 25, 0, 25, 32);
    painter.drawPixmap(25 + map_XOffset, this->time_YOffset, bmpdigital, stemp * 25, 0, 25, 32);
    painter.drawPixmap(50 + map_XOffset, this->time_YOffset, bmpdigital, gtemp * 25, 0, 25, 32);
    /*绘制剩余雷数的统计处理流程**********************************************************************End*/

    /*绘制时间统计的处理流程************************************************************************Start*/
    int time_temp = this->time_cost;
    btemp = time_temp / 100;
    stemp = time_temp / 10 - btemp * 10;
    gtemp = time_temp - btemp * 100 - stemp * 10;
    cout << "timecost: " << this->time_cost << "  btemp: " << btemp << "  stemp: " << stemp << "  gtemp:" << gtemp << endl;
    painter.drawPixmap(this->column * 25 + map_XOffset - 75, this->time_YOffset, bmpdigital, btemp * 25, 0, 25, 32);
    painter.drawPixmap(this->column * 25 + map_XOffset - 50, this->time_YOffset, bmpdigital, stemp * 25, 0, 25, 32);
    painter.drawPixmap(this->column * 25 + map_XOffset - 25, this->time_YOffset, bmpdigital, gtemp * 25, 0, 25, 32);
    /*绘制时间统计的处理流程************************************************************************End*/
}

//修改后的双击事件
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event){
    int ppx = event->x();
    int ppy = event->y();
    if(!((ppx > map_XOffset && ppx < 25 * this->column + map_XOffset) && (ppy > map_YOffset && ppy < this->raw * 25 + map_YOffset)))
        return;

    if(1 == STATE_FLAG.IsOver)
    {
        return;
    }
    changePosToIndex(&ppx, &ppy);
    if((Qt::LeftButton|Qt::RightButton) == event->buttons())
    {
        //如果游戏还没有开始，不响应左双击事件
        if(0 == STATE_FLAG.IsStart)
        {
            return;
        }
        pressLeftRightButtonPorc(ppy, ppx);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    int ppx = event->x();
    int ppy = event->y();
    cout << "ppx: " << ppx << "  ppy: " << ppy << endl;
    cout << "xArea: " << map_XOffset << "," << 25 * this->column + map_XOffset << endl;
    cout << "yArea: " << map_YOffset << "," << 25 * this->raw + map_YOffset << endl;
    //如果点击位置不在方格区域内，点击表情进行处理，否则不处理
    if(!((ppx > map_XOffset && ppx < 25 * this->column + map_XOffset) && (ppy > map_YOffset && ppy < this->raw * 25 + map_YOffset)))
        //不在方格区域内
    {
        int temp_x = (this->column * 25 + 2 * map_XOffset) / 2 - 15;
        int temp_y = this->time_YOffset;
        if(ppx >= temp_x && ppx <= temp_x + 31 && ppy >= temp_y && ppy <= temp_y + 31)
        {
            on_actionRestart_triggered();
        }
        //cout << "error: not in the correct area!" << endl;
        return;
    }
    //如果游戏处于结束状态，不响应点击事件
    if(1 == STATE_FLAG.IsOver)
    {
        return;
    }
    changePosToIndex(&ppx, &ppy);
    if((Qt::LeftButton|Qt::RightButton) == event->buttons())
    {
        //如果游戏还没有开始，不响应左右双击事件
        if(0 == STATE_FLAG.IsStart)
        {
            return;
        }
        pressLeftRightButtonPorc(ppy, ppx);
    }
    else if(Qt::LeftButton == event->button())//点击
    {
        if(0 == STATE_FLAG.IsStart)//如果游戏还没有开始，那么左击一下，即代表游戏开始，并开始计时
        {
            STATE_FLAG.IsStart = 1;
            this->timer->start(1000);
        }
        pressLeftButtonProc(ppy, ppx);
    }
    else if(Qt::RightButton == event->button())
    {
        pressRightButtonProc(ppy, ppx);
    }

    //更新游戏状态
    JudgeIsOver();
    update();//重新启动窗口

}

//为上层标记地图申请内存，指针为maindow的成员
//删除原来raw，column构成的map，使用新的in_raw,in_column构成新的map
bool MainWindow::MallocMemForMap_IsPushed(int in_raw, int in_column)
{
    if(1 > in_raw || 1 > in_column)
    {
        cout << "error: malloc mem for map_IsPushed failed !" << endl;
        return false;
    }
    if(NULL != map_flag)
    {
        for(int i = 0; i < raw ;i ++)
        {
            delete map_flag[i];
        }
        delete map_flag;
        map_flag = NULL;
    }

    raw = in_raw;
    column = in_column;

    map_flag = new int*[raw];
    for(int j = 0; j < raw; j ++)
    {
        map_flag[j] = new int[column];
    }

    return true;
}

//为上层标记地图初始化，用0、1、2代表未访问过、访问过、插旗标志
void MainWindow::InitMap_IsPushed()
{
    for(int i = 0 ;i < raw ;i ++)
    {
        for(int j = 0; j < column; j ++)
        {
            map_flag[i][j] = 0;//将所有格子都初始化为为访问过的格式
        }
    }
}

//生成上层标记地图，调用MallocMemForMap_IsPushed、InitMap_IsPushed
bool MainWindow::GeneratePushedMap(int in_raw, int in_column)
{
    if(false == MallocMemForMap_IsPushed(in_raw, in_column))
    {
        cout << "error: malloc mem for map_IsPushed failed!!" << endl;
        return false;
    }

    InitMap_IsPushed();
    return true;
}

void MainWindow::changePosToIndex(int *px, int *py)
{
    //进入此函数，由外部调用保证点击区域为方块区域
    *px = (*px - map_XOffset) / 25;
    *py = (*py - map_YOffset) / 25;
}

void MainWindow::recursiveFreshBlock(int raw_pos, int col_pos)
{
    int raw_s, raw_e, col_s, col_e;
    //设定周围可遍历方格的区间
    raw_s = (raw_pos - 1 >= 0) ? (raw_pos - 1) : raw_pos;
    raw_e = (raw_pos + 1 < raw) ? (raw_pos + 1) : raw_pos;
    col_s = (col_pos - 1 >= 0) ? (col_pos - 1) : col_pos;
    col_e = (col_pos + 1 < column) ? (col_pos + 1) : col_pos;

    map_flag[raw_pos][col_pos] = 1;//？？

    for(int i = raw_s; i <= raw_e; i ++)
    {
        for(int j = col_s; j <= col_e; j ++)
        {
            if(i == raw_pos && j == col_pos)
                continue;
            if(0 == m.map[i][j] && 0 == map_flag[i][j])
                recursiveFreshBlock(i, j);
            else
                map_flag[i][j] = 1;
        }
    }
}

//双击事件的具体实现过程
void MainWindow::pressLeftRightButtonPorc(int raw_pos, int col_pos)//行列与横纵坐标相反
{
    //先统计周围一圈的小旗个数，比较是否和此块数值相同
    int cnt = 0;//方格周围的小旗数
    int raw_s, raw_e, col_s, col_e;//上下左右边界值
    //设定周围可遍历方格的区间
    raw_s = (raw_pos - 1 >= 0) ? (raw_pos - 1) : raw_pos;
    raw_e = (raw_pos + 1 < raw) ? (raw_pos + 1) : raw_pos;
    col_s = (col_pos - 1 >= 0) ? (col_pos - 1) : col_pos;
    col_e = (col_pos + 1 < column) ? (col_pos + 1) : col_pos;

    //统计该方格周围的小旗数cnt
    for(int i = raw_s; i <= raw_e;i ++)
    {
        for(int j = col_s; j <= col_e; j ++)
        {
            if(2 == map_flag[i][j])//插上了小旗
            {
                cnt ++;
            }
        }
    }

    //判断小旗数是否等于该方格显示的数值
    if(cnt != m.map[raw_pos][col_pos])
    {
        return;
    }
    else
    {
        for(int i = raw_s; i <= raw_e;i ++)
        {
            for(int j = col_s; j <= col_e; j ++)
            {
                switch(map_flag[i][j])
                {
                case 2:
                    //插错旗的情况
                    if('*' != m.map[i][j])
                    {
                        map_flag[i][j] = 3;
                    }
                    break;
                case 0:
                    pressLeftButtonProc(i, j);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void MainWindow::pressLeftButtonProc(int raw_pos, int col_pos)
{
    if(1 == map_flag[raw_pos][col_pos] || 2 == map_flag[raw_pos][col_pos])
    {
        //已经点击的区域或已插上红旗的区域，不需要处理
        return;
    }
    map_flag[raw_pos][col_pos] = 1;
    if(0 == m.map[raw_pos][col_pos])
    {
        recursiveFreshBlock(raw_pos, col_pos);
    }
}

void MainWindow::pressRightButtonProc(int raw_pos, int col_pos)
{
    switch (map_flag[raw_pos][col_pos])
    {
    case 1:
        return;
    case 2:
        map_flag[raw_pos][col_pos] = 0;
        this->mine_Left ++;
        break;
    case 0:
        map_flag[raw_pos][col_pos] = 2;
        this->mine_Left --;
        break;
    default:
        break;
    }
}

//检查状态，标志置位处理
void MainWindow::JudgeIsOver()
{
    bool tempflag = 1;
    for(int i = 0; i < this->raw; i ++)
    {
        for(int j = 0; j < this->column; j ++)
        {
            //如果点击到雷，游戏结束，结果判负
            if(1 == map_flag[i][j] && '*' == m.map[i][j])
            {
                STATE_FLAG.IsOver = 1;
                STATE_FLAG.result = 0;
                this->timer->stop();
                return;
            }
            //如果有非雷非0的区域没有被点击，游戏没有结束，继续检查。
            if('*' != m.map[i][j] && 0 != m.map[i][j] && 0 == map_flag[i][j])
            {
                tempflag = 0;
            }
        }
    }
    if(tempflag)
    {
        //所有非雷非0格子都被点过，游戏结束，结果判胜
        STATE_FLAG.IsOver = 1;
        STATE_FLAG.result = 1;
        this->timer->stop();
    }
}

//生成两级地图
bool MainWindow::GenerateGlobalMap(int in_raw, int in_column, int in_mineNum)
{
    mine_Left = in_mineNum;
    //生成一层地图
    if(false == m.GenerateMap(in_raw,in_column,in_mineNum))
    {
        cout << "error: generate map failed !!" << endl;
        return false;
    }
    //生成二层地图
    if(false == GeneratePushedMap(in_raw, in_column))
    {
        cout << "error: generate pushed map failed!!" << endl;
        return false;
    }
    return true;
}

bool MainWindow::ChangeLevel(int in_raw, int in_column, int in_mineNum)
{
    STATE_FLAG.IsOver = 0;
    STATE_FLAG.IsStart = 0;
    setFixedSize(in_column * 25 + map_XOffset * 2 , in_raw * 25 + map_YOffset + 5);
    this->time_cost = 0;
    this->timer->stop();
    GenerateGlobalMap(in_raw, in_column, in_mineNum);
    update();
    return true;
}

//退出游戏
void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

//选择难度
void MainWindow::on_actionprimary_triggered()
{
    ChangeLevel(defaultMap[0][0], defaultMap[0][1], defaultMap[0][2]);
}

void MainWindow::on_actionmedium_triggered()
{
    ChangeLevel(defaultMap[1][0], defaultMap[1][1], defaultMap[1][2]);
}

void MainWindow::on_actionexpert_triggered()
{
    ChangeLevel(defaultMap[2][0], defaultMap[2][1], defaultMap[2][2]);
}

//重新开始
void MainWindow::on_actionRestart_triggered()
{
    STATE_FLAG.IsOver= 0;
    STATE_FLAG.IsStart = 0;
    setFixedSize(column * 25 + map_XOffset * 2 , raw * 25 + map_YOffset + 5);
    this->mine_Left = m.getMineNumber();
    this->time_cost = 0;
    this->timer->stop();
    m.reGenerateMap();
    InitMap_IsPushed();
    update();
}

void MainWindow::on_timeChange()
{
    this->time_cost ++;
    cout << "@@time: " << this->time_cost << endl;
    update();

}

void MainWindow::on_actionVersion_triggered()
{
    QString title = "Version";
    QString content = "Version:2.0\nBasic function is over.\n\t\t\t\t2019.02.15";
    QMessageBox::information(NULL, title, content, QMessageBox::Ok);
}

void MainWindow::on_actionAuthor_triggered()
{
    QString title = "Author";
    QString content = "\t Data Stucture Test\n\t201800301204\n\t18AI\tTianJie";
    QMessageBox::information(NULL, title, content, QMessageBox::Ok);
}
