#include<stdio.h>//玛德好搞笑，我一开始写的是include<main.h>
#include<math.h>
#include<Windows.h> 
#include<stdlib.h>//Windows清屏指令用
#include<sysinfoapi.h>
#include<conio.h>
#include<mmsystem.h>//包含多媒体设备接口头文件
#pragma comment (lib,"winmm.lib")//加载静态库
//!!!C语言数组从0开始数!!!
//for里面三个语句用分号隔开!!! 
// ==才是比较，不是=！！！
//用VS studio会自动补全很多空格，所以可能有的地方有空格有的地方没有（一开始用devc敲的）
	//致几天前用完VS studio又润回devcpp的我：你有没有想过为何VS studio会在Hdskip和TlSkip上报错啊？！

//有个很严重的问题，C89不支持for循环内部定义变量，但我已经写了好多了...
//但是后来发现，我好多小函数里面前面for循环里面的变量还想在外面用结果都语法错误，所以也被迫改了说是...

int fd[5][2], s[10000][2], hd[1][2], rdtime, validrdtime;//蛇块位置、食物位置这几个必须用作全局变量，因为各个子函数里面也要用
DWORD begint;//游戏开始时间
char dir='R';//还是需要个初方向 

int HdSkip()
{
	int i=0;
	while (s[i][0]+s[i][1]==0)//这里不应该有分号孩子
	i++;//先跳过开头的空蛇块
	//一开始只有蛇头的时候s数组是空的！会出问题！！【怎么可能只有一开始】
	return i;
}
int TlSkip()
{
	int j=0;
	while (s[j][0]+s[j][1]==0)//这里也不应该有分号孩子
	j++;//先跳过开头的空蛇块
	
	do {
		j++;
	} while (s[j][0] + s[j][1] > 0);//再盘点中间的非空蛇块//但是这里确实要分号，因为这是dowhile不是while（
	return j;
	//为何不从后往前遍历？因为后面往往会剩很多空白蛇块，不如从前面更快 
}

int Check(int x,int y)//检查重合用 
{
	int i = HdSkip(), j = TlSkip(), k;
	for(k=i;k<j;k++)
	{//在非空蛇块中看看重不重复 
		if(
		(x-s[k][0]==0)&&(y-s[k][1]==0)
		)
		{
			return 0;
			break;
		}
	}
	if(k>=j) return 1;
//当条件1成立时继续循环，一旦不成立便输出0
//若一直成立则循环至条件2（不）成立时，再输出1 
//如果统计蛇长度的话就可以少一个j，甚至可以再多操作一下少个i，但是我觉得没差 
//中间绝对不可能有空蛇块（按理来说） 
 }

int Checkfd(int x, int y)//检查与食物重合用 
{
	int k;
	for (k = 0; k < 5; k++)
	{ 
		if (
			(x - fd[k][0] == 0) && (y - fd[k][1] == 0)
			)
		{
			return k;//用于后面重生被吃食物
			break;
		}
	}
	if (k >= 5) return 10;//前面输出0~4，所以这里只能避开，理论上取个其他值就行，这里就取个10算了
	//食物太少，没必要做什么算法简化 
	//记住，输出10就是完美避开，输出0~4就是没避开，还会告诉你和谁重合了
}
void perfectrand(int *a,int *b)
{
//反正这个函数就是要不与其他7788的东西冲突地随机生成坐标
	int x,y,conti=0;
	do {
		x=rand()%60+1;
		y=rand()%60+1;
		{
			if(
				(Check(x,y) == 0)||(Checkfd(x,y) != 10)//很奇怪？因为checkfd有改动~
			)
				{
				conti=1;
			}
		} 
	}
	while(conti==1);//==please
	/* 核心算法就是： 
	我不知道一次就能生成合格的，所以要反复尝试
	每次尝试后都要全面检查是否合格
	一旦合格就停止，否则继续*/
	*a=x;
	*b=y;//指针用于修改输入的变量，记得调用的时候用&	
}

//令人感动，C89库里面连比较大小的函数都没有，甚至math库里面也一样
//如果不自己动手丰衣足食的话，那再检验蛇头是否天元突破(?)冲出地图的时候会很冗长
int ma(int a, int b) {
	return (a > b) ? a : b;
}
int mi(int a, int b) {
	return (a < b) ? a : b;
}

