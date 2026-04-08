#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "function.h"

// --- 环境适配区 ---
#ifdef _MSC_VER 
#include "local_mock.h" 
#else
#include <arm_neon.h>
#include <kblas.h>
#include <kspblas.h>
#endif

int main() {
	printf("执行四种代码中...\n\n");

	mt bas = basic();
	mt neo = neon();
	mt kbl = kblas();
	mt spb = kml_spblas();

	printf("全部测试完成喵~\n");
	printf("下面集中显示结果('W')ノ\n");
	printf("\n");

	// 1. 顶部边框 (拉宽以容纳3列数据)
	printf("┌──────────────────────────────────────────────────────────────────────────────┐\n");
	printf("│                           多算法矩阵乘法速度比较                             │\n");
	printf("├───────────────────────┬──────────────────┬──────────────────┬────────────────┤\n"); // 分隔线

	// 2. 表头 (手动空格对齐)
	// 这里的空格是根据下面的 %12.8f 加上边距精心计算的
	printf("│        版   本        │    稠密 * 稠密   │    稠密 * 稀疏   │    稀疏 * 稀疏 │\n");
	printf("├───────────────────────┼──────────────────┼──────────────────┼────────────────┤\n");

	// 3. 数据行
	// %14.8f 意思是：总宽度14个字符(不够补空格)，其中包含8位小数。

	// Step 1: Basic
	printf("│ ⮞ Step 1 (BASIC)      │  %14.8f  │  %14.8f  │ %14.8f │\n",
		bas.denden, bas.spaden, bas.spaspa);

	// Step 2: NEON (补全 neo 变量)
	printf("│ ⮞ Step 2 (NEON)       │  %14.8f  │  %14.8f  │ %14.8f │\n",
		neo.denden, neo.spaden, neo.spaspa);

	// Step 3: KML BLAS (补全 kbl 变量)
	printf("│ ⮞ Step 3 (KML)        │  %14.8f  │  %14.8f  │ %14.8f │\n",
		kbl.denden, kbl.spaden, kbl.spaspa);

	// Step 4: KML SPBLAS (补全 spb 变量)
	printf("│ ⮞ Step 4 (KML_SPBLAS) │  %14.8f  │  %14.8f  │ %14.8f │\n",
		spb.denden, spb.spaden, spb.spaspa);

	// 4. 底部边框
	printf("└───────────────────────┴──────────────────┴──────────────────┴────────────────┘\n");

	printf("\n测试结束，感谢使用喵~\n");
	printf("\n");
	printf(" [ 致谢 ]\n");
	printf("\n");
	printf("___________________________________________________________________________\n");
	printf("(C) 2025 Huawei Technologies Co., Ltd. All rights reserved.\n");
	printf("Optimization based on KML (Kunpeng Math Library) & NEON SIMD Instructions.\n");
	printf("Code maintained by %s. Built on %s.\n", "leo_grayrat", __DATE__); // __DATE__ 会自动填入编译日期
	printf("___________________________________________________________________________\n");
}