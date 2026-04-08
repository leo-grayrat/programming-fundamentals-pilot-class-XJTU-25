// local_mock.h
#ifndef LOCAL_MOCK_H
#define LOCAL_MOCK_H

// _MSC_VER 是 Visual Studio 独有的宏，只有在本地 VS 打开时才会进入这里
#ifdef _MSC_VER 

#include <stdio.h>
#include <stdlib.h>

// ==========================================
// 0. 伪造 Linux 内存管理函数
// ==========================================
// 这一步是为了欺骗 VS，让它觉得本地有 posix_memalign 这个函数
// 实际上我们在本地直接调用 malloc (虽然不对齐，但能跑通逻辑且不崩)

static int posix_memalign(void** memptr, size_t alignment, size_t size) {
    // 在 Windows 本地调试时，直接用 malloc 顶替
    // 注意：这样虽然没有 64字节对齐，但不会报错，
    // 而且可以用普通的 free() 释放，避免了 _aligned_free 的麻烦
    *memptr = malloc(size);

    // 如果申请失败返回非0，成功返回0 (符合 POSIX 标准)
    return (*memptr == NULL) ? 1 : 0;
}

// ==========================================
// 1. 伪造 ARM NEON (向量化指令)
// ==========================================
// 定义向量类型 (假装它是个结构体)
typedef struct { float val[4]; } float32x4_t;
typedef struct { int val[4]; } int32x4_t;

// 定义用到的函数原型 (只声明，不需要实现，VS 只要看到名字就不报错)
// 加载
static float32x4_t vld1q_f32(const float* ptr) { float32x4_t t; return t; }
static int32x4_t vld1q_s32(const int* ptr) { int32x4_t t; return t; }
// 扩展
static float32x4_t vdupq_n_f32(float value) { float32x4_t t; return t; }//gemini啊，你为什么会单缺一条这个啊
static int32x4_t vdupq_n_s32(int value) { int32x4_t t; return t; }
// 运算
static float32x4_t vmlaq_f32(float32x4_t a, float32x4_t b, float32x4_t c) { return a; }
static int32x4_t vmlaq_s32(int32x4_t a, int32x4_t b, int32x4_t c) { return a; }
// 存储
static void vst1q_f32(float* ptr, float32x4_t val) {}
static void vst1q_s32(int* ptr, int32x4_t val) {}

// ==========================================
// 2. 伪造 KML_BLAS (基础代数库)
// ==========================================
// 定义枚举类型
typedef enum { CblasRowMajor = 101, CblasColMajor = 102 } CBLAS_LAYOUT;
typedef enum { CblasNoTrans = 111, CblasTrans = 112, CblasConjTrans = 113 } CBLAS_TRANSPOSE;

// 定义函数原型
static void cblas_sgemm(const CBLAS_LAYOUT Layout, const CBLAS_TRANSPOSE TransA,
    const CBLAS_TRANSPOSE TransB, const int M, const int N,
    const int K, const float alpha, const float* A,
    const int lda, const float* B, const int ldb,
    const float beta, float* C, const int ldc) {
}

// ==========================================
// 3. 伪造 KML_SPBLAS (稀疏矩阵库)
// ==========================================
// 定义句柄和类型
typedef void* kml_sparse_operation_t;
typedef enum { KML_SPARSE_OPERATION_NON_TRANSPOSE } kml_sparse_operation_e;
typedef enum { KML_SPARSE_FORMAT_CSR } kml_sparse_format_t;
typedef int kml_sparse_status_t;

// 定义函数原型
// 必须和文档里的参数一一对应：
// 12个参数：opt, m, n, k, a(val), ja(col), ia(row), b(val), jb(col), ib(row), c(out), ldc
static int kml_sparse_scsrmultd(
    const int opt,
    const int m, const int n, const int k,
    const float* a, const int* ja, const int* ia, // 矩阵 A 的三件套
    const float* b, const int* jb, const int* ib, // 矩阵 B 的三件套
    float* c, const int ldc                       // 结果 C
) {
    return 0;
}

#endif // 结束 _MSC_VER
#endif // 结束 LOCAL_MOCK_H