void input()
{// 简化逻辑：只要不反向就行，我本来还写的是“如果反向就直接break，否则赋方向了再break”
//有个很大的问题：如果我们一直直接改变真正的dir的话，“反向不合法”的判据会变成新输入的方向，出大问题！！！所以要用个临时变量存方向
	/*begint = GetTickCount();//重置回合起始时间 */
	int tempodir = dir;//如果没输入的话tempodir会是空的！！！那后面赋值就不好赋了
	/*DWORD limt = begint + validrdtime;
	while (GetTickCount() < limt) { */
		if (_kbhit())
		{
			char in = getch();
			switch (in) {
			case 'w':
				if (dir != 'D')
					tempodir = 'U';
				break;
				//玛德我到底要多少次才能记住if后面执行语句块要打{}
					//哎这里但是不用打啊，break是公用的（
			case 's':
				if (dir != 'U')
					tempodir = 'D';
				break;

			case 'a':
				if (dir != 'R')
					tempodir = 'L';
				break;

			case 'd':
				if (dir != 'L')
					tempodir = 'R';
				break;
			}
		}/*
		Sleep(10);//适当的休息cpu
	}*/
	
	
	dir = tempodir;
}
	//读取输入上下左右，可以做到“输入多次只取最后一次"（一直循环）
		//但是上面注释掉了检测时间的部分，因为这样输入有bug，实际上控制不了 
	
void output()
{
	switch (dir) {
	case 'U':hd[0][1] -= 1; break;
	case 'D':hd[0][1] += 1; break;//符合C语言坐标系y轴向下 
	case 'R':hd[0][0] += 1; break;
	case 'L':hd[0][0] -= 1; break;
	}
}

void gotoxy(int x, int y) {//移动光标 
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {x, y};
    SetConsoleCursorPosition(hConsole, pos);
}

