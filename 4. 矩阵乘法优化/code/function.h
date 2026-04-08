#ifndef FUNCTION_H  // 防止头文件被重复包含的保护盾
#define FUNCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- 环境适配区 ---
#ifdef _MSC_VER 
#include "local_mock.h" 
#else
#include <arm_neon.h>
#include <kblas.h>
#include <kspblas.h>
#endif

// --- 结构体定义区 (搬到这里！让所有文件都能看到) ---
typedef struct ChinaSuperRed {//?我保证我只是随便敲了首字母对应单词
    /*
    float values[NNZ];
    int colidx[NNZ];
    int rowptr[size + 1];
    */
    //NNZ 和 size 是程序跑起来才知道的数字，而编译器在编译时就要定大小
    //不要试图在结构体里存数据，要在结构体里指向数据【指针】

    int /*NNZ*/homo;//homo就是非0数NNZ（什么东西
    int size;

    float* values;
    int* colidx;
    int* rowptr;
}CSR;

typedef struct modeltime {
    double denden;
    double spaden;
    double spaspa;
}mt;

// --- 函数声明区 (菜单) ---
// 只写第一行，后面加分号。不要写函数体！

void datainject(float** a, int size);
void datadelete(float** a, int size);
void sparse(float** a, int size);

mt basic();
mt neon();
mt kblas();
mt kml_spblas();

#endif

