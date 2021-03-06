#include "mine.h"
#include "stdlib.h"
#include <time.h>
#include <iostream>
#include <ctime>
using namespace std;

Mine::Mine()
{
    map = NULL;
}


//根据传入参数，生成二维数组地图
bool Mine::GenerateMap(int in_raw, int in_column, int in_mineNum)
{
    if(false == MallocMemForMap(in_raw, in_column, in_mineNum))
    {
        cout << "error: malloc memory failed!" << endl;
        return false;
    }

    if(false == InitMap())
    {
        cout << "error: init map failed!" << endl;
        return false;
    }

    return true;
}

void Mine::reGenerateMap()
{
    InitMap();
}

int Mine::getMineNumber()
{
    return this->mineNum;
}



bool Mine::MallocMemForMap(int in_raw, int in_column, int in_mineNum)
{
    //非法入参
    if(1 > in_raw || 1 > in_column || 1 > in_mineNum || in_mineNum > in_raw * in_column)
    {
        cout << "error: para error!" << endl;
        return false;
    }
    if(NULL != map)
    {
        for(int i = 0; i < raw; i ++)
        {
            delete map[i];
        }
        delete map;
    }
    map = NULL;

    raw = in_raw;
    column = in_column;
    mineNum = in_mineNum;

    map = new unsigned char*[raw];
    for(unsigned char j = 0; j < raw; j ++)
    {
        map[j] = new unsigned char[column];
    }
    return true;
}



//在地图中埋雷，为每个方格填值
bool Mine::InitMap()
{
    int xpos, ypos, mineLeft = mineNum;
    int xs, xe, ys, ye;//填值时使用的上下左右边界值
    //int* set_Mine;//应用洗牌算法的数组

    if(NULL == map)
    {
        cout << "error: null ptr!" << endl;
        return false;
    }
    //初始化内存，全部填0
    for(int i = 0; i < raw; i ++)
    {
        for(int j = 0; j < column; j ++)
        {
            map[i][j] = 0;
        }
    }
///////////////////新加部分////洗牌算法/////////////////////
    //将雷安按行安放在前边
    for(int i = 0; i < mineNum; i++){
        int x = i / column;
        int y = i % column;
        map[x][y] = '*';
    }
    //应用洗牌算法
    for(int i = raw * column - 1; i > 0; i--){
        xpos = i / column;
        ypos = i % column;

        //产生随机坐标
        int randNum = rand() % i;

        int xrand = randNum / column;
        int yrand = randNum % column;

        //进行交换
        unsigned char temp = map[xpos][ypos];
        map[xpos][ypos] = map[xrand][yrand];
        map[xrand][yrand] = temp;
    }

//////////////////////////////结束////////////////////////
    //埋雷---采用随机生成雷坐标的方式,雷使用符号'*'的ASCII
   /* srand(time(NULL));//以当前时间为随机种子，保证随机性
    while(0 != mineLeft)
    {
        xpos = rand() % raw;
        ypos = rand() % column;
        if(0 == map[xpos][ypos])
        {
            map[xpos][ypos] = '*';
            mineLeft --;
        }
        continue;
    }
    */

    //根据雷的分布填充其他方格数值
    for(int i = 0; i < raw; i ++)
    {
        for(int j = 0; j < column; j ++)
        {
            if('*' == map[i][j])
            {
                //设定雷周围可遍历方格的区间
                xs = (i - 1 >= 0) ? (i - 1) : i;
                xe = (i + 1 < raw) ? (i + 1) : i;
                ys = (j - 1 >= 0) ? (j - 1) : j;
                ye = (j + 1 < column) ? (j + 1) : j;

                for(xpos = xs; xpos <= xe ; xpos ++)
                {
                    for(int ypos = ys; ypos <= ye; ypos ++)
                    {
                        if('*' == map[xpos][ypos])
                        {
                            continue;
                        }
                        map[xpos][ypos] ++;
                    }
                }
            }
        }
    }
    return true;
}

void Mine::PrintMap()
{
    for(int i = 0; i < raw; i ++)
    {
        for(int j = 0; j < column; j ++)
        {
            cout << (int)map[i][j] << "\t";
        }
        cout << endl;
    }
}