void hide() {//隐藏光标闪烁 
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void begindraw(){
	int i;
	for(i=0;i<62;i++){//第0行为上边框，1~60为图形，61为下边框；上下边框要在0~61列 
		gotoxy(i,0); printf("#");
		gotoxy(i,61); printf("#");	
	}
	
	for(i=1;i<61;i++){//列类似，但是只需要1~60行（防止四角重复） 
		gotoxy(0,i); printf("|");
		gotoxy(61,i); printf("|");
	}
	
	gotoxy(hd[0][0],hd[0][1]); printf("@");//蛇头 
	
	for(i=0;i<5;i++){
		int x=fd[i][0],y=fd[i][1]; 
		gotoxy(x,y); printf("$");//break;//食物 //你才前面为什么注释了个break，死亡回放说是 
	}
}


void deletedraw(int x,int y){
	gotoxy(x,y); printf(" ");//清除 
} 

void diydraw(int x,int y,char z) {
	gotoxy(x,y);
	switch(z){
		case 's':printf("*");break;//蛇块 
		case 'f':printf("$");break;//食物 
		case 'h':printf("@");break;//蛇头
	}/*为什么要叫diydraw?就是你想画什么就给你画什么 
	你说得对但是diy...どぅー いっと ゆあせるふ！！...是... 
	*/
}

int main()
{
	printf("欢迎进入游戏『贪吃蛇』！\n按回车键进入难度设置选项~\n\n本游戏有BGM，请开启声音\n建议切换到英文输入法\n按Ctrl+滑动滚轮可以缩放页面字体大小");//转义字符是\不是/
	getchar();
	system("cls");//Windows清屏
	
	int difficulty,beginrdtime;
	double param;//缩放时间参数 
	printf("本游戏主要规则：WASD控制&不能反向&不能撞墙&不能撞自己\n\n本游戏有三大难度：简单、适中、困难\n难度越高，游戏速度越快\n据说最高难度有什么「奇异」等着勇于挑战的玩家呢！\n\n输入1/2/3对应简单、适中、困难\n");
	scanf("%d",&difficulty);
	
	system("cls");//Windows清屏
	switch(difficulty){
		case 1:beginrdtime = 400, param = 0.15;printf("你选择了简单难度！努力练习上手吧  (∠> ω< )⌒☆");break;
		case 2:beginrdtime = 300, param = 0.25;printf("你选择了适中难度！不错哦 ( 'ω' )/");break;
		case 3:beginrdtime = 200, param = 0.4;printf("你选择了困难难度！这就是高手的实力吗...(シ_ _)シ");break;
		default:beginrdtime = 300, param = 0.25;printf("你...按错键了，惩罚你去困难（划掉）适中难度罢 (￣^￣)べ");break;
	}
	rdtime=beginrdtime;
	printf("\n7s后开始游戏...准备好！");
	PlaySound("sound1.wav", NULL, SND_FILENAME | SND_ASYNC);
	Sleep(7000); 
	
	hide();
	begint = GetTickCount();//开始记录时间
	system("cls");//Windows清屏
	
	//先暂定地图为60*60吧 ，坐标从1开始到60 
	//先把回合数、回合时间、蛇头位置定义了吧
	int rdnum = 1, wasted = 0, score = 0, slength = 1, lucky = 0,/*触发隐藏结局用*/zaggocount=0/*求之不得机制*/;//疑似不需要wasted这个参数，直接break就行了？param是游戏时间缩放参数，未来可设置难度选项(这个不要也顺手int了口牙)
	
	//int firstrow = 1,/*首轮太特殊，只能这样操作了，写个修正什么的太麻烦*/
	/*好的，大型出错现场。我当时给首轮打补丁是因为我发现首轮只有蛇头没有蛇身，所以s数组是空的，Tlskip会直接越界，导致后续的崩溃
	但是谁告诉你只有首轮才是这样？！在没吃到食物之前永远会是这样的！！！所以越界还是会发生despite打了补丁*/
	//现在要特殊对待的是单蛇节情况！
		//不用再定义变量了！善用蛇长！！！
	
	hd[0][0]=rand()%60+1;//head为何是个二维数组是个历史遗留问题，不要在意（
	hd[0][1]=rand()%60+1;
	//下面随机生成食物
	/*临时变量都记作i
	鉴于若要求使用C89编译器的话，for变量要在外部声明，故将for循环先都各自括起来，否则太多临时变量冲突不太好*/
	{
		int i;
		for (i = 0; i < 5; i++)
		{
			perfectrand(&fd[i][0], &fd[i][1]);
		}
	}
	
	begindraw();//绘制初始地图 
	
	switch(difficulty){
		case 1:PlaySound("diy_gentle.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);break;
		case 2:PlaySound("diy.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);break;
		case 3:PlaySound("sf.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);break;
		default:PlaySound("diy.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);break;
	}

	while (wasted==0)//在游戏没结束的时候持续进行以下游戏回合循环 
	{
		/*由于这个循环一开始就把旧蛇头扬了，
		所以必须一开始做旧蛇头の大清洗工作（就是替换显示
		顺便盘一下新旧蛇头交替机理
		1.回合开始时旧蛇头替换显示 【前提是tmd这条蛇不是只有一块】！！！ [事实上问题不在这里...]
		2.按输入方向修改蛇头坐标，相当于一开始就旧头无了 
		3.判定结束后输出蛇头显示
		4.将蛇头坐标存到蛇身中，准备下一轮大清洗 */
		
		//gotoxy(0,63); printf("%d,%d;",HdSkip(),TlSkip()); //调试用 
		if (slength==1){
			deletedraw(hd[0][0], hd[0][1]);
		}
		else diydraw(hd[0][0], hd[0][1],'s');//旧蛇头替换显示为蛇身 

		input(); output();
		//按（上个ifelse存下来的）方向移动，新蛇块位置存入hd
		//【但是先别存到s里面！！！问就是check函数不想再写一遍了】
		//几天之后的我：...你知道一个一开始空的s数组会导致HdSkipTlSkip直接跑到1000吗，后果不堪设想啊 
		
		if (Checkfd(hd[0][0], hd[0][1]) != 10) /*蛇头坐标和食物重合*/
		{
			int eaten = Checkfd(hd[0][0], hd[0][1]);
			perfectrand(&fd[eaten][0], &fd[eaten][1]);
			diydraw(fd[eaten][0], fd[eaten][1],'f');//显示新食物 
			
			// 改进的得分逻辑：基础分 + 长度奖励 + 速度奖励
    		int baseScore = 100;
    		int lengthBonus = slength * 2;    // 越长奖励越多
    		int speedBonus = (beginrdtime - rdtime) / 10; // 速度越快奖励越多
    		score += baseScore + lengthBonus + speedBonus;
			
			slength++;//还是直接慢慢加更好，别老是用skip
			zaggocount=0;
		}//再生成一单位食物 

		else {
			int i;
			if (slength == 1) {
				i = 0; 
			}
			else i = HdSkip();
			deletedraw(s[i][0],s[i][1]);//消除旧蛇块 【但是，如果你只有一块，那么此刻你还没把它存到蛇块里，那么不就删不了了...不过实际上这个数据必定在前面被覆盖了，所以...】 
			s[i][0] = 0, s[i][1] = 0;
		}
		//如果不是单节蛇，则从s0开始，遍历所有蛇块坐标，若不是(0,0)则将其归零
		//若为单节蛇，就不用遍历，直接杀s0
			//归零了得让他不显示啊qwq（update:现在没这个问题了，感谢hdskip和tlskip 

		if (
			ma(hd[0][0], hd[0][1]) > 60
			||/*蛇头坐标超出地图范围，即＜1或＞60*/
			mi(hd[0][0], hd[0][1]) < 1
			)
			break;//跳出游戏循环，直接失败 

		if(Check(hd[0][0],hd[0][1])==0) /*蛇头坐标与任意（除了头以外）的蛇块重合*/
			break;//跳出游戏循环，直接失败 
		else {++rdnum; ++score;}//回合数加一，成功活过所有死亡判定，おめでとう
		
		//求之不得机制 
		{
			int i,zaggo,temzaggocount=0,whichzaggo;
			for(i=0;i<=4;i++){
				zaggo=abs((fd[i][0]-hd[0][0]))+abs((fd[i][1]-hd[0][1]));
				if(zaggo<=9){
					temzaggocount++;
					whichzaggo=i;
				}
			}
			if (temzaggocount>0){
				zaggocount++;
				temzaggocount=0;
			}
			if(zaggocount>=60){
				zaggocount=0;
				score/=1.4;
				deletedraw(fd[whichzaggo][0],fd[whichzaggo][1]);
				perfectrand(&fd[whichzaggo][0],&fd[whichzaggo][1]);
				diydraw(fd[whichzaggo][0],fd[whichzaggo][1],'f');
				gotoxy(0,63);printf("当小蛇蛇围着食物转圈圈超过60回合时——\n食物君会气鼓鼓地跺脚：「扭来扭去的笨蛋！不理你啦！ヽ(≧Д≦)ノ」\n噗咻~一声消失不见~\n（本机制专治各种强迫症绕圈，请控制好手速，抢占最佳抢食时机！否则...会扣分哦~）   \n上次「求之不得」发生在%d回合        ",rdnum);
			}
		}
		

		
		//存蛇头入蛇块
		{
			int i;
			if (slength == 1) {
				i = 0;
				//单节蛇特供
				/*值得指出的是，这里似乎处理得显得多余，理论上可以不删不增，直接移动蛇头hd，但是实际上需要打很多的if补丁
				原因很简单，核心算法发生了变化，所以更科学的做法就只是【规避hdskiptlskip，直接上0】*/
			}
			else i = TlSkip();
			s[i][0] = hd[0][0];
			s[i][1] = hd[0][1];
			/*新蛇头堂堂登场*/
			diydraw(hd[0][0], hd[0][1],'h');
		}

		do
		{
			Sleep(3);
		} while (GetTickCount()-begint<rdtime);//检测时间有没有到


		//以下为成绩展示
		gotoxy(0,62);
		printf("回合: %d | 长度: %d | 分数: %d | 使用WASD控制 | 蛇头:@ 蛇身:* 食物:$                         ",rdnum,slength,score);
		//调试部分完 

		begint = GetTickCount();//重置回合起始时间

		rdtime = max(beginrdtime / (1 + log(1 + rdnum * param)), 50);//加快rdtime,采用ds建议的平方根缩放，50ms(20fps)为底线
		//完了します~ 	

		lucky= rand() % 100;
		if (rdnum>=1500&&lucky>=98&&slength>=15&&score>=3000){
			PlaySound(NULL, NULL, 0);
			system("cls");
			printf("咦？游戏似乎不见了，但是我们好像发现了制作组留下来的...一点...文字？\n");
			Sleep(2000);
			break;
		}
	}
	PlaySound(NULL, NULL, 0);
	if (lucky < 98) {
		system("cls");
		PlaySound(TEXT("sound2.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	else {
		PlaySound(TEXT("gentle.wav"), NULL, SND_FILENAME | SND_ASYNC);
		showHiddenEnding();
	}
	printf("你活过了%d个回合，获得了%d分！",rdnum,score);	
	//添加一些根据活过回合数多少更改的暖心提示说是 
	if(score < 1000) printf("刚刚起步，新手小蛇~\n");
	else if(score < 2000) printf("有点意思，熟练玩家！\n"); 
	else if(score < 3000) printf("不错哦，蛇の大师！\n");
	else printf("orz_终极蛇王！\n");
	printf("按ESC键退出游戏");
	
	{
		char key;
		do { 
        	key = _getch();  // 获取按键，不显示在屏幕上
    	} while(key != 27);  // 27是ESC键的ASCII码
	}
    
    return 0;
}


void typewriterPrint(const char* text, int delayMs) {
    int length = strlen(text);
    for (int i = 0; i < length; i++) {
        putchar(text[i]);
        fflush(stdout);
        Sleep(delayMs);
        
        // 在标点符号处稍微停顿久一点
        if (text[i] == '。' || text[i] == '！' || text[i] == '？' || text[i] == '\n') {
            Sleep(delayMs * 5);
        }
    }
}

void showHiddenEnding() {
    system("cls");
    
    // 设置文字颜色为淡绿色，模拟老式打印机的效果
    system("color 0A");
    
    printf("\n");
    
        // 第一部分：恭喜信息
    typewriterPrint("恭喜你触发隐藏结局！\n\n", 80);
    Sleep(800);
    
    typewriterPrint("亲爱的玩家，当这行文字缓缓浮现时，你已经在贪吃蛇的世界里度过了上千个回合。\n", 60);
    typewriterPrint("那些在方寸屏幕间游走的日夜，那些不断刷新记录的瞬间，我们都默默见证着。\n\n", 60);
    Sleep(500);
    
    // 第二部分：回忆与感悟
    typewriterPrint("还记得第一次控制小蛇吃下果实时的欣喜吗？\n", 70);
    typewriterPrint("还记得无数次撞墙后重来的坚持吗？\n", 70);
    typewriterPrint("这不仅仅是一个游戏，这是你与代码世界的第一次亲密对话。\n\n", 65);
    Sleep(600);
    
    typewriterPrint("在那些像素点的移动中，我们看到了你的耐心；\n", 70);
    typewriterPrint("在那些不断优化的策略里，我们看到了你的智慧；\n", 70);
    typewriterPrint("在那些永不言弃的重开中，我们看到了你的坚韧。\n\n", 65);
    Sleep(700);
    
    // 第三部分：转折与启发
    typewriterPrint("但是，亲爱的朋友，是时候看看更广阔的天空了。\n", 75);
    typewriterPrint("这条小小的贪吃蛇，只是计算机科学世界中的一粒微尘。\n\n", 60);
    Sleep(500);
    
    typewriterPrint("想象一下：\n", 100);
    typewriterPrint("你能用算法让千万人在社交网络中相遇...\n", 70);
    typewriterPrint("你能用代码让机器理解人类的语言...\n", 70);
    typewriterPrint("你能用程序守护网络世界的安全...\n", 70);
    typewriterPrint("你能用技术改变无数人的生活...\n\n", 65);
    Sleep(800);
    
    // 第四部分：鼓励与召唤
    typewriterPrint("这条贪吃蛇，是你编程之路的起点。\n", 70);
    typewriterPrint("而前方，是无限可能的星辰大海。\n\n", 65);
    Sleep(500);
    
    typewriterPrint("计算机科学的世界里：\n", 80);
    typewriterPrint("有让人惊叹的人工智能，\n", 70);
    typewriterPrint("有精妙绝伦的数据结构，\n", 70);
    typewriterPrint("有深不可测的算法奥秘，\n", 70);
    typewriterPrint("有连接全球的网络奇迹。\n\n", 65);
    Sleep(700);
    
    // 第五部分：告别与祝福
    typewriterPrint("所以，请带上我们制作人员最真挚的热忱与祝福，\n", 60);
    typewriterPrint("更积极、更勇敢地投入到计算机科学的事业中吧！\n\n", 55);
    Sleep(600);
    
    typewriterPrint("这条小蛇会记得你指尖的温度，\n", 70);
    typewriterPrint("这个粗糙的游戏会珍藏你的成长足迹。\n", 65);
    typewriterPrint("而现在，是时候开启新的征程了。\n\n", 60);
    Sleep(800);
    
    // 第六部分：更深层的思考
    typewriterPrint("每一个伟大的程序，都始于最简单的\"Hello World\"。\n", 65);
    typewriterPrint("每一次技术的突破，都源于对现状的不满足。\n", 65);
    typewriterPrint("你在这条小蛇身上花费的时间，让你变得如此珍贵。\n\n", 60);
    Sleep(700);
    
    typewriterPrint("就像小蛇不断成长，我们也在不断进步：\n", 70);
    typewriterPrint("从理解变量到掌握指针，\n", 80);
    typewriterPrint("从编写循环到设计架构，\n", 80);
    typewriterPrint("从解决问题到创造价值。\n\n", 75);
    Sleep(800);
    
    // 最终寄语
    typewriterPrint("伟大的计算机科学与技术，\n", 80);
    typewriterPrint("远不止这方寸之间的贪吃小蛇。\n", 70);
    typewriterPrint("它等待着你去探索、去创造、去改变！\n\n", 60);
    Sleep(600);
    
    typewriterPrint("愿你在这条路上：\n", 100);
    typewriterPrint("保持好奇，保持热爱，\n", 90);
    typewriterPrint("永远年轻，永远热泪盈眶。\n\n", 80);
    Sleep(1000);
    
    // 签名和表情
    typewriterPrint("(·W ·)/\n\n", 200);
    
    // 等待一下，然后开始鸣谢部分
    Sleep(2000);
    
    // 鸣谢部分 - 改变颜色为青色以区分
    system("color 0B");
    
    typewriterPrint("════════════════════════════════════════\n", 40);
    typewriterPrint("                项目鸣谢\n", 80);
    typewriterPrint("════════════════════════════════════════\n\n", 40);
    Sleep(500);
    
    typewriterPrint("编写团队：\n", 100);
    typewriterPrint("  程天喆（计算机2504）\n", 80);
    typewriterPrint("  白烨（计算机2504）\n\n", 80);
    Sleep(600);
    
    typewriterPrint("技术指导与参考资料：\n", 90);
    typewriterPrint("  Bing 搜索引擎\n", 70);
    typewriterPrint("  PingCode 项目协作平台\n", 70);
    typewriterPrint("  CSDN 技术社区\n", 70);
    typewriterPrint("  知乎 专栏\n", 70);
    typewriterPrint("  以及所有开源社区的贡献者们\n\n", 65);
    Sleep(700);
    
    typewriterPrint("开发工具与环境：\n", 90);
    typewriterPrint("  Deepseek - 智能代码助手\n", 70);
    typewriterPrint("  Google AI Studio - AI开发平台\n", 70);
    typewriterPrint("  ProcessOn - 专业流程图设计\n", 70);
    typewriterPrint("  DevC++ 5.11 - 轻量级开发环境\n", 70);
    typewriterPrint("  Microsoft Visual Studio 2022 - 专业IDE\n", 70);
    typewriterPrint("  Microsoft Word - 文档编写\n\n", 70);
    Sleep(800);
    
    typewriterPrint("背景音乐 (BGM)：\n", 90);
    typewriterPrint("  《機動戦士ガンダム GQuuuuuuX》动画OST\n", 70);
    typewriterPrint("     コロニーの彼女 (I_006A) - 照井順政\n\n", 65);
    
    typewriterPrint("  《Do It Yourself!! ‐どぅー.いっと.ゆあせるふ‐》动画OST\n", 65);
    typewriterPrint("     どぅー.いっと.ゆあせるふ - 佐高陵平(y0c1e)\n", 60);
    typewriterPrint("     どぅー.いっと.ゆあせるふ (しっとり) - 佐高陵平(y0c1e)\n", 60);
    typewriterPrint("     アイキャッチ.A&B - 佐高陵平(y0c1e)\n", 60);
    typewriterPrint("     変わらないもの - 佐高陵平(y0c1e)\n\n", 60);
    Sleep(1000);
    
    typewriterPrint("特别感谢：\n", 100);
    typewriterPrint("  那个单节蛇时总是出bug的[s]数组和skip函数\n", 80);
    typewriterPrint("  大显神通无所不能的Windows API函数\n", 80);
    typewriterPrint("  提供了莫大情绪价值（和bgm）的《少女手工》动画\n", 80);
    typewriterPrint("  以及每一位，坚持玩到这里的你口牙！\n\n", 75);
    Sleep(800);
    
    typewriterPrint("这个项目不仅仅是一次编程练习，\n", 70);
    typewriterPrint("更是我们学习旅程的见证。\n", 70);
    typewriterPrint("感谢技术，让我们相遇；\n", 70);
    typewriterPrint("感谢代码，让我们创造。\n\n", 70);
    Sleep(700);
    
    typewriterPrint("════════════════════════════════════════════════\n", 40);
    typewriterPrint("        感谢游玩，期待在代码的海洋中再次相遇！\n", 60);
    typewriterPrint("════════════════════════════════════════════════\n\n", 40);
    
    // 最终停顿
    Sleep(5000);
    
    // 恢复默认颜色
    system("color 07");
}
