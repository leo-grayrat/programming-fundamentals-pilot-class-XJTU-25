#include <stdio.h>
#include <math.h>
#include <stdlib.h>
double ma(double a, double b) {
	double wotagei = (a > b) ? a : b;
	return wotagei;
}
/*
double mi(double a, double b) {
	double wotagei = (a < b) ? a : b;
	return wotagei;
}*/

double k;//calk函数导致的

double calk(double hatsunemiku) {
	double pjsk = ma(0, /*hatsunemiku - k*/k - hatsunemiku);//看跌期权，你要涨了反而是赔钱（准确的说是算出负值） 
	return pjsk;
}

int main() {
	double s0, /*k,*/ r, sig; int w;
	scanf(/*"%f,%f,%f,%f,%f"*/"%lf %lf %lf %lf %d", &s0, &k, &r, &sig, &w);//输入格式不能乱改啊 
	if( 0.1 <= s0 && k <= 10.0 && 0.1 <= k && s0 <= 10.0 && 0.005 <= r && r <= 0.05 && 0.05 <= sig && sig <= 0.5 && 1 <= w && w <= 26) ;
	else {
		printf("\nError! Please try again.");
		return 0;
	}
	/*时间变化："?t=1/52" 
	上涨概率：u = "e" ^ ("σ*" √("?t"))
	下跌概率：d = 1 / u
	风险中性概率，把未来上涨和下跌可能的价值折现回当前：q = ("e" ^ "r*?t"  "-d") / "u-d"
	利息贴现："e" ^ "-r?t"*/
	double t = 1.0 / 52.0;//Δt
	double u = exp(sig * sqrt(t));
	double d = 1.0 / u;
	double q = /*exp((r * t - d) / (u - d))*/
			  (exp(r * t) - d) / (u - d);
			//括号范围呐/(ㄒoㄒ)/~~ 
	double di = exp(-r * t);//利息贴现discounted interest
	
	int cir=pow(2,w)+ 1e-2;
	double** s;
	s = (double**)malloc((w + 1) * sizeof(double*));
	{ 
		int i;
		for (i = 0; i <= w; i++) {
			s[i] = (double*)malloc(cir * sizeof(double));
		}
	}
	//到期周数过大时可能会栈溢出，故动态分配内存
	//double s[w+1][cir] ;//= { 0 };//标价
	s[0][0] = s0;
	{
		int i, j;
		for (i = 1; i <= w; i++) {
			for (j = 0; j <= pow(2, i - 1)+ 1e-2 - 1; j++) {
				s[i][/*2j*/2*j] = s[i - 1][j] * u;//你再给我写2j试试 
				s[i][/*2j*/2*j + 1] = s[i - 1][j] * d;
			}
		}
	}

	//期权
	double** option;
	option = (double**)malloc((w + 1) * sizeof(double*));
	{
		int i;
		for (i = 0; i <= w; i++) {
			option[i] = (double*)malloc(cir * sizeof(double));
		}
	}
	//double /*k*/option[w+1][cir] ;//= { 0 };//不要变量名和数组名重复了 
	{
		int i;
		for (i = 0; i <= cir-1; i++) {
			/*k*/option[w][i] = calk(s[w][i]);
		}//计算终点价值
	}

	{
		int i, j;
		for (i = w-1; i >= 0; i--) {
			for (j = 0; j <= pow(2, i)+ 1e-2 - 1; j++) {
				/*k*/option[i][j] = q * /*k*/option[i + 1][/*2j*/2*j] + (1 - q) * /*k*/option[i + 1][/*2j*/2*j + 1];
				/*k*/option[i][j] *= di;
			}
		}
	}

	double kekyoku = /*k*/option[0][0];
	printf("%.4lf", kekyoku);
	{
		//释放内存
		int i = 0;
		for (i = 0; i <= w; i++) {
			free(s[i]);
			free(option[i]);
		}
		free(s);
		free(option);
	}
	
	return 0;
}

