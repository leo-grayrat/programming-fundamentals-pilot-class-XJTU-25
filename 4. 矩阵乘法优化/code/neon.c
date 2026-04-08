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
//#include <kblas.h>       // 引用真文件
//#include <kspblas.h>  // 引用真文件
#endif



void matmul_optimized(float** A, float** B, float** C, int size) {
    int i, j, k;
    for (i = 0; i < size; i++) {
        for (k = 0; k < size; k++) {
            //固定住 A 的一个元素，把它贡献给 C 的一整行，反正有+=
            //为了向量化，我们没有办法再按照原本的ijk顺序了
            float tema = A[i][k];
            float32x4_t av = vdupq_n_f32(tema);//获取向量，从单个数//av指a的vector（确信
            //C[i][j] += A[i][k] * B[k][j]，在一次vec运算中
            //B[k][j] 是连续的 4 个不同的数（例如：1.0, 2.0, 3.0, 4.0），我们用 vld1q_f32 加载
            //但是，A[i][k] 在这个内层循环里是固定不变的同一个数（例如：10.0）

            for (j = 0; j < size; j += 4) {
                float32x4_t bv = vld1q_f32(&B[k][j]), cv = vld1q_f32(&C[k][j]);//获取向量，从4个数(实际上是第一个数的地址)
                cv = vmlaq_f32(cv, av, bv);//Result=Addend+(Mul1×Mul2)，自带加乘运算//注意顺序
                vst1q_f32(&C[k][j], cv);//把向量数值存回地址
            }
        }
    }
}

mt neon() {
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

    printf("\n\nCurrently using the [NEON unified vectorization algorithm] for matrix multiplication\n");

    for (count = 1; count <= 5; count++) {
        //调试用
        printf("Test No.%d ...\n", count);

        //随机一次
        datainject(A, size);
        datainject(B, size);

		//printf("oioi\n");

        //稠密*稠密
        start = clock();
        matmul_optimized(A, B, C, size);
        end = clock();
        timing[0][count - 1] = ((double)(end - start)) / CLOCKS_PER_SEC;
        datadelete(C, size);//C之后还要用，先归零

        sparse(A, size);
        //稠密*稀疏
        start = clock();
        matmul_optimized(A, B, C, size);
        end = clock();
        timing[1][count - 1] = ((double)(end - start)) / CLOCKS_PER_SEC;
        datadelete(C, size);//C之后还要用，先归零

        sparse(B, size);
        //稀疏*稀疏
        start = clock();
        matmul_optimized(A, B, C, size);
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