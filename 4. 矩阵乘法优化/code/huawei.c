#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "function.h"

#ifdef _MSC_VER 
    // 如果是在 Visual Studio 这种本地环境
#include "local_mock.h"  // 引用刚才那个假文件
#else
    // 如果是在 华为服务器 (Linux GCC) 环境
#include <arm_neon.h>    // 引用真文件
#include <kblas.h>       // 引用真文件
//#include <kspblas.h>  // 引用真文件
#endif



int chengdu(float** a, int size) {//找0（与成都并非无关//准确的说是找非0数目
    /*
    有反转...
    a[i][j] = chance ? 0 : a[i][j];
    这里赋的 0 是绝对的、数学上的、毫无误差的 0。
    在内存里，它的二进制表示是 0x00000000。
    误差范围：0。它就是纯粹的无。
    判断标准：你完全可以用 if (x == 0.0f) 去判断，不会出任何问题
    没有误差！！！
    */

    //采用 “两遍扫描法”：第一遍扫描：查户口（确定内存大小）
    //因为是动态分配，你一开始不知道有多少个非零元素（精确值未知）
    int i, j;
    int counter = 0;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            //if (!a[i][j]) counter++;
			if (a[i][j]) counter++;//找的是非0数
        }
    }
    return counter;
}

//结构体需要在头文件公共定义
/*
    typedef struct ChinaSuperRed {//?我保证我只是随便敲了首字母对应单词
    
    //float values[NNZ];
    //int colidx[NNZ];
    //int rowptr[size + 1];
    
    //NNZ 和 size 是程序跑起来才知道的数字，而编译器在编译时就要定大小
    //不要试图在结构体里存数据，要在结构体里指向数据【指针】

int homo;//homo就是非0数NNZ（什么东西
int size;

float* values;
int* colidx;
int* rowptr;
}CSR;
*/

CSR sichuan(float** a, int homo, int size, float* values, int* colidx, int* rowptr) {//第二次找（非）0，这次要存了哦//homo就是非0数NNZ（什么东西
    int i, j;
    int k = 0;
    //值得注意的是，所有涉及矩阵第几行第几列的都要求Base-1 索引（即第0列要记为1）！！！
    
    rowptr[0] = 1;//勿忘第一行
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            if (a[i][j]) {
                values[k] = a[i][j];//把矩阵里所有非 0 的数，按行读取，挨个排成一列
                colidx[k] = j + 1;//对应 values 里的每一个数，记录它原本在第几列
                k++;
            }
            //if (k >= homo) goto presentation;
            //k 永远不可能超过 homo，相信你的统计逻辑!
            //这样反而会...
            //如果内存不够了，就强行截断，假装填完了
            //这会导致矩阵后面的一部分数据丢失，算出来的 C 矩阵下半部分可能全是错的
        }
        //一行读完了？是时候告诉csr“下一行的非零元素，是从 values 数组的第几个下标开始的”
        rowptr[i + 1] = k + 1;
    }

//presentation://不要忘了标签后面不能接声明
    {
        CSR res;
        res.homo = homo;
        res.size = size;
        res.values = values;
        res.colidx = colidx;
        res.rowptr = rowptr;
        
        return res;
    }
}

//把以上两个函数组合
CSR junpei(float** a, int size) {//变量名什么的已经坏掉了...
    int homo = chengdu(a, size);

    float* values = (float*)malloc(homo * sizeof(float));
    int* colidx = (int*)malloc(homo * sizeof(int));
    int* rowptr = (int*)malloc((size + 1) * sizeof(int));

    CSR yajuusenpai=sichuan(a, homo, size, values, colidx, rowptr);
    return yajuusenpai;
}

void matmul_huawei_pro_max(float** A, float** B, float** C, int size) {
    CSR ACSR = junpei(A, size), BCSR = junpei(B, size);//注意，这里使用了malloc，记得回收
    float* aval = ACSR.values, * bval = BCSR.values;
    int* acol = ACSR.colidx, * bcol = BCSR.colidx;
    int* arwp = ACSR.rowptr, * brwp = BCSR.rowptr;
    
    /*
    kml_sparse_status_t kml_sparse_scsrmultd(
    const kml_sparse_operation_t opt, // 操作开关：决定 A 是否需要转置 (NO_TRANS, TRANS, CONJ_TRANS)
    const KML_INT m,                  // 维度 M：矩阵 A 的行数 = 结果 C 的行数
    const KML_INT n,                  // 维度 N：矩阵 A 的列数 = 矩阵 B 的行数 (中间消去的维度)
    const KML_INT k,                  // 维度 K：矩阵 B 的列数 = 结果 C 的列数
    const float *a,                   // 矩阵 A (CSR) 的“数值”数组 (Values)：存非零值
    const KML_INT *ja,                // 矩阵 A (CSR) 的“列号”数组 (Col Indices)：基1索引
    const KML_INT *ia,                // 矩阵 A (CSR) 的“行偏移”数组 (Row Pointers)：基1索引，长度 m+1
    const float *b,                   // 矩阵 B (CSR) 的“数值”数组：存非零值
    const KML_INT *jb,                // 矩阵 B (CSR) 的“列号”数组：基1索引
    const KML_INT *ib,                // 矩阵 B (CSR) 的“行偏移”数组：基1索引，长度 n+1
    float *c,                         // 结果矩阵 C (稠密) 的首地址：计算结果填入这里 (一维连续 float*)
    const KML_INT ldc                 // 结果 C 的主维度 (Leading Dimension)：通常填 k (一行的宽度)
    );
    */
    kml_sparse_scsrmultd(0, size, size, size, aval, acol, arwp, bval, bcol, brwp, C[0], size);

    //释放内存！
    free(ACSR.values); free(ACSR.colidx); free(ACSR.rowptr);
    free(BCSR.values); free(BCSR.colidx); free(BCSR.rowptr);
    /*
        出生（Malloc）：
    在 junpei 函数里，你调用了 malloc 申请了堆内存。虽然 values, colidx, rowptr 是在子函数里诞生的，但它们存活在**堆（Heap）**上，不会因为子函数结束而消失。

        传递（Return by Value）：
    junpei 返回了一个 CSR 结构体。注意，结构体是按值返回的，这意味着结构体里的那三个指针变量被复制了一份传给了 ACSR。
    关键点：指针变量被复制了，【但指针指向的地址（那块内存）没有变】。ACSR.values 指向的依然是 junpei 里申请的那块地。

        消亡（Free）：
    在 matmul 函数结束前，你必须手动释放这块地。如果你不 free，这块内存就会变成“孤魂野鬼”（内存泄漏），直到程序彻底退出。
    */
}