/*
死亡记录
代码中的错误点分析
1. 期权的最终价值计算错误 (概念性错误)

    位置: calk 函数

    问题: 你的函数写的是 ma(0, hatsunemiku - k)，也就是 max(0, S - K)。

    分析: 这是看涨期权（Call Option）的价值公式，代表“用约定价K买入股票的权利”。而李先生担心暴跌，他需要的是看跌期权（Put Option），即“用约定价K卖出股票的权利”。它的价值应该是“约定的卖出价”减去“当时的市场价”。

2. “风险中性概率 q” 的计算公式错误 (数学实现错误)

    位置: main 函数中的 q 计算行

    问题: double q = exp((r * t - d) / (u - d));

    分析: 你把 exp() 函数套在了整个表达式的外面。请再仔细看一下公式：q = (e^(r*?t) - d) / (u - d)。这里的 e^(...)，也就是 exp()，应该只作用于 r*t 这一小部分，而不是整个分数。

3. 变量名冲突与作用域问题 (最关键的程序逻辑错误)

这是导致你输出 3.5000 的最直接原因。

    位置: main 函数和全局变量声明

    问题:

        你在 main 函数的开头通过 scanf 读取了行权价 k。

        但是，随后你又定义了一个二维数组，也叫 k：double k[5][16] = { 0 };。在 main 函数的这个大括号内，k 这个名字从此以后就代表这个数组了，之前那个浮点数 k 被“覆盖”了。

        你的 calk 函数在计算时，使用的是它能访问到的、在函数外定义的全局浮点数 k。

        你在 scanf 的格式化字符串中，把 , 写了进去：scanf("%f,%f,%f,%f,%f", ...)。这意味着你的输入必须是 3.5,3.4,0.025,0.22,4 这种带逗号的格式。如果你输入的是用空格隔开的样例，那么从第二个变量 k 开始，所有变量都没有被正确读入！

        综合第3和第4点：scanf 读取 k 失败，导致全局浮点数 k 的值是0或者一个不确定的初始值。因此，你的 calk 函数实际上在计算 max(0, S - 0)，结果恒等于 S。

    后果: 最终导致在第4回合，你的“期权价值”数组 k[4] 里的值，其实就是股票价格 s[4] 的值。然后你一路把股票价格折现回来，结果当然就是当前的股价 s0。 
*/

/*
为什么需要动态内存分配？

想象一下，你要建一个房子（程序），需要用到很多砖块（内存）。你有两种方式获取砖块：

	静态/自动分配 (在栈上)：就像你家后院有一小堆固定的砖块。这堆砖块（栈内存）取用非常方便快捷，但它的大小在盖房子之前就定死了，而且容量很小。你程序里的 double s[5][16]; 就属于这种，大小在编译时就固定了。当你把 [5][16] 改成 [w+1][cir] 时，你告诉编译器：“我需要的砖块数量，在房子盖好之前（运行时）才能知道。” 编译器就懵了，因为它没法预留地方。更糟的是，当 w 很大时，你需要的砖块数量远远超过了后院那小堆的容量，导致“后院塌方”，也就是栈溢出。

	动态内存分配 (在堆上)：就像城市里有一个巨大的建材市场（堆内存）。这个市场非常庞大，你随时需要多少砖块，都可以去市场里申请。申请到的砖块会给你一个“提货单地址”（指针），你可以通过这个地址去使用它们。用完了，你必须负责把这些砖块还给市场，不然市场里的砖块会越来越少（内存泄漏）。

动态内存分配的方法论，就是一套“向市场借砖、用砖、还砖”的规范流程：

	借 (Ask)：在程序运行时，计算出你需要多少内存，然后使用 malloc 函数向“内存市场”（堆）提出申请。

	用 (Use)：市场会给你一个地址（指针），你通过这个指针来访问和使用你申请到的那块内存。

	还 (Return)：当你不再需要这块内存时，必须使用 free 函数，把这个地址的内存归还给市场，以便其他人或其他程序可以使用。

这个流程让你能够处理大小未知或尺寸巨大的数据，极大地增强了程序的灵活性和能力。
*/
