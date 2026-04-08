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



void matmul_huawei(float** A, float** B, float** C, int size) {
    /*关于这个函数...
    遵照公式C=α(AB)+βC，也就是说可以保留C的原值的任意比例
    void cblas_sgemm(
    const CBLAS_LAYOUT Layout,    // 内存布局，行主序（C）还是列主序（Fortran）
    const CBLAS_TRANSPOSE TransA, // A 要不要转置?
    const CBLAS_TRANSPOSE TransB, // B 要不要转置?
    const int M,                  // 矩阵 C 的行数
    const int N,                  // 矩阵 C 的列数
    const int K,                  // 中间维度 (A的列数/B的行数)
    const float alpha,            // 公式的 alpha
    const float *A,               // 矩阵 A 的首地址
    const int lda,                // A 的主维度 (跨度)
    const float *B,               // 矩阵 B 的首地址
    const int ldb,                // B 的主维度 (跨度)
    const float beta,             // 公式的 beta
    float *C,                     // 矩阵 C 的首地址 (结果写这里)
    const int ldc                 // C 的主维度 (跨度)
    );
    */
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0f, A[0], size, B[0], size, 0.0f, C[0], size);//f是将小数存为float，否则会默认为double
}

mt kblas() {
    // 此时你在VS里敲 vld1q_f32，会有提示，且没有红线
    // 但这个程序只能在 VS 里“看”，不能按 F5 运行（因为函数体是空的）
    // 把这个文件拖到 MobaXterm 编译时，服务器会自动无视 local_mock.h，去读真正的库
    //简而言之，我想在vsstudio里面用智能感知，就这样

    //gemini提醒我，KML_BLAS 库通常要求矩阵内存是连续的
    //而“A[i][j] 每一行 malloc 出来的地址是不连续的。
    //CPU 读取 A[0] 行后，想读 A[1] 行，可能要跳到内存的另一个角落”
    //所以他建议我们用一维数组+索引的方式
    int size = 1024;

    //已经是mt型了就不能再return0了
	mt errormt;
	errormt.denden = -1.0;
	errormt.spaden = -1.0;
	errormt.spaspa = -1.0;

    //函数原型：int posix_memalign(void **memptr, ...);把唯一的返回值位置用来返回状态码，而不是分配出来的内存指针
    float* data_A;
    //int A_res = posix_memalign(&data_A, 64, size);//64对齐，以便cpu处理
    //int A_res = posix_memalign((void**)&data_A, 64, size * sizeof(float));
	//1.二级指针并不能自动被转换为void** 2.申请size个float的空间，不是申请size个字节
    int A_res = posix_memalign((void**)&data_A, 64, size * size * sizeof(float));
		//二编：一个矩阵tmd有size*size个float
    if (A_res) return errormt;//0是正常，反之即出错
    float** A = (float**)malloc(size * sizeof(float*));
    {
        int i;
        for (i = 0; i < size; i++) {
            A[i] = &data_A[i*size];
        }
    }
    
    float* data_B;
    int B_res = posix_memalign((void**)&data_B, 64, size * size * sizeof(float));
    if (B_res) return errormt;
    float** B = (float**)malloc(size * sizeof(float*));
    {
        int i;
        for (i = 0; i < size; i++) {
            B[i] = &data_B[i * size];
        }
    }
    
    float* data_C;
	int C_res = posix_memalign((void**)&data_C, 64, size * size * sizeof(float));
    if (C_res) return errormt;
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

    printf("\n\nCurrently using [the (C)HUAWEI KML_BLAS linear algebra library] for matrix multiplication\n");

    for (count = 1; count <= 5; count++) {
        //调试用
        printf("Test No.%d ...\n", count);

        //随机一次
        datainject(A, size);
        datainject(B, size);

		//printf("oioi\n");

        //稠密*稠密
        start = clock();
        matmul_huawei(A, B, C, size);
        end = clock();
        timing[0][count - 1] = ((double)(end - start)) / CLOCKS_PER_SEC;
        datadelete(C, size);//C之后还要用，先归零

        sparse(A, size);
        //稠密*稀疏
        start = clock();
        matmul_huawei(A, B, C, size);
        end = clock();
        timing[1][count - 1] = ((double)(end - start)) / CLOCKS_PER_SEC;
        datadelete(C, size);//C之后还要用，先归零

        sparse(B, size);
        //稀疏*稀疏
        start = clock();
        matmul_huawei(A, B, C, size);
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