mt kml_spblas() {
    // 此时你在VS里敲 vld1q_f32，会有提示，且没有红线
    // 但这个程序只能在 VS 里“看”，不能按 F5 运行（因为函数体是空的）
    // 把这个文件拖到 MobaXterm 编译时，服务器会自动无视 local_mock.h，去读真正的库
    //简而言之，我想在vsstudio里面用智能感知，就这样

    //gemini提醒我，KML_BLAS 库通常要求矩阵内存是连续的
    //而“A[i][j] 每一行 malloc 出来的地址是不连续的。
    //CPU 读取 A[0] 行后，想读 A[1] 行，可能要跳到内存的另一个角落”
    //所以他建议我们用一维数组+索引的方式
    int size = 1024;

    float* data_A = (float*)malloc(size * size * sizeof(float));
    float** A = (float**)malloc(size * sizeof(float*));
    {
        int i;
        for (i = 0; i < size; i++) {
            A[i] = &data_A[i*size];
        }
    }
    //这提醒我们，a[b]运算符其实就是*(a+b)，最极端的例子就是p[2] = 5 -> 2[p] = 5
    
    float* data_B = (float*)malloc(size * size * sizeof(float));
    float** B = (float**)malloc(size * sizeof(float*));
    {
        int i;
        for (i = 0; i < size; i++) {
            B[i] = &data_B[i * size];
        }
    }
    
    float* data_C = (float*)malloc(size * size * sizeof(float));
    float** C = (float**)malloc(size * sizeof(float*));
    {
        int i;
        for (i = 0; i < size; i++) {
            C[i] = &data_C[i * size];
        }
    }

    //初始化矩阵
	datadelete(A, size);
	datadelete(B, size);
	datadelete(C, size);
    
    srand((unsigned)time(NULL));  //使用当前时间作为种子
    //为了确保每次运行程序时生成的随机数不同

    //为保证数据合理性，这里稀疏*稀疏、稀疏*稠密、稠密*稠密均各做5次
    int count;
    clock_t start, end;//起止时间
    double timing[3][5];//存放每次时间

    printf("\n\nCurrently using [the (C)HUAWEI KML_SPBLAS sparse matrix basic algebra library] for matrix multiplication\n");

    for (count = 1; count <= 5; count++) {
        //调试用
        printf("Test No.%d ...\n", count);

        //随机一次
        datainject(A, size);
        datainject(B, size);

		//printf("oioi\n");

        //稠密*稠密
        start = clock();
        matmul_huawei_pro_max(A, B, C, size);
        end = clock();
        timing[0][count - 1] = ((double)(end - start)) / CLOCKS_PER_SEC;
        datadelete(C, size);//C之后还要用，先归零

        sparse(A, size);
        //稠密*稀疏
        start = clock();
        matmul_huawei_pro_max(A, B, C, size);
        end = clock();
        timing[1][count - 1] = ((double)(end - start)) / CLOCKS_PER_SEC;
        datadelete(C, size);//C之后还要用，先归零

        sparse(B, size);
        //稀疏*稀疏
        start = clock();
        matmul_huawei_pro_max(A, B, C, size);
        end = clock();
        timing[2][count - 1] = ((double)(end - start)) / CLOCKS_PER_SEC;
        datadelete(C, size);//C之后还要用，先归零
    }

    double avetime[3];
    {
        int i;
        for (i = 0; i < 3; i++) {
            avetime[i] = 0;
            int j;
            for (j = 0; j < 5; j++) {
                avetime[i] += timing[i][j];
            }
            avetime[i] /= 5.0;
        }
    }

    printf("Timing Results:\n");

    printf("Dense * Dense:\n");
    printf("Average: %.6fs\n", avetime[0]);
    printf("Each run: ");
    for (count = 0; count < 5; count++) {
        printf("%.6fs ", timing[0][count]);
    }
    printf("\nDense * Sparse:\n");
    printf("Average: %.6fs\n", avetime[1]);
    printf("Each run: ");
    for (count = 0; count < 5; count++) {
        printf("%.6fs ", timing[1][count]);
    }
    printf("\nSparse * Sparse:\n");
    printf("Average: %.6fs\n", avetime[2]);
    printf("Each run: ");
    for (count = 0; count < 5; count++) {
        printf("%.6fs ", timing[2][count]);
    }

    mt matrix;
    matrix.denden = avetime[0];
    matrix.spaden = avetime[1];
    matrix.spaspa = avetime[2];

    free(data_A);
    free(A);
    free(data_B);
    free(B);
    free(data_C);
    free(C);

    return matrix;
}