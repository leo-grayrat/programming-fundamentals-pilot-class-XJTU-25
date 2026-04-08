#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//各文件通用函数

void datainject(float** a, int size) {//填入数据
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            a[i][j] = rand() % 100;
        }
    }
}

void datadelete(float** a, int size) {//删除数据，因为有多次结果
    //如果需要的话，也可以用来初始化矩阵
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            a[i][j] = 0;
        }
    }
}

void sparse(float** a, int size) {//疏密化
    int i, j;
    int chance;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            chance = rand() % 10;  //将随机数限制在0到9之间,因为我们要用那个90%赋值为零
            a[i][j] = chance ? 0 : a[i][j];
        }
    }
    //值得注意的是，不能在sparse的时候一旦摇出要把这个数据不归0，就把这个数据位置加到csr中（一边摇号一边填表）
    //realloc？涉及内存搬运，在大循环里调用它是 HPC 的大忌
    //临时变量中转？C 语言处理动态列表很麻烦，链表还没在上个任务把你弄四吗
    //values 和 colidx 数组必须是紧凑的
    //对于计算机来说，遍历一次内存（即便是 100万次）是非常快的，完全不用为了省这一次遍历而牺牲代码的安全性
}