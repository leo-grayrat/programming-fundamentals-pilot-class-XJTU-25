#define _CRT_SECURE_NO_WARNINGS
    //防止vs警告某些旧的函数，如strtok,sscanf

//【重要申】需要修改head的函数（ins, del, rls等）应该传递&head，而只读的函数（src, printList）应该传递head

//要让整个项目统一使用ANSI (多字节字符集)
#include <stdio.h>
#include <string.h>//字符串相关
#include <math.h>//相信用得到数学计算
#include <time.h>//命名需要日期时间
#include <Windows.h>//WindowsAPI
#include <stdlib.h>
#include <ctype.h>//isdigit用
#include <conio.h>//智慧的输入 getch,kbhit
                    //微软：不你不智慧，给我用_kbhit
//#include <stdbool.h>
    //我又要🐎C89了，我之前都疑惑过C语言有没有那种逻辑变量，结果就是那个布尔值
    //但是C99才有...还是int吧孩子们

//#define _CRT_SECURE_NO_WARNINGS   不能放在include之后
    //防止vs警告某些旧的函数，如strtok,sscanf

double maxi(double a, double b) {
    double res = a > b ? a : b;
    return res;
}           //DEBUG1: maxmin函数撞名

double mini(double a, double b) {
    double res = a < b ? a : b;
    return res;
}

int bet(double kanojou, double minimum, double maximum) {
    int res = (kanojou > (minimum - 1e-6)) * (kanojou <= (maximum + 1e-6));
    return res;
    //简化区间类型的条件，每次 a >= min && a <= max 太麻烦
        //二编：为何结果是个int?因为这个函数是输出逻辑值的
        //但是考虑到我们需要的是闭区间而且类型转换有误差，所以用1e-6修正下
            //三编：如果是个int，要想闭区间，而下限在-1e-6之后取整会-1，所以不能在左边打=
}
//编写的时候用高精度类型没坏处

typedef struct envdata{
    int id;                 // 监测点ID
    char loca[100];         // 监测点位置
    float temp;             // 温度, celcius
    float humi;             // 湿度, %
    float pm25;             // PM2.5浓度,取一位小数,单位为µg/m^3
    float wdspd;            // 风速 (单位: m/s)
    int wddir;              // 风向 (单位: 度, 默认向北，顺时针旋转0-360°)
    char /*date[11]*/date[12];//errorchange: 在最后添加数据时，如果11字符截断的话，\n会留在缓冲区里面...          // 日期（格式：YYYY-MM-DD）
    int pollv;              // 污染等级 (1=优, 2=良, 3=轻度污染, 4=中度污染, 5=重度污染)
    struct envdata* next;   // 指向下一个节点的指针
    //如果知道了所有污染物的浓度，就可以根据国家发布的《环境空气质量指数（AQI）技术规定》
    //用一个固定的数学公式，直接计算出IAQI（单项空气质量指数），然后取最大值得到AQI，
    //最后根据AQI的范围确定污染等级。这个过程是确定的、纯计算的，根本不需要什么机器学习来‘预测’或‘聚类’
    //但是这样就无法达到深度学习/聚类分析的效果了，变成个计算器了说是
    //所以这里不能加（虽然真实气象站肯定会采集到的）其他信息
    //btw，虽然任务1中没有说风速风向，但是后续聚类分析任务中提到了，故加上之
}ed;

//ed* ins(envdata** head) {//如果不想搞全局变量の灾难的话，就老老实实传指针      //DEBUG3:如果不用简称，那么就要用struct+name，【你以为简称只是用来简写的吗】
ed* ins(ed **head) {//如果不想搞全局变量の灾难的话，就老老实实传指针    
    //插入节点
    
    //到底二级指针是要搞什么？下文解释
    //变量	含义	                    类型	                    传递给函数
    //A	    节点实体	                EnvData	                -
    //B	    指向节点的指针(head)	    EnvData* (一级指针)	    ins(head) -> 函数得到 B的副本
    //C	    指向指针的指针(&head)  	EnvData** (二级指针)	    ins(&head) -> 函数得到 B的地址
    //为了在函数内部永久性地修改 main 函数里那个变量 B 的值
    //即修改 【head 指向的位置】（核心！）
    //你必须将 B 的地址（C） 传进去

    //值得注意的是，没有必要把这个函数弄成插入数据
    //因为可以用插入节点+修改数据组合而成
    ed* newed = (ed*)malloc(sizeof(ed));
    if (newed == NULL) {
        printf("内存已满qwq...");
        return NULL;//malloc是会失败的孩子们
    }
    newed->next = NULL;//不是说放到最后，是说创建一个和其他节点无关的新の节点
    //a->b就是访问(*a).b
    //那么有人要问了，主播主播为何会是个指针呢？
    //malloc 本身就返回一个 void* 类型的指针。所以，接收它返回值的必须是一个指针变量
    //那为什么要这么设计呢？
    //生命周期可控：通过 malloc 在堆上创建的数据，它的生命周期与创建它的那个函数无关。
        //只要你不调用 free()，它就永远存在，直到整个程序结束。
    //全局可访问：因为堆上的数据不会被自动销毁，所以我们可以在一个函数里用malloc创建它，
        //然后把它的地址（指针）传递给其他函数，甚至存储在全局变量（如head）中，在程序的任何地方都可以安全地访问它。
    //链表的节点不能随着函数的结束而消失（栈式），必须按需存在和销毁（堆式）

    //下面是插入链表节点
    //头插法最为便捷，只用改自己和head的next
    //但是...有个事情还是跟其他节点相关...アイデイです，ID得顺延
    //int i=1;//泪目了，回来吧我的int，指针什么的都衮罢（？
        //孩子，指针怎么可能滚呢，你以为你还在遍历数组吗
    //ed* i = &head;//errorchange
    ed* i = *head;//你上面是ed***你知道吗...，*是解指针运算（在运算的时候）errorchange
    int j = 0;//但是id还是要int的~
    while (i != NULL) {
        j = maxi((i->id), j);
        //i = i > -next;你踏马在写什么
        i = i->next;
    }
    //我一开始以为“不能向数组一样有个临时变量去存储那个东西（ed* i = head;）”，但事实上是可以的
        //那是啥呢？为何我记忆中有一种东西不能这么弄呢？
            //数组不能int copy[len]=already[len]

    newed->id = ++j;//id确定
    newed->next = *head;//*head是原来链表的头部
    //此时newNode ---> [Node A] ---> [Node B] ---> NULL ！新节点已经指向了旧链表
    //head ---> [Node A]  ！head 指针还没变
    *head = newed;//更新我们总台账（main函数里的head指针），宣布新的链表头部现在是 newed

        //二编：你返回值呢？这样怎么跟其他函数衔接？
        return newed;
        //注意，返回的是ed*指针
}

//见src函数中的解释
//由于编译从前向后读，故把这些放前面（理论上放最前是最好的，但是此处还是为了解释编码思路历程
#define src_id          (1 << 0)  
#define src_loca        (1 << 1)  
#define src_temp        (1 << 2)
#define src_humi        (1 << 3)
#define src_pm25        (1 << 4)
#define src_wdspd       (1 << 5)
#define src_wddir       (1 << 6)
#define src_date        (1 << 7)
#define src_pollv       (1 << 8)

//搜索得另外弄个结构体，因为参数变多了
//A表示下界，B表示下界
typedef struct envdatasrc {
    int idA;
    int idB;                // 监测点ID
    char loca[100];         // 监测点位置
    float tempA;
    float tempB;            // 温度, celcius
    float humiA;
    float humiB;    // 湿度, %
    float pm25A;
    float pm25B;    // PM2.5浓度,取一位小数
    float wdspdA;
    float wdspdB;   // 风速 (单位: m/s)
    int wddir;              // 风向
        //这个之后用东南西北判断，不需要人为给定上下界
        // 0=北, 1=东北, 2=东, 3=东南, 4=南, 5=西南, 6=西, 7=西北
    char /*dateA[11]*/dateA[12];//errorchange: 在最后添加数据时，如果11字符截断的话，\n会留在缓冲区里面...
    //char dateB[11]// 日期（格式：YYYY-MM-DD）      //DEBUG2:神秘消失的; 疑似复制少了
    char /*dateB[11]*/dateB[12];//errorchange: 在最后添加数据时，如果11字符截断的话，\n会留在缓冲区里面...
    int pollv;              // 污染等级 (1=优, 2=良, 3=轻度污染, 4=中度污染, 5=重度污染)
    //不需要指向下一个节点的指针了
}eds;

void rls(ed** head);//先声明...

ed* src(const /*envdatasrc*/eds* bing, ed* head, unsigned int flag) {/*我说必应是最好搜索引擎，谁反对*/
    //查询数据

    //显然，为各种不同数据的查询写多个子函数很头大，那么
    //C语言里有没有一种“万能盒子”或者“通用指针”，可以用来传递任意类型的数据？
    //孩子们通用指针来力,void*しゅきしゅきだいしゅき
    //char which,void *bing，然后switchcase即可
        //但是呢，事情没有这么简单
        //1.有些搜索要输入一个值（查询名字/id），有些却需要区间（各种准确数据）
        //2.如果我不只根据一个条件筛选呢？多重条件怎么办？
        //这时就要请出可变参数の函数了！最简单的例子就是printf和scanf...
            //哎哎停停停，真的是这样吗
            //调用这个函数的人会很困惑，search(head, "ID", 101, NULL, 0.0, 0.0)，
            //后面这一串NULL和0.0是什么？接口暴露了太多它内部的实现细节，而且很难直观理解
            //以及，va_arg 就像一个盲人，你告诉他往前走4步（int的大小）拿东西，他就绝不会走8步（double的大小）
            //类型传错了，后果是灾难性的，而且编译器一声都不会吭，如printf("%d",1.14)
            //以及，函数体积庞大，逻辑交织，这不是什么好事
            //针对不同数据类型的switch判断和if比较逻辑，是这个功能固有的复杂度，
            //不会因为你用了子函数、传递了结构体或者使用了可变参数而消失
            //但是coder的目标，往往不是消除复杂度，而是有效地【组织和隔离复杂度】
            //就是所谓“对扩展开放，对修改封闭”
            //用个简单的母函数写出基本逻辑，再用子函数/结构体封装处理每个类型分别的逻辑
                //但是，值得注意的是，如果我们有多重条件，就没办法switchcase了 
                //进一步地，如果我们既要支持与逻辑还要支持或逻辑，那就更头大了
                //而且此时又牵扯到了输入函数的变量要机动变化的问题
                //进一步地，即使处理好归一化的输入，一次性输入至少9个变量实在是臃肿...
                    //位运算 一个unsigned int数搞定各种是非标识！
                    //实际上是宏，编译时替换define的指令
                        //由于时间限制和全局考虑，我们放弃兼容或运算

    //下面看起来很像ai写的，没错就是啊
        //但是这不是偷懒，不同条件下的搜索条件已经规划好了，位运算和布尔值判断还有结构体输入以及结构体遍历这些真算法都想好了
        //剩下就是重复工作，既然这样何乐而不为？
            //但是，c89没有布尔值...
    //此外，需要传入head以便遍历

    ed* head_src = NULL;
    //现在有个小问题，这个函数（排序那个）除了要在显示全部时调用，还需要在搜索时调用，
    //此时能否通过把搜索结果存在一个另外的ed链表里面（由head_src开始）来达成这一目的？
    //当然可以~
        
    // 检查是否有传入搜索条件，如果没有则直接返回
    if (flag == 0 || bing == NULL) {
        printf("您并没有键入搜索条件呢(´⊙ω⊙`)\n");
        return NULL;//errorchange，必须返回值
    }

    // 检查链表是否为空
    if (head == NULL) {
        printf("哎呀...请先录入数据ρ(・ω・、)\n");
        return NULL;//errorchange，必须返回值
    }

    printf("\n--- 好的，开始搜索啦！( • ̀ω•́ ) ---\n");

    ed* current = head;
    int found_count = 0;
    int is_match;//本来是布尔值

    // 开始遍历链表
    while (current != NULL) {
        is_match = 1; // 先假设当前节点是匹配的

        // --- 使用位掩码，逐一检查所有被激活的条件 ---

        // 1. 检查ID范围
        if (flag & src_id) {
            if (!bet(current->id, bing->idA, bing->idB)) {
                is_match = 0; // 不匹配，一票否决
            }
        }

        // 2. 检查位置（模糊匹配）
        if (is_match && (flag & src_loca)) {
            // strstr 返回NULL表示未找到子字符串
            if (strstr(current->loca, bing->loca) == NULL) {
                is_match = 0;
            }
        }

        // 3. 检查温度范围
        if (is_match && (flag & src_temp)) {
            if (!bet(current->temp, bing->tempA, bing->tempB)) {
                is_match = 0;
            }
        }

        // 4. 检查湿度范围
        if (is_match && (flag & src_humi)) {
            if (!bet(current->humi, bing->humiA, bing->humiB)) {
                is_match = 0;
            }
        }

        // 5. 检查PM2.5范围
        if (is_match && (flag & src_pm25)) {
            if (!bet(current->pm25, bing->pm25A, bing->pm25B)) {
                is_match = 0;
            }
        }

        // 6. 检查风速范围
        if (is_match && (flag & src_wdspd)) {
            if (!bet(current->wdspd, bing->wdspdA, bing->wdspdB)) {
                is_match = 0;
            }
        }

        // 7. 检查风向（按8个方向匹配）
        if (is_match && (flag & src_wddir)) {
            // 将节点的风向角度和搜索的风向角度都转换为0-7的类别
                    //DEBUG4: 手敲的恶果，wddir写成dir了
            /*float dir = ((current->dir) / 22.5 + 1) / 2;
            */
            float dir = ((current->wddir) / 22.5 + 1) / 2;
            int dirchanged = mini((int)dir, 7);//将方向归一化为数字0~8，但是8和0都是北，所以处理一下
            if (dirchanged != (bing->wddir)) {
                is_match = 0;
            }
        }

        // 8. 检查日期（精确匹配）
            //并非，改为区间匹配
        if (is_match && (flag & src_date)) {
            // strcmp 返回0表示字符串相等
                //由于日期排列就是字典序，所以也可以靠这个比较大小
            if (strcmp(current->date, bing->dateA) < 0||strcmp(current->date, bing->dateB) > 0) {
                is_match = 0;
            }
        }

        // 9. 检查污染等级（精确匹配）
        if (is_match && (flag & src_pollv)) {
            if (current->pollv != bing->pollv) {
                is_match = 0;
            }
        }

        // --- 所有检查结束 ---

        // 如果经过所有已激活条件的检查后，is_match仍然为true，则打印该节点信息
        if (is_match) {
            found_count++;
            /*
            printf("----------------------------------------\n");
            printf("ID: %d\n", current->id);
            printf("  位置: %s\n", current->loca);
            printf("  日期: %s\n", current->date);
            printf("  温度: %.1f C\n", current->temp);
            printf("  湿度: %.1f %%\n", current->humi);//输出%要%两次
            printf("  PM2.5: %.1f\n", current->pm25);
            printf("  风速: %.1f m/s\n", current->wdspd);
            printf("  风向: %d°\n", current->wddir);
            printf("  污染等级: %d\n", current->pollv);
            */
            //得排序孩子们
            //此处需要拷贝所需节点到另外一串链表里
            ed* newNode = (ed*)malloc(sizeof(ed));
            if (newNode == NULL) {
                printf("糟糕！在复制结果时内存不足了... (｡ŏ_ŏ)\n");
                //致命错误，立刻停止并清理已创建的结果集
                rls(&head_src); // 释放已经收集到的部分结果       //子函数调用难道还要按顺序？还真是，就像一个变量，他得先声明才能被使用
                    //rls会从头开始释放一直到尾，所以没问题
                return NULL;  // 返回NULL表示失败
            }

            // b. 把节点 i 的【所有数据】复制到 newNode  //DEBUG5: 并没有所谓的i，是current
            newNode->id = /*i*/current->id;
            strcpy(newNode->loca, /*i*/current->loca);
            strcpy(newNode->date, /*i*/current->date);
            newNode->temp = /*i*/current->temp;
            newNode->humi = /*i*/current->humi;
            newNode->pm25 = /*i*/current->pm25;
            newNode->wdspd = /*i*/current->wdspd;
            newNode->wddir = /*i*/current->wddir;
            newNode->pollv = /*i*/current->pollv;
            // 注意【没有】复制 /*i*/current->next

            // c. 使用头插法，将这个全新的副本 newNode 插入到“结果集”链表 (head_src) 的头部
            newNode->next = head_src;
            head_src = newNode;
        }
        current = current->next; // 移动到下一个节点
    }

    printf("\n--- 搜索结束 ---\n");
    if (found_count == 0) {
        printf("嘶...没有找到符合所有条件的记录啊(｡ŏ_ŏ)\n");
    }
    else {
        printf("完成！共找到 %d 条符合条件的记录的说(=´ω`=)\n", found_count);
    }
    return head_src;//指向了那个只包含搜索结果的新链表
}

//之前是废案，但现在考虑到要排序显示结果，所以得先排序再输出，故加回来
void prt(const /*envdata*/ed* node) {
    //展示数据
    if (node == NULL) return;
    printf(/*"ID: %-4d | 位置: %-20s | 温度: %5.1°C | 湿度: %4.1f%% | PM2.5: %5.1f | 风速: %4.1f m/s | 风向：%3f° | 日期: %s | 污染等级: %d\n",*/
        "ID: %-4d | 位置: %-50s | 温度: %2.1f°C  | 湿度: %4.1f%% | PM2.5: %5.1f | 风速: %4.1f m/s | 风向: %3d° | 日期: %s | 污染等级: %d\n",//errorchange 风向是int
        node->id,               //源代码文件保存为UTF-8，而编译器按GBK读取（或者反之），导致像°这样的特殊符号的编码被错误解析?
        node->loca,             //二编：完全不是因为这个！温度那里少了f...
        node->temp,             //GEMINI...
        node->humi,
        node->pm25,
        node->wdspd,
        node->wddir,
        node->date,
        node->pollv);
}
//advancechange: 如果有负温度，对齐就会出问题，这个无法用%a.b解决

typedef struct envdataedi {
    //int id;id显然不能改
    char loca[100];         // 监测点位置
    float temp;             // 温度, celcius
    float humi;             // 湿度, %
    float pm25;             // PM2.5浓度,取一位小数
    float wdspd;            // 风速 (单位: m/s)
    int wddir;              // 风向 (单位: 度, 默认向北，顺时针旋转0-360°)
    char /*date[11]*/date[12];//errorchange: 在最后添加数据时，如果11字符截断的话，\n会留在缓冲区里面...          // 日期（格式：YYYY-MM-DD）
    int pollv;              // 污染等级 (1=优, 2=良, 3=轻度污染, 4=中度污染, 5=重度污染)
    //struct envdata* next;指向下一个节点的指针也不需要
}ede;

void edi(ed* edited, const ede* data, unsigned int flag) {//目标，源，指令【习惯排列】
    //修改数据
    //仍然是显然的重复工作，使用ai代写
    //但是值得注意的是，最好也是直接传入一个结构体
        //但是，使用ed会有内存释放风险，eds则有冗余变量，故为了意图明显使用envdataedi这个新结构体
    
    // --- 开始前先做一些检查 (o´ω`o)ﾉ ---

    // 健壮性检查：确保目标节点和新数据源都存在
    if (edited == NULL) {
        printf("呜哇！Σ(°△°|||) 你想修改的那个节点好像不见了，我找不到它...\n");
        return;
    }
    if (data == NULL) {
        printf("欸？(・_・?) 你好像忘了给我新数据，我不知道要改成什么呀。\n");
        return;
    }

    // 检查是否有指定任何要修改的字段
    if (flag == 0) {
        printf("唔...你叫我来修改，但是没有告诉我具体要改哪个地方呢...(._.`)\n");
        return;
    }

    printf("\n收到指令！开始为ID为 %d 的节点进行更新~ (o゜▽゜)o☆\n", edited->id);
    int modified_count = 0;

    // --- 使用位掩码，逐一检查并执行修改操作 ---

    // 1. 修改位置
    if (flag & src_loca) {
        strcpy(edited->loca, data->loca);
        printf("  位置已更新为: %s\n", data->loca);
        modified_count++;
    }

    // 2. 修改温度
    if (flag & src_temp) {
        edited->temp = data->temp;
        printf("  温度已更新为: %.1f °C\n", data->temp);
        modified_count++;
    }

    // 3. 修改湿度
    if (flag & src_humi) {
        edited->humi = data->humi;
        printf("  湿度已更新为: %.1f %%\n", data->humi);
        modified_count++;
    }

    // 4. 修改PM2.5
    if (flag & src_pm25) {
        edited->pm25 = data->pm25;
        printf("  PM2.5浓度已更新为: %.1f\n", data->pm25);
        modified_count++;
    }

    // 5. 修改风速
    if (flag & src_wdspd) {
        edited->wdspd = data->wdspd;
        printf("  风速已更新为: %.1f m/s\n", data->wdspd);
        modified_count++;
    }

    // 6. 修改风向
    if (flag & src_wddir) {
        edited->wddir = data->wddir;
        printf("  风向已更新为: %d°\n", data->wddir);
        modified_count++;
    }

    // 7. 修改日期
    if (flag & src_date) {
        strcpy(edited->date, data->date);
        printf("  日期已更新为: %s\n", data->date);
        modified_count++;
    }

    // 8. 修改污染等级
    if (flag & src_pollv) {
        edited->pollv = data->pollv;
        printf("  污染等级已更新为: %d\n", data->pollv);
        modified_count++;
    }

    printf("--- 更新完成！一共修改了 %d 个字段~ (ﾉ>ω<)ﾉ ---\n", modified_count);
}

void del(ed** head,int id) {
    //删除节点
    
    //C语言链表并没有数组那么简单，尤其在于连接逻辑和内存释放
    //如果删除的是普通节点B，可以直接A->B->C改为A->C，即修改前一个结点的next，然后free(B)即可
        //previous->next = current->next;
    //如果那个节点是最先的呢？没有前一个结点了，而是head->A->...这样就需要修改head
        //head = head->next;

    ed* i = *head;
    ed* j = NULL;
    /*
    if (head->next->id == id) {
        //就是第一个节点！
        head = head->next;
    }
    else {
        while (i->next->id != id) {
            if (i->next == NULL) { 
                printf("删除节点失败呢...可能是节点不存在？(._.`)")
                return;
            }
            i = i->next;
        }
        i->next = i->next->next;
    }
    */
    //上面有什么问题？如果你不提前把那个要删除的节点记录下来，那么改变链表连接之后...
        //你就再也找不到那个链表节点了！
    //以及，可能链表全空

    if (i == NULL) {
        printf("数据集是空的...先加载再来删除吧(._.`)\n");
        return;
    }
    if (i/*->next*/->id == id) {
        //就是第一个节点！
        *head = i->next;
        //此刻i还存着原来的*head，即head指向的那个原头
    }
    else {
        while (i->id != id) {
            if (i->next == NULL) {
                printf("删除节点失败呢...可能ID为 %d 的节点不存在？(._.`)", id);
                return;
            }
            j = i;//先准备好原节点（PREVIOUS）
            i = i->next;//CURRENT往前跑
        }
        j->next = i->next;
        //previous->next = current->next!
    }
    free(i);
    printf("成功删除ID为 %d 的节点(ﾉ>ω<)ﾉ\n", id);
    //如果head是**ed，那么*head不就是***ed吗？我之前有这么的疑惑，但事实是
        //星号在C语言中有两种截然不同的身份！
    /*
    身份一：在“声明”时，作为“类型说明符”
        作用：告诉编译器，我正在定义一个指针类型。
        例子：
            int* p; -> “p是一个指针，它指向一个int”
            ed* head; -> “head是一个指针，它指向一个ed结构体”
            ed** head_ptr; -> “head_ptr是一个指针，它指向一个ed*类型的指针”

    身份二：在“使用”时，作为“解引用运算符”
        作用：这是一个动作！它表示“顺着这个指针，找到它所指向的那块内存”。
        例子：
            *p = 10; -> “顺着p找到那个int，把它的值改成10”
            printf("%d", (*head).id); -> “顺着head找到那个ed结构体，然后访问它的id”
            printf("%d", head->id); -> (这是上面的语法糖)

    为什么*head不是***ed？
    ***ed是一个类型，它表示“指向‘指向指针的指针’的指针”。
    *head是一个表达式，是一个动作，它的结果是一个ed*类型的值（地址）。
    
    把它们分开理解：
    ed** head_param; (声明了一个二级指针变量)
    ed* head = *head_param; (使用二级指针，解引用一次，得到一个一级指针)
    ed node = **head_param; (使用二级指针，解引用两次，得到一个实体)
    */
}


//辅助函数：交换两个节点内部的数据
//注意：这个函数【只交换数据】，不改变节点的物理位置或next指针
//否则next转换逻辑都已经很烦人了...而且也没有必要
void swp(ed* a, ed* b) {
    ed temp_storage; // 创建一个临时的结构体来中转

    // 1. 把 a 的所有数据（除了next指针）都备份到 temp_storage
    temp_storage.id = a->id;
    strcpy(temp_storage.loca, a->loca);
    strcpy(temp_storage.date, a->date);
    temp_storage.temp = a->temp;
    temp_storage.humi = a->humi;
    temp_storage.pm25 = a->pm25;
    temp_storage.wdspd = a->wdspd;
    temp_storage.wddir = a->wddir;
    temp_storage.pollv = a->pollv;

    // 2. 把 b 的数据复制到 a
    a->id = b->id;
    strcpy(a->loca, b->loca);
    strcpy(a->date, b->date);
    a->temp = b->temp;
    a->humi = b->humi;
    a->pm25 = b->pm25;
    a->wdspd = b->wdspd;
    a->wddir = b->wddir;
    a->pollv = b->pollv;

    // 3. 把备份在 temp_storage 里的原始 a 数据，复制到 b
    b->id = temp_storage.id;
    strcpy(b->loca, temp_storage.loca);
    strcpy(b->date, temp_storage.date);
    b->temp = temp_storage.temp;
    b->humi = temp_storage.humi;
    b->pm25 = temp_storage.pm25;
    b->wdspd = temp_storage.wdspd;
    b->wddir = temp_storage.wddir;
    b->pollv = temp_storage.pollv;
}

void ord(ed** head, unsigned int flag) {
    //排序
    //排序方法千千万，但是链表没有办法“随机访问”而只能遍历，所以只能用最朴实的冒泡
    
    //这里最优的处理方法是函数指针，并写若干个小比较器函数，用统一的格式调用
    //但是又有些牛刀杀鸡了，当时想到了这样的方法，可以避免普通情况下双层if和若干个相同的排序
    //如果 (排序标志是ID 并且 ID需要交换) 或者 (排序标志是温度 并且 温度需要交换) 或者 ... 那么就进行交换。
    
    // 如果链表为空，或者只有一个节点，那就不需要排序啦 ( ´ ▽ ` )ﾉ
    if (*head == NULL || (*head)->next == NULL) {
        return;
    }

    ed* i; // 外层循环指针
    ed* j; // 内层循环指针

    //将前后两两相比较，每比较一次会把一轮最大或最小的数放在最后
    // 外层循环：从头开始
    for (i = *head; i->next != NULL; i = i->next) {
        // 内层循环：从 i 的下一个节点开始，与 i 进行比较
        for (j = i->next; j != NULL; j = j->next) {

            // --- 这就是那个“臃肿但有效”的单一if语句 ---
            // 它的作用是判断“i节点是否应该排在j节点的后面”
            // (默认按升序排列)
            if (
                (flag == src_id && i->id > j->id) ||
                (flag == src_loca && strcmp(i->loca, j->loca) > 0) ||
                (flag == src_date && strcmp(i->date, j->date) > 0) ||
                (flag == src_temp && i->temp > j->temp) ||
                (flag == src_humi && i->humi > j->humi) ||
                (flag == src_pm25 && i->pm25 > j->pm25) ||
                (flag == src_wdspd && i->wdspd > j->wdspd) ||
                (flag == src_wddir && i->wddir > j->wddir) ||
                (flag == src_pollv && i->pollv > j->pollv)
                ) {
                // 如果条件成立，说明 i 和 j 的顺序错了，需要交换它们的数据
                swp(i, j);
            }
        }
    }
}

//以下均为分箱函数及其准备工作

// 特征枚举
typedef enum {
    TEMP, HUMI, PM25, WIND_SPD, WIND_DIR, NUM_FEATURES
} FeatureType;

// 温度分箱 (5箱)
typedef enum {
    TEMP_VERY_COLD, // <= -10
    TEMP_COLD,      // (-10, 0]
    TEMP_COOL,      // (0, 10]
    TEMP_WARM,      // (10, 20]
    TEMP_HOT,       // (20, 30]
    TEMP_VERY_HOT   // > 30
} TempBin;

// 湿度分箱 (4箱)
typedef enum {
    HUMI_DRY,       // [0, 25]
    HUMI_COMFORT,   // (25, 50]
    HUMI_WET,       // (50, 75]
    HUMI_VERY_WET   // (75, 100]
} HumiBin;

// PM2.5分箱 (6箱 - 参考国标AQI)
typedef enum {
    PM25_EXCELLENT, // [0, 35]
    PM25_GOOD,      // (35, 75]
    PM25_LIGHT,     // (75, 115]
    PM25_MODERATE,  // (115, 150]
    PM25_HEAVY,     // (150, 250]
    PM25_SEVERE     // > 250
} PM25Bin;

// 风速分箱 (7箱 - 参考蒲福风级)
typedef enum {
    WIND_CALM,      // 0-1级: [0, 1.5]
    WIND_LIGHT,     // 2-3级: (1.5, 5.4]
    WIND_MODERATE,  // 4-5级: (5.4, 10.7]
    WIND_FRESH,     // 6-7级: (10.7, 17.1]
    WIND_STRONG,    // 8-9级: (17.1, 24.4]
    WIND_GALE       // > 9级: > 24.4
} WindSpeedBin;

// 风向分箱 (8箱)
typedef enum {
    DIR_N, DIR_NE, DIR_E, DIR_SE, DIR_S, DIR_SW, DIR_W, DIR_NW
} WindDirBin;

// 【关键】创建一个常量数组来存储每个特征的分箱数
const int NUM_BINS_PER_FEATURE[NUM_FEATURES] = {
    6, // TEMP (0) -> 6个分箱
    4, // HUMI (1) -> 4个分箱
    6, // PM25 (2) -> 6个分箱
    6, // WIND_SPD (3) -> 6个分箱
    8  // WIND_DIR (4) -> 8个分箱
};

// --- 1. 温度分箱函数 (按10度分档) ---
int get_temp_bin(double temp) {
    if (temp <= -10.0) return TEMP_VERY_COLD;
    if (bet(temp, -10.0, 0.0)) return TEMP_COLD;
    if (bet(temp, 0.0, 10.0)) return TEMP_COOL;
    if (bet(temp, 10.0, 20.0)) return TEMP_WARM;
    if (bet(temp, 20.0, 30.0)) return TEMP_HOT;
    return TEMP_VERY_HOT; // 所有其他情况 ( > 30 )
}

// --- 2. 湿度分箱函数 (平均分为4档) ---
int get_humi_bin(double humi) {
    if (bet(humi, 0.0, 25.0)) return HUMI_DRY;
    if (bet(humi, 25.0, 50.0)) return HUMI_COMFORT;
    if (bet(humi, 50.0, 75.0)) return HUMI_WET;
    return HUMI_VERY_WET; // 默认 (75.0, 100.0]
}

// --- 3. PM2.5分箱函数 (参考国标) ---
int get_pm25_bin(double pm25) {
    if (bet(pm25, 0.0, 35.0)) return PM25_EXCELLENT;
    if (bet(pm25, 35.0, 75.0)) return PM25_GOOD;
    if (bet(pm25, 75.0, 115.0)) return PM25_LIGHT;
    if (bet(pm25, 115.0, 150.0)) return PM25_MODERATE;
    if (bet(pm25, 150.0, 250.0)) return PM25_HEAVY;
    return PM25_SEVERE; // > 250
}

// --- 4. 风速分箱函数 (参考蒲福风级) ---
// 0-1级: 0-1.5 m/s
// 2-3级: 1.6-5.4 m/s
// 4-5级: 5.5-10.7 m/s
// 6-7级: 10.8-17.1 m/s
// 8-9级: 17.2-24.4 m/s
// > 9级: > 24.4 m/s
int get_wind_speed_bin(double wdspd) {
    if (bet(wdspd, 0.0, 1.5)) return WIND_CALM;
    if (bet(wdspd, 1.5, 5.4)) return WIND_LIGHT;
    if (bet(wdspd, 5.4, 10.7)) return WIND_MODERATE;
    if (bet(wdspd, 10.7, 17.1)) return WIND_FRESH;
    if (bet(wdspd, 17.1, 24.4)) return WIND_STRONG;
    return WIND_GALE; // > 24.4
}

// --- 5. 风向分箱函数 (8个方向) ---
int get_wind_dir_bin(int wddir) {
    int angle = wddir;
    // 将0-360度的圆盘，映射到0-7的8个扇区
    // 每个扇区45度。分界线在 22.5, 67.5, ...
    //加上22.5度再除以45度即可
    return (angle + 22) / 45 % 8;
}


void sta(ed* head,ed* analysed) {
    //统计分析
    //一个半小时速通朴素贝叶斯！！！

    //朴素贝叶斯其实就是先验概率P(C)和条件概率P(F|C)

    //需要一个三维数组[哪个数据][这个数据的什么等级][污染等级]
    int ftr[5][8][6] = { {{0}} };//feature,5个特征，最多8个档次，6个污染等级，用来算条件概率
    int vld = 0;//valid,有效数据
    int cat[6] = { 0 };//category,污染等级计数，用来算先验概率

    double pri[6] = { 0 };//prior,先验概率
    double lik[5][8][6] = { {{0}} };//likelihood,条件概率，在（前两维的）条件下处于（最后一个维度确定的）污染等级上的概率

    //先分类
    //湿度平均地分为四档，温度-10~30每十度分一档（当然还有两侧的），pm2.5照抄国家标准6挡，
    //风向8档（可以按照+22再÷22.5-1的方法），风速按照蒲福氏风级每两级划分为一个档次，24.4以上单独）
    //见那几个get_xxx_bin函数，就是gemini所谓“分箱”

    //由于单独一个[a][a][a]太不直观了，Gemini建议了enum出不同数字代表的意思，如[0][3][4]->[TEMP][TEMP_HOT][LEVEL_SEVERE]
    //enum就是自定义的、有限的整数常量集合（枚举？）

    //以上两段注释说的内容见sta函数前面的enum和#define

    //Ⅰ遍历统计
    ed* i = head;
    while (i != NULL) {
        if (bet(i->pollv, 1, 5)) {//有效污染等级才统计
            vld++;
            cat[i->pollv]++;//该污染等级计数+1
            //温度
            int temp_bin = get_temp_bin(i->temp);
            ftr[TEMP][temp_bin][i->pollv]++;
            //湿度
            int humi_bin = get_humi_bin(i->humi);
            ftr[HUMI][humi_bin][i->pollv]++;
            //PM2.5
            int pm25_bin = get_pm25_bin(i->pm25);
            ftr[PM25][pm25_bin][i->pollv]++;
            //风速
            int wdspd_bin = get_wind_speed_bin(i->wdspd);
            ftr[WIND_SPD][wdspd_bin][i->pollv]++;
            //风向
            int wddir_bin = get_wind_dir_bin(i->wddir);
            ftr[WIND_DIR][wddir_bin][i->pollv]++;
        }
        i = i->next;
    }

    //Ⅱ计算先验概率
    {
        int c;
        for (c = 1; c <= 5; c++) {
            pri[c] = (double)cat[c] / vld;
        }
    }

    //Ⅲ计算条件概率
    {
        int f, b, c;
        for (int f = 0; f < 5; f++) { // 遍历特征
            for (int c = 1; c <= 5; c++) { // 遍历污染等级
                for (int b = 0; b < NUM_BINS_PER_FEATURE[f]; b++) { // 遍历该特征的分箱
                    // P(F=b|C=c) = (在c类中F=b的次数) / (c类的总次数)
                    lik[f][b][c] = ((double)ftr[f][b][c] + 1.0) / (cat[c] + NUM_BINS_PER_FEATURE[f]);
                    //拉普拉斯平滑，概率永远不为零，分子加1，分母加上类别数
                    //防止最终概率就会因为连乘且某一项为0而变成0
                }
            }
        }
    }

	//Ⅳ对给定的analysed链表进行分析
    //不得不说copilot这个帮写代码有点过于强大了...要不还是关掉吧
    ed* j = analysed;
    while (j != NULL && /*j->id == 0*/j->pollv == 0) {
        //double post[6] = { 0 };//posterior,后验概率
		double logpost[6] = { 0 };
        //需要是对数形式，否则越乘越小
        int t_bin = get_temp_bin(j->temp);
        int h_bin = get_humi_bin(j->humi);
        int p_bin = get_pm25_bin(j->pm25);
        int ws_bin = get_wind_speed_bin(j->wdspd);
        int wd_bin = get_wind_dir_bin(j->wddir);
        int c;
        for (c = 1; c <= 5; c++) {
            /*post[c] = pri[c] *
                lik[TEMP][t_bin][c] *
                lik[HUMI][h_bin][c] *
                lik[PM25][p_bin][c] *
                lik[WIND_SPD][ws_bin][c] *
                lik[WIND_DIR][wd_bin][c];
                */
            logpost[c] = log(pri[c]) +
                log(lik[TEMP][t_bin][c]) +
                log(lik[HUMI][h_bin][c]) +
                log(lik[PM25][p_bin][c]) +
                log(lik[WIND_SPD][ws_bin][c]) +
				log(lik[WIND_DIR][wd_bin][c]);
            //
        }
        //找出最大后验概率对应的污染等级
        int best_c = 1;
        double best_post = logpost[1];

		double confidence = 0.0;//总置信度

        for (c = 2; c <= 5; c++) {
            /*if (post[c] > best_post) {
                best_post = post[c];
                best_c = c;
            }*/
			confidence += exp(logpost[c]);//计算总概率，用于置信度计算
			best_post = maxi(best_post, logpost[c]);
			best_c = (best_post == logpost[c]) ? c : best_c;
        }
        /*
        //将预测的污染等级赋值给节点
        j->pollv = best_c;
        j = j->next;
        */

		//Ⅴ显示预测结果及置信程度
        prt(j);
        printf("预测ID为 %d 的这个节点污染等级为 %d，置信度约为 %.2f%%哦~\n",
            j->id,
            best_c,
			100 * exp(best_post) / confidence //将对数形式的后验概率转换回原始概率，并计算相对置信度百分比
		);
		printf("\n");

        //将预测的污染等级赋值给节点，你得先展示再赋值
        j->pollv = best_c;
        j = j->next;
	}
}


void sav(/*ed* saved,*/ ed* head) {//并不需要给出特定节点，因为是对全体节点存储
    //存储文件
/*Ⅰ 获取时间，即默认文件名*/
    char szFile[260];//文件名字符串

    //C语言原始库获取时间的方法太死板了，真的
    time_t ancient;//time_t是一种特殊的long int，这里就是声明变量
    time(&ancient);//获取Unix时间戳：从1970年1月1日午夜到现在的总秒数
    struct tm* modern;//C语言贴心（存疑）的为你内置了这个结构体，专门用来存放分解后的时间信息
    modern = localtime(&ancient);//这个内置函数就是干时间转换的，现在是正常的时分秒了
    strftime(szFile, sizeof(szFile), "%Y-%m-%d_%H-%M-%S.csv", modern);
    //string format time，一个极其强大的函数，可以按照指定的任何格式，把struct tm里的信息“打印”到字符串里
    /*
    % Y : 四位数的年份(e.g., 2024) % m: 两位数的月份(01 - 12) % d : 两位数的日期(01 - 31)
    % H : 24小时制的时(00 - 23) % M : 两位数的分(00 - 59) % S : 两位数的秒(00 - 60)
    */

/*Ⅱ 非常自由的存文件*/
    //接下来是存文件时间，为了更现代，我们还是用日常使用中常见的windows“另存为”对话框
        //这个api会比我之前用过的各种windowsapi复杂很多，原因是...
    /*
    因为它开启的是一个“双向的、持续的、充满变数的对话”，而不是一个简单的问答或命令。
    这个对话的另一方，是一个你完全无法预测的人类用户。

    思考一下，当那个“另存为”窗口弹出时，可能发生多少种情况？
        用户可能直接点击“保存”。用户可能修改你提供的默认文件名。用户可能切换到另一个磁盘、另一个文件夹。
        用户可能新建一个文件夹。用户可能从下拉菜单里选择不同的文件类型（比如从.csv切换到.txt）。
        用户可能会输入一个非法的文件名（比如con或aux）。用户可能会选择一个只读的文件夹，导致保存失败。
        用户可能什么都不做，直接点击了“取消”或关闭按钮。用户可能会把窗口放一边，过了10分钟才想起来操作。
        ...
    那个“巨大的结构体”(OPENFILENAME)就是为了应对所有这些不确定性而设计的！ 
        lpstrFile: 你在提供一个缓冲区，让API能把用户最终的决定传回给你。
        lpstrFilter: 你在配置对话框的下拉菜单里应该显示哪些选项。
        lpstrTitle: 你在配置对话框的标题栏文字。
        Flags: 你在用一系列“开关”配置对话框的各种高级行为（比如“如果文件已存在，请弹出提示”）。
    比喻：这不再是买可乐了，之前的那些什么获取句柄/移动光标，都是瞬时的、可预测的。
    这是你在定制一辆汽车。销售员（API）给你一张长长的配置表（OPENFILENAME），
    你需要详细填写你想要的引擎型号、车身颜色、内饰材质、轮毂样式... 因为你的选择非常多，所以这张表必然很长、很复杂。
    */
    //当然，比C语言更高级的那些语言有更好的封装，但是本质是一样的
    
    OPENFILENAME ofn;        //存文件各种配置项の结构体
    //char szFile[260];      //接收文件名
    //如果我们此前对szfile赋值了，那么就会成“默认文件名”！

    memset(&ofn, 0, sizeof(ofn));//赋值前先将整个结构体的内存清零
    
    //配置项
    ofn.lStructSize = sizeof(ofn);//通过检查大小，确认ofn结构体的版本
    ofn.hwndOwner = NULL;//对话框的父窗口，控制台中无此概念
    ofn.lpstrFile = szFile;//Long Pointer to String（表示常量字符串的长指针数据类型）
                           //告诉结构体那个文件名字符串的地址，以便知道把结果填到哪里
    ofn.nMaxFile = sizeof(szFile);//文件名上限
    ofn.lpstrFilter = "CSV 类数据 (*.csv)\0*.csv\0所有文件 (*.*)\0*.*\0";
        //文件类型过滤器，\0为分隔符，引号内为给用户看的文字，后面那一项是真正的规则
    ofn.nFilterIndex = 1;//默认选中上述第一个过滤器，即csv
    ofn.lpstrInitialDir = NULL;//默认打开路径
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;//对话框行为
        //第一项：路径要真实存在   第二项：不能与其他文件重名，否则弹出弹窗
    ofn.lpstrDefExt = "csv"; //添加默认扩展名csv

    //提交
    if (GetSaveFileName(&ofn) == TRUE) {//C89震怒...吗？并非，Windows.h内定义了t和f
        //一点小插曲，vsstudio显示
        //“GetSaveFileName 扩展到GetSaveFileNameA” 
        //“浏览到此宏的操作可能会失败，请考虑将它添加到提示文件中”
        //并非错误，而是Windows API为了同时支持两种不同的字符编码，搞了一点宏定义
        /*原始代码belike    
        #ifdef UNICODE
            #define GetSaveFileName GetSaveFileNameW
        #else
            #define GetSaveFileName GetSaveFileNameA
        #endif
        */
        //而vs的‘智能感知’(IntelliSense)功能会犯迷糊，所以会这样

        // 如果成功，打印用户选择的文件名
        printf("好啦！(*´∀`)~♥文件保存为: %s\n", ofn.lpstrFile);

/*Ⅲ 打开文件*/
        //可以用 ofn.lpstrFile(文件地址) 去 fopen（打开文件） 了
        FILE* saving = fopen(ofn.lpstrFile, "w");//w是写入，如果文件不存在则创建，如果文件已存在则清空
        //FILE类是一个结构体，就是文件名、修改时间、权限等一些信息
        if (saving == NULL) {
            printf("保存失败了！试着换个文件名？(ㆀ˘･з･˘)\n");
            return;
        }
        else {
/*Ⅳ 向文件存入信息*/
            //使用csv逗号分隔格式，可以用excel打开
            //这样是为了方便在程序外打开也能看懂，而且我们大抵是不会有输入英文逗号的，所以没有风险
            //为了更加方便阅读，第一行我们像一个普通表格一样给每一列起标题
            fprintf(saving, "ID,位置,日期,温度,湿度,PM2.5浓度,风速,风向,污染等级\n");
            //使用fprintf格式化写入，需要遍历所有节点
            ed* i = head;
            while (i != NULL) {
                //int test = fprintf(saving, "%d,%s,%s,%.1f,%.1f,%.1f,%.1f,%d,%d\n", i->id, i->loca, i->date, i->temp, i->humi, i->pm25, i->wdspd, i->wddir, i->pollv);
                //advancechange: 我恨excel，它会自动转换日期格式变成/分割+吞0，真是纸张啊，必须=\ \来伪装一下
                int test = fprintf(saving, "%d,%s,=\"%s\",%.1f,%.1f,%.1f,%.1f,%d,%d\n", i->id, i->loca, i->date, i->temp, i->humi, i->pm25, i->wdspd, i->wddir, i->pollv);
                //fprintf函数返回成功打印（或写入）的字符总数。如果发生错误，返回一个负数。
                //所以可以用来检测bug，如磁盘满、文件只读等原因都可能失败
                    //printf也是这样，scanf则是匹配和赋值的变量数
                if (test < 0) {
                    printf("写入文件错误！怎么回事呢...Σ(°△°|||)\n");
                    fclose(saving);//一旦出错先尝试关闭文件
                    return;//直接结束
                }
                i = i->next;
            }
/*Ⅴ 关闭文件，收工！*/
            fclose(saving);
            printf("所有数据都安全地保存好啦！(=´ω`=)\n");
            return;
        }
    }
    else {
        printf("操作取消了...那再考虑下？(｡•ㅅ•｡)♡\n");
        return;
    }
}   
//复盘的时候，发现几次在else里面大幅展开
//事实上有种更好的方法，就是所谓卫语句/提前退出
    //在函数开头，集中处理所有的错误情况，一旦发现错误就立即return。
    //这样，函数的主体逻辑就可以在没有else包裹的情况下，“平铺”地写下来。

//见debug7
#define true 1;
#define false 0;

int qlf(const ed* ck);//errorchange

void red(ed** head) {//它必须能够修改main函数里的那个head指针，所以传入二级指针
    //读取文件

/*Ⅰ 非常自由的选取文件*/
    //几乎和保存文件异曲同工
    OPENFILENAME ofn;       //存文件各种配置项の结构体
    char szFile[260]= "支持Do It Yourself!!和Medalist谢谢喵";       //初始被打开文件名
    //Windows允许的路径最大长度是260
    //对的，这个不止文件名，也存路径

    memset(&ofn, 0, sizeof(ofn));//赋值前先将整个结构体的内存清零
    //szFile[0] = '\0';//字符串清零不用遍历，直接开头换成结束符即可

    //配置项
    ofn.lStructSize = sizeof(ofn);//通过检查大小，确认ofn结构体的版本
    ofn.hwndOwner = NULL;//对话框的父窗口，控制台中无此概念
    ofn.lpstrFile = szFile;//Long Pointer to String（表示常量字符串的长指针数据类型）
    //告诉结构体那个文件名字符串的地址，以便知道把结果填到哪里
    ofn.nMaxFile = sizeof(szFile);//文件名上限
    ofn.lpstrFilter = "CSV 类数据 (*.csv)\0*.csv\0所有文件 (*.*)\0*.*\0";
    //文件类型过滤器，\0为分隔符，引号内为给用户看的文字，后面那一项是真正的规则
    ofn.nFilterIndex = 1;//默认选中上述第一个过滤器，即csv
    ofn.lpstrInitialDir = NULL;//默认打开路径
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;//对话框行为
    //第一项：路径要真实存在   第二项：文件必须存在
    ofn.lpstrDefExt = "csv"; //添加默认扩展名csv

/*Ⅱ 打开被选择文件*/
    if (GetOpenFileName(&ofn) == TRUE) {
        printf("好啦！(*´∀`)~♥已打开文件: %s\n", ofn.lpstrFile);

/*Ⅲ 读取文件*/
        //可以用 ofn.lpstrFile(文件地址) 去 fopen（打开文件） 了
        FILE* reading = fopen(ofn.lpstrFile, "r");//r是读取
        if (reading == NULL) {
            printf("读取失败了！是不是文件名输错了？(ㆀ˘･з･˘)\n");
            return;
        }
        char firstline[514];//只是512+2罢了（确信）
        //第一行是文字，需要忽略，但是要注意可能文件损坏或者为空
        if (fgets(firstline, sizeof(firstline), reading) == NULL) {
            printf("NO!文件为空或读取错误...(｡•ㅅ•｡)\n");
            //无论如何都要关闭文件
            fclose(reading);
            return;
        }
        else {
            char line[514];
            //int linecount = 1;          //DEBUG8: 后面都写的linecounter，但这里是linecount
            int linecounter = 1;
            while (fgets(line, sizeof(line), reading) != NULL) {
                //清理末尾的换行符
                line[strcspn(line, "\n")] = '\0'; // 找到第一个\n并替换为\0
                
                ed* newed = ins(head);//利用插入节点的函数获取一个新节点...
                //...的地址
                    //值得注意的是，ins也是会自动为你找到id的，这个在这里多余
                
                //插入（创建）节点失败の检查
                if (newed == NULL) {
                    printf("节点创建失败了...内存可能满了(｡•ㅅ•｡)\n");
                    break;
                }
                else {
                    /*
                    //fscanf（划掉）
                    sscanf(line, "%d,%c,%f,%f,%f,%f,%d,%c,%d\n",
                        newed->id, newed->loca, newed->temp,
                        newed->humi, newed->pm25, newed->wdspd,
                        newed->wddir, newed->date, newed->pollv);
                    //fscanf类似scanf，会跳过空格，那你loca字段里面有空格怎么办？
                    */
                        //但是sscanf也一样哦...它的好处不在这里，在于可以安全地跳过错误行
                        //上面的另一个问题是，%c是单个字符，用不了字符串写入

/*Ⅳ 写入数据*/
                    //还记得为什么要用csv吗？逗号分隔符，对吧，那为什么不用,来分割各项呢？
                    //请出strtok(string tokenizer)，语法是(char* 被分割字符串, const char* 分隔符)
                    //原理是
                        //它把这个分隔符原地修改成字符串的结束符 \0。
                        //然后，它会返回被切下来的第一个字段的起始地址。
                        //同时，它在自己内部记住了刚刚那个 \0 的位置，以便下次能从那里继续
                        //如果再次调用时第一项输入NULL，则从上次停下的地方继续
                        //即可重复“修改逗号为\0 -> 返回字段地址”这个过程
                        //最后返回NULL就是扫描失败了
                    //值得注意的是，为什么要用指针呢？因为ta可以指向一个字符...
                    //也可以指向一个字符序列（字符串）的开头！

                    char* token;

                    //但是切出来的仍然是字符串，要想复制到其他地方得做不同处理
                        //1.整型  atoi,ascii to integer
                        //2.浮点数 atof,ascii to float（虽然实际上转化为double而非float）
                        //3.字符串 由于数组不能复制式赋值，所以strcpy
                            
                        //二编：见这个函数后面的注释，我们需要做输入检测
                            //1.ato?那两个 
                            //你给他一段文字，他会尽力从开头翻译，一旦遇到他看不懂的东西，他就会停下来，把他已经翻译好的部分交给你。
                            //如果一开头就看不懂，他就直接给你一个“0”
                            //所以纯粹非数字/不纯粹的数字/空都会错误的转录为数字（或0）
                            //以及，溢出...
                                //我们有114种方法写大概514行来解决溢出/非数字/非完全数字的各种问题甚至给出对应报错
                                //但是有必要吗？我这个项目已经在1919810个细枝末节的地方抠了太多细节了，间接导致如此大量的代码和最后ddl极其紧急的状态                         
                            //错误输入？只看第一个字符，你后面乱了我不管了，简单一点
                            //溢出？直接strlen之类的，如果太长直接报错，不亦乐乎？
                            //int<=2,147,483,647，直接卡10位数，负数也杀无赦（这里int那几项不可能出现负数）
                            //float<=3.4*10^38，这倒没有问题，卡个15位数吧 
                             
                            //2.身体人才培养（划掉）strcpy
                            //如果输入太长，那么strcpy会无视location只有10个字节的容量，疯狂地向后写入
                            //这个好改，有strncpy，不会写入超过你指定长度的字符
                            //还是有更先进的snprintf，它会自动处理截断和结尾的'\0'...
                                //哎，但是很クソバカヤロー的事情是，c89跟现代这种事情可是不沾边的哦~
                            //所以只能strncpy，最多复制len-1个字符，最后一个留给'\0'
                                //什么，你问日期？yyyy-mm-dd?无视算了...
                                //这个要严格限制-位置还有什么月份还有**的leap真的不知道要写多少
                            
                            //至于具体取值范围，我再写个小函数吧，这个好办且需要在其他地方用

                    /*以下为初版
                    // 第一次调用，切出ID
                    token = strtok(line, ",");
                    if (token != NULL) newed->id = atoi(token); 

                    // 第二次调用，切出location
                    token = strtok(NULL, ",");
                    if (token != NULL) strcpy(newed->loca, token);

                    // 第三次调用，切出date
                    token = strtok(NULL, ",");
                    if (token != NULL) strcpy(newed->date, token);

                    // 第四次，切出temperature
                    token = strtok(NULL, ",");
                    if (token != NULL) newed->temp = atof(token); 

                    // ... 依次切出剩下的字段 ...
                    token = strtok(NULL, ",");
                    if (token != NULL) newed->humi = atof(token);

                    token = strtok(NULL, ",");
                    if (token != NULL) newed->pm25 = atof(token);

                    token = strtok(NULL, ",");
                    if (token != NULL) newed->wdspd = atof(token);

                    token = strtok(NULL, ",");
                    if (token != NULL) newed->wddir = atoi(token);

                    // 最后一次，切出 pollution level
                    token = strtok(NULL, ",");
                    if (token != NULL) newed->pollv = atoi(token);
                    */

                    //以下为改版，逻辑是我给的，ai负责搬砖
                    int parse_error = false;        //DEBUG7: 虽然Windows.h规定了布尔值一样的TRUEFALSE，但是是大写...

                    // --- 开始统一解析与验证 ---

                    // 1. ID (虽然ins已赋值, 但文件中的ID需要被解析以覆盖它)
                    token = strtok(line, ",");
                    if (token != NULL && strlen(token) < 11 && isdigit(token[0])) {
                        //newed->id = atoi(token);      //advancechange: 如果随文件里面的id读取的话，【重复了怎么办】？
                    }
                    else {
                        parse_error = true;
                    }

                    // 2. Location
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        if (token != NULL) {
                            strncpy(newed->loca, token, sizeof(newed->loca) - 1);
                            newed->loca[sizeof(newed->loca) - 1] = '\0';
                        }
                        else {
                            parse_error = true;
                        }
                    }

                    // 3. Date
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        /*if (token != NULL) {
                            strncpy(newed->date, token, sizeof(newed->date) - 1);
                            newed->date[sizeof(newed->date) - 1] = '\0';
                        }*/
                        
                        //advancechange: 见“我恨excel”

                        if (token != NULL) {
                            // 使用sscanf，从 "=\"YYYY-MM-DD\"" 格式中，只提取出我们想要的部分
                            // 尝试按 ="..." 格式剥离，如果成功(返回1)，就什么都不用做了
                            // 如果失败(返回0)，说明是普通格式，我们就直接拷贝
                            if (sscanf(token, "=\"%10[^\"]\"", newed->date) != 1) {//10是为了给末尾的/0留下空间，还要，【引号套引号必须用\"】
                                // sscanf 失败，说明 token 是一个普通日期，不是 ="..." 格式
                                strncpy(newed->date, token, sizeof(newed->date) - 1);
                                newed->date[sizeof(newed->date) - 1] = '\0';
                            }

                        }
                        else {
                            parse_error = true;
                        }
                    }
                    

                    // 4. Temperature
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        if (token != NULL && strlen(token) < 16 && (isdigit(token[0]) || token[0] == '-' || token[0] == '.')) {
                            newed->temp = atof(token);
                        }
                        else {
                            parse_error = true;
                        }
                    }

                    // 5. Humidity
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        if (token != NULL && strlen(token) < 16 && (isdigit(token[0]) || token[0] == '.')) {
                            newed->humi = atof(token);
                        }
                        else {
                            parse_error = true;
                        }
                    }

                    // 6. PM2.5
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        if (token != NULL && strlen(token) < 16 && (isdigit(token[0]) || token[0] == '.')) {
                            newed->pm25 = atof(token);
                        }
                        else {
                            parse_error = true;
                        }
                    }

                    // 7. Wind Speed
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        if (token != NULL && strlen(token) < 16 && (isdigit(token[0]) || token[0] == '.')) {
                            newed->wdspd = atof(token);
                        }
                        else {
                            parse_error = true;
                        }
                    }

                    // 8. Wind Direction
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        if (token != NULL && strlen(token) < 11 && isdigit(token[0])) {
                            newed->wddir = atoi(token);
                        }
                        else {
                            parse_error = true;
                        }
                    }

                    // 9. Pollution Level
                    if (!parse_error) {
                        token = strtok(NULL, ",");
                        if (token != NULL && strlen(token) < 11 && isdigit(token[0])) {
                            newed->pollv = atoi(token);
                        }
                        else {
                            parse_error = true;
                        }
                    }
                    linecounter++;
                    // --- 最终裁决 ---
                    //if (parse_error|| !qlf(token)) {            //errorchange: qlf未声明就调用
                    if (parse_error || !qlf(newed)) {//errorchange: token是切分的字符串，newed才是节点
                        // 如果解析过程中任何一步出错了，就打印统一的错误信息
                            //二编：qlf用于检测输入是否合理，这些不会导致出现硬性错误，但是不符合常理
                        printf("警告：文件中的第 %d 行数据格式不正确或数值只可能来自异世界，已跳过...\n", linecounter);

                        // 把刚刚添加的那个不完整的节点删除掉
                        del(head, newed->id);
                    }                   
                    else {
                        printf("ID为 %d 的节点数据已从文件加载！( •̀ ω •́ )✧这是第 %d 行哦\n", newed->id, linecounter);
                    }
                }
            }

/*Ⅴ 检查是否bug*/
            //因为EOF既能表示“读完了”又能表示“出错了”
            if (feof(reading)) {
                //检查是不是正常结束
                printf("文件已经全部读取完毕啦！(ﾉ>ω<)ﾉ\n");
            }
            else if (ferror(reading)) {
                //如果不是正常结束，检查是不是发生了意外
                printf("呜哇！读取文件时，磁盘或文件好像出了点问题...Σ(°△°|||)\n");
            }
            else {
                //【其他情况】fgets不会有这种问题，只可能是 fscanf
                // 如果因为格式不匹配（比如想读数字但读到了字母）导致循环退出，此时feof和ferror可能都false
                printf("嗯？文件数据格式好像不太对哦...Σ(°△°|||)\n");
            }
            //无论如何都要关闭文件
            fclose(reading);
            return;
        }
    }
    else {
        printf("操作取消了...那再考虑下？(｡•ㅅ•｡)♡\n");
        return;
    }
}
/*复盘的时候发现案例代码有几点优于我的写法
    1.“计数式解析”：通过一个fieldCount计数器，严格地根据字段在行中的位置来决定如何解析它。
        更具扩展性，如果未来你的CSV格式可能变化（顺序调换/增加可选字段），switch-case的结构更容易维护
    2.strtok_r (re-entrant, 可重入) 或 strtok_s (secure) 是strtok的线程安全版本
        strtok有一个“毛病”：它为了记住上次切分的位置，使用了一个全局的静态变量
        额外增加了一个char** rest参数（示例中的&rest），用来由程序员自己保存切分的状态，而不是依赖全局变量
            如果你在一个循环里用strtok切分字符串A，然后在这个循环内部又调用了另一个函数，那个函数也用strtok去切分字符串B，
            那么第二个strtok就会**“污染”**第一个strtok的内部状态，导致逻辑混乱
        当然，单线程的话这样也无所谓，这里就保留了，以起警示作用
    3.数据验证...这个确实要改，见各处atoi/atof/strcpy
*/

int qlf(const ed* ck) {
    //检测数值合法
    int humi = bet(ck->humi, 0.0, 100.0);
    int wddir = bet(ck->wddir, 0.0, 359.0);
    int pollv = bet(ck->pollv, /*1.0*/0.0, 5.0);//想到要做那个等级预测，所以填0默认无等级
    int temp = bet(ck->temp, -100, 100);
    int wdspd = bet(ck->wdspd, 0.0, 600);
    int pm25 = bet(ck->pm25, 0.0, 1500);
    //date限制某几位必须是-，不细卡日期了
    int date1 = (ck->date[4] == '-');
    int date2 = (ck->date[7] == '-');

    if (humi && wddir && pollv && temp && wdspd && pm25 && date1 && date2) {
        return 1;
    }

    return 0;
    //考虑之后还是觉得输出位运算结果没啥意义，到时候把输入标准展示出来不就可以了吗，自己排查去
}

//如果你想把qlf泛用，那么通用指针 void* + 类型标志 enum，里面还要必须赋给临时变量再判断...
//代码激增114行...我投降，我复制个不行吗(╯°▽°)╯ ┻━┻

/*
int qlf_e(const ede* ck) {
    //检测数值合法
    int humi = bet(ck->humi, 0.0, 100.0);
    int wddir = bet(ck->wddir, 0.0, 359.0);
    int pollv = bet(ck->pollv, 0.0, 5.0);
    int temp = bet(ck->temp, -100, 100);
    int wdspd = bet(ck->wdspd, 0.0, 600);
    int pm25 = bet(ck->pm25, 0.0, 1500);
    int date1 = (ck->date[4] == '-');
    int date2 = (ck->date[7] == '-');

    if (humi && wddir && pollv && temp && wdspd && pm25 && date1 && date2) {
        return 1;
    }
    return 0;
}
*/
//errorchange: ede不是满的，我们只需要判断那些改了的——ede里面有的，加入位运算
    //又是？：的妙用，看起来真的很好
int qlf_e(const ede* ck, unsigned int flag) {
    //检测数值合法

    //对于每个字段，我们都用三元运算符来决定它的校验结果
    //条件是“是否需要检查”，真的结果是“检查的实际结果”，假的默认结果是“1 (通过)”

    int humi = (flag & src_humi) ? bet(ck->humi, 0.0, 100.0) : 1;
    int wddir = (flag & src_wddir) ? bet(ck->wddir, 0.0, 359.0) : 1;
    int pollv = (flag & src_pollv) ? bet(ck->pollv, 0.0, 5.0) : 1;
    int temp = (flag & src_temp) ? bet(ck->temp, -100, 100) : 1;
    int wdspd = (flag & src_wdspd) ? bet(ck->wdspd, 0.0, 600) : 1;
    int pm25 = (flag & src_pm25) ? bet(ck->pm25, 0.0, 1500) : 1;

    // 对于日期，逻辑完全相同
    int date1 = (flag & src_date) ? (ck->date[4] == '-') : 1;
    int date2 = (flag & src_date) ? (ck->date[7] == '-') : 1;

    // --- 最终裁决：你的原始框架，完全不变 ---
    if (humi && wddir && pollv && temp && wdspd && pm25 && date1 && date2) {
        return 1;
    }

    return 0;
}

int qlf_s(const eds* ck) {
    //检测数值合法
    int id = (ck->idA <= ck->idB);
    int temp = (ck->tempA <= ck->tempB);
    int humi = (ck->humiA <= ck->humiB);
    int pm25 = (ck->pm25A <= ck->pm25B);
    int wdspd = (ck->wdspdA <= ck->wdspdB);
    int wddir = bet(ck->wddir, 0.0, 7.0); // wddir是0-7的方向
    int pollv = bet(ck->pollv, 0.0, 5.0);
    int date = (strcmp(ck->dateA, ck->dateB) <= 0);

    if (id && temp && humi && pm25 && wdspd && wddir && pollv && date) {
        return 1;
    }
    return 0;
}

void rls(ed** head) {
    //释放内存
    ed* i = *head;
    ed* j = NULL;
    while (i != NULL) {
        j = i;
        i = i->next;
        free(j);
    }
    //过河拆桥，临时保存待释放节点的指针
    *head = NULL;
    //解决悬挂指针，防止依旧存放着原来第一个节点的旧地址（现在是无效内存）
    //说明这是一个空的链表
}

//我们都知道，搜索要填eds，修改要填ede，这两个结构体不通用，那岂不是要写很多printfscanf？
//但是我们可以 停止编写“流程”，开始构建“组件”
//不妨试试从一些更小的环节开始流程化！我们无非就是要整数/浮点数/字符串！这是可以通用的

// --- 活字一号：获取一个整数 ---
int get_integer_input(const char* prompt, int* out_value) {
    char buffer[100];
    printf("%s", prompt); // 打印你传入的、自定义的提示语
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

        //advancechange: 在精确搜索中，我们希望“按回车就跳过上界，变为精确搜索”，但是会有“输入无效”提示，所以改动
        // 如果是空行，直接“静默失败”，返回0
        if (buffer[0] == '\n') return 0;

        if (sscanf(buffer, "%d", out_value) == 1) {
            return true; // 成功！
        }
    }
    printf("输入无效，请输入一个整数哦 (｡•ˇ‸ˇ•｡)\n");
    return false;
}

// --- 活字二号：获取一个浮点数 ---
int get_float_input(const char* prompt, float* out_value) {
    char buffer[100];
    printf("%s", prompt);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {

        //advancechange: 在精确搜索中，我们希望“按回车就跳过上界，变为精确搜索”，但是会有“输入无效”提示，所以改动
        // 如果是空行，直接“静默失败”，返回0
        if (buffer[0] == '\n') return 0;

        if (sscanf(buffer, "%f", out_value) == 1) {
            return true;
        }
    }
    printf("输入无效，请输入一个数字啦 (｡•ˇ‸ˇ•｡)\n");
    return false;
}

// --- 活字三号：获取一个字符串 ---
void get_string_input(const char* prompt, char* out_buffer, int buffer_size) {
    printf("%s", prompt);
    if (fgets(out_buffer, buffer_size, stdin) != NULL) {
        // 清理末尾的换行符
        out_buffer[strcspn(out_buffer, "\n")] = '\0';
    }
}

//获取flag也可以单独拿出来
unsigned int flg(int single) {
    unsigned int flag = 0; // 初始状态，所有开关都是关闭的
    int key;

    while (1) {
        system("cls"); // 清屏，准备重新绘制界面

        printf("--- 请选择您想操作的字段 ---\n");
        printf("   按对应数字键 [切换] 选中状态\n");
        printf("   按 [Enter] 确认选择\n");
        printf("   按 [ESC]   取消操作\n");
        printf("   ※ X表示选中，未显示表示未选中\n");
        printf("----------------------------------------\n");

        // --- 动态显示每个选项的当前状态 ---
        // (flag & src_id) 的结果如果非零(true)，就打印[X]，否则打印[ ]
            //这个确实妙，我当时只想到要单独写选中flag的子函数和其实现方法，但是动态显示这个我没给prompt
        printf(" [%c] 1. ID\n", (flag & src_id) ? 'X' : ' ');
        printf(" [%c] 2. 位置 (Location)\n", (flag & src_loca) ? 'X' : ' ');
        printf(" [%c] 3. 温度 (Temp)\n", (flag & src_temp) ? 'X' : ' ');
        printf(" [%c] 4. 湿度 (Humidity)\n", (flag & src_humi) ? 'X' : ' ');
        printf(" [%c] 5. PM2.5\n", (flag & src_pm25) ? 'X' : ' ');
        printf(" [%c] 6. 风速 (Wind Spd)\n", (flag & src_wdspd) ? 'X' : ' ');
        printf(" [%c] 7. 风向 (Wind Dir)\n", (flag & src_wddir) ? 'X' : ' ');
        printf(" [%c] 8. 日期 (Date)\n", (flag & src_date) ? 'X' : ' ');
        printf(" [%c] 9. 污染等级 (Level)\n", (flag & src_pollv) ? 'X' : ' ');

        printf("----------------------------------------\n");
        printf("请按键选择...\n");

        key = _getch(); // 等待并获取一个按键，不回显

        // --- 根据按键，更新flag ---
        switch (key) {
            // -- 切换选项 --
            // 使用 ^ (XOR) 来切换位的状态
        case '1': flag ^= src_id;    break;
        case '2': flag ^= src_loca;  break;
        case '3': flag ^= src_temp;  break;
        case '4': flag ^= src_humi;  break;
        case '5': flag ^= src_pm25;  break;
        case '6': flag ^= src_wdspd; break;
        case '7': flag ^= src_wddir; break;
        case '8': flag ^= src_date;  break;
        case '9': flag ^= src_pollv; break;
        

            // -- 确认或取消 --
        case 13: // 13 是 Enter (回车) 键的ASCII码
            if (flag != 0) return flag; // 确认！返回当前选中的flag
            //前提是你得选择点什么
        case 27: // 27 是 ESC 键的ASCII码
            printf("\n操作已取消... (｡•ㅅ•｡)♡\n");
            return 0;    // 取消！返回0表示没有选中任何东西

            // default:
                // 对于其他无效按键，我们什么都不做，循环会继续
                // 我就是要“输入错误不管”
        }
        if (single == 1 && /*bet(key, 0, 9)*/bet(key,'0','9')) {//advancechange: 注意输入的是ascii值
            //只让选一个的情况，如排序指标
            printf("这次只能选一个指标哦~马上继续啦(• ̀ω•́ )\n");
            //sleep(3000);
            Sleep(3000);//errorchange
            return flag;
        }
    }
}

void tip(unsigned int flag,int IHATEWINDDIR) {
    //提示输入文本
        //二编：传入一个flag然后按需展示

    // 如果flag为0，说明不需要显示任何特定提示，可以直接返回
    if (flag == 0) {
        return;
    }

    //提示输入文本
    printf("======================================================================\n");
    printf("                        注意！(｢･ω･)｢\n");
    printf("    为了保证数据合理规范且易于处理，输入数据时请遵循以下约定哦~\n");
    printf("----------------------------------------------------------------------\n\n");

    // --- 开始根据 flag 按需展示 ---

    if (flag & src_temp) {
        printf("    - 温度 (Temperature) 范围: (-100.0, 100.0)\n");
        printf("      单位: 摄氏度 (°C)\n");
        printf("      最低约-89.2°C于南极，最高约56.7°C于美国死亡谷\n\n");
    }

    if (flag & src_humi) {
        printf("    - 湿度 (Humidity) 范围: [0.0, 100.0]\n");
        printf("      单位: 百分比 (%%)\n\n");
    }

    if (flag & src_pm25) {
        printf("    - PM2.5 浓度范围: [0.0, 1500.0)\n");
        printf("      单位: 微克/立方米 (µg/m³)\n");
        printf("      沙尘暴、森林火灾或工业燃煤造成的局地瞬时PM2.5浓度可超过 1,000 µg/m³\n\n");
    }

    if (flag & src_wdspd) {
        printf("    - 风速 (Wind Speed) 范围: [0.0, 600.0)\n");
        printf("      单位: 米/秒 (m/s)\n");
        printf("      1999年5月3日的“桥溪摩尔”号龙卷风的最大风速为517公里/小时。\n\n");
    }

    if (flag & src_wddir) {
        if(IHATEWINDDIR){
            printf("    - 风向 (Wind Direction) 范围: [0, 359] (整数)\n");
            printf("      单位: 度 (°)\n");
            printf("      表示从正北开始顺时针转过的角度，例如: 0°为正北风, 45°为正东北风\n\n");
        }
        else {
            printf("    - 风向 (Wind Direction) 范围: 0~7 (整数)\n");
            printf("      注意！为了便于搜索，不再是角度了哦\n");//...IHATEWINDDIR（某int临时变量名称）
            printf("      0为北 1为东北 2为东 3为东南\n");
            printf("      4为南 5为西南 6为西 7为西北\n");
        }
        //我真觉得风向你要用角度式还得费力支援方向式，但是直接用方向式是不是就会好很多呢？
    }

    if (flag & src_pollv) {
        printf("    - 污染等级 (Pollution Level): [0, 5] (整数)\n");
        printf("      0=未定, 1=优, 2=良, 3=轻度, 4=中度, 5=重度污染\n\n");
    }

    if (flag & src_loca) {
        printf("    - 位置 (Location): 请输入监测点具体名称 (不含英文逗号)\n");
        printf("      建议相同的监测点采用相同的名称，这样更容易显示和理解哦！\n\n");
    }

    if (flag & src_date) {
        printf("    - 日期 (Date) 格式: YYYY-MM-DD (例如: 2024-08-05)\n");
        printf("      月份和日期不足两位也需要补'0'哦，像这样~ ( ´ ▽ ` )ﾉ\n\n");
    }

    printf("======================================================================\n");
}

void maintip() {
    printf("===== 城市环境监测数据分析与管理系统 =====\n");
    printf("by leo_grayrat & Gemini 2.5 Pro & Github Copilot\n");
    printf("1. 添加监测数据\n");
    printf("2. 显示所有数据\n");
    printf("3. 查询监测数据\n");
    printf("4. 修改监测数据\n");
    printf("5. 删除监测数据\n");
    printf("6. 排序监测数据\n");
    printf("7. 数据统计分析\n");
    printf("8. 保存数据到文件\n");
    printf("9. 从文件加载数据\n");
    printf("0. 退出系统\n");
    printf("输入对应数字进入对应板块，无需回车哦~(´•ω•`)\n");
}

#define SRC_ALL (src_id | src_loca | src_temp | src_humi | src_pm25 | src_wdspd | src_wddir | src_date | src_pollv)//见qlf_e(&new_data,SRC_ALL)那里的errorchange

int main() {
    //四个小时写完主函数，会赢吗
        //第二天：不会
    /*printf("=== 城市环境监测数据分析与管理系统 ===\n");
    printf("1. 添加监测数据\n");
    printf("2. 显示所有数据\n");
    printf("3. 查询监测数据\n");
    printf("4. 修改监测数据\n");
    printf("5. 删除监测数据\n");
    printf("6. 排序监测数据\n");
    printf("7. 数据统计分析\n");
    printf("8. 保存数据到文件\n");
    printf("9. 从文件加载数据\n");
    printf("0. 退出系统\n");
    printf("输入对应数字进入对应板块，无需回车哦~(´•ω•`)\n");
    */
    //errorchange: 提示文本应该在while里面

    int choice = -1;//给个默认失败的初始值
    //scanf("%d", &choice);
    //choice=getchar();
    //因为缓冲区残留...这些都是不会取完缓冲区的，影响之后输入
    //最简单的方法并非来自C语言本身，感谢Windows console库
        //ooc:输入一个数字都有这么多学问，programming确实不容易啊

    //ed** head = NULL;
    ed* head = NULL;
    ed* head_src = NULL;//搜索结果不能扔
	ed* head_emp = NULL;//空污染等级的数据
    /*
    再论到底head要几层指针
    因为有时要修改，有时只需要知道头在哪里

    （ai总结）
    场景一：派“侦察兵”src去侦察
    src函数的签名：ed* src(ed* head, ...)
    它的需求：它只需要知道“侦察的起点坐标”。
    你该递什么？ 你把你的head密信复印一份，把复印件交给它。
        代码：ed* results = src(head, ...);
        背后：main函数里的head的值（一个地址），被复制给了src函数的head参数。
            src函数无论怎么用这个地址去侦察，都影响不到你手里的原始密信。

    场景二：派“改装工”del去改造网络
    del函数的签名：void del(ed** head, ...)
    它的需求：它需要修改你手里那张原始密信的权限。
    你该递什么？ 你不能给它复印件了。你必须告诉它，你那张原始的head密信，存放在你司令部办公室的哪个抽屉里。
        “抽屉的地址”，就是 &head。
        head的类型是ed*。
        &head（head的地址）的类型，自然就是 ed**。
        代码：del(&head, ...);
    */

    while (1) {
        maintip();

        choice = _getch() - '0';//注意按字符处理
        if (!bet(choice, 0.0, 9.0)) {
            printf("\n\n都说了请输入0到9的数字了！真是的！(╬`д´) ノ\n");
            printf("程序生气地退出了！\n");
            /*s*/Sleep(1500);//errorchange
            printf("\n算了，请再试一次吧~ (｡･ω･｡)ﾉ");
            /*s*/Sleep(1000);//errorchange
            continue;
        }

        //下面为正常输入
        system("cls");

        if (!choice) {//0
            printf("\n感谢使用，再见啦~ (｡･ω･｡)ﾉ\n程序将在3秒后退出...");
            /*s*/Sleep(3000);
            break;
        }

        //以下为非退出的选项
            //设计的不好的地方就是，按理来说我可以先搜再批量删/改，但是由于搜索逻辑产生新链表，所以所有这种操作都不可行
            //我和链表真是一对苦命鸳鸯啊...
        switch (choice) {
        
        //很冷但是也很合理的知识：
        //如果下面这样的代码直接运行，没有{}，就会报错：标号只能是语句的一部分，而声明并非语句
        //冒号是什么意思？记得goto吗？他是不是就跳到一个生命后面，而声明就是冒号的样子，所以...
        //switch case的原理就是一个大型goto！
        //而C语言对“标号”的严格规定：一个“标号”后面，必须紧跟着一个语句
        //但是，声明并非语句，如果声明在最前，那直接完蛋，编译错误
            //我原本担心的变量在各个case语句中通用倒是不太有...不过考虑到goto逻辑...也不是不可能出这么问题？
        //但是{}就是复合语句了，那么编译器不会再过不去了

        case 1: {
            //添加监测数据
            
            ede new_data;
            memset(&new_data, 0, sizeof(ede));

            // 统一显示所有提示
            unsigned int all_fields_flag = src_loca | src_temp | src_humi | src_pm25 | src_wdspd | src_wddir | src_date | src_pollv;
            tip(all_fields_flag, 1);

            printf("\n--- 请逐项输入新的监测数据 ---\n");

            // --- 使用“活字”逐个获取输入，不再进行循环校验 ---
            get_string_input("请输入 位置 (Location): ", new_data.loca, sizeof(new_data.loca));
            get_float_input("请输入 温度 (Temperature): ", &new_data.temp);
            get_float_input("请输入 湿度 (Humidity): ", &new_data.humi);
            get_float_input("请输入 PM2.5: ", &new_data.pm25);
            get_float_input("请输入 风速: ", &new_data.wdspd);
            get_integer_input("请输入 风向: ", &new_data.wddir);
            get_string_input("请输入 日期 (YYYY-MM-DD): ", new_data.date, sizeof(new_data.date));
            get_integer_input("请输入 污染等级 (0-5): ", &new_data.pollv);

            // --- 【关键】在所有数据输入后，进行“总体验收” ---
            if (qlf_e(&new_data, SRC_ALL)) { //ede类型のqlf版本      //errorchange: 后面修改部分要求位运算存入结果，于是改了qlfe，但是这里添加的时候就绝对得全改了...需要定义一个“src_all”的宏就好了

                //ed* new_node = ins(head);//errorchange: ins需要更改head这个ed*,所以要传ed* head的地址&head
                ed* new_node = ins(&head);
                if (new_node != NULL) {
                    // 校验通过，执行复制
                    strcpy(new_node->loca, new_data.loca);
                    new_node->temp = new_data.temp;
                    new_node->humi = new_data.humi;
                    new_node->pm25 = new_data.pm25;
                    new_node->wdspd = new_data.wdspd;
                    new_node->wddir = new_data.wddir;
                    strcpy(new_node->date, new_data.date);
                    new_node->pollv = new_data.pollv;

                    printf("\n添加成功！ID为 %d 的新数据已录入~ (ﾉ>ω<)ﾉ\n", new_node->id);
                }
                else {
                    printf("\n糟糕！内存不足，添加失败了 (｡ŏ_ŏ)\n");
                }

            }
            else {
                // qlf 校验失败
                printf("\n呜哇！您输入的部分数据格式或范围不正确，添加操作已取消 (｡ŏ_ŏ)\n");
            }
            break;
        }

        case 2: {
            //显示所有数据

            ed * i = head;
            int count = 0;
            if (i == NULL) {
                printf("数据集是空的呢(ﾟдﾟ)尝试读取文件/添加节点？\n");
                break;
            }
            while (i != NULL) {
                prt(i);
                count++;
                i = i->next;
            }
            printf("显示完毕！共 %d 项数据\n", count);
            break;
        }
        case 3: {
            //查询监测数据

            //获取指令
            unsigned int flag = flg(0);
            if (flag == 0) return; // 用户取消

            //准备 & 显示提示
            eds criteria;
            memset(&criteria, 0, sizeof(eds));

            system("cls");
            //tip(flag); // 只显示用户选中的字段的提示       //DEBUG9: 忘记tip还有那个IHATEWINDDIR参数
            tip(flag, 0); // 0表示方向搜索
            
            printf("\n--- 请输入您已选中的筛选条件 ---\n");

            //按需获取输入
            if (flag & src_id) {
                printf("--- 正在设置 [ID] 范围 ---\n");
                get_integer_input("  请输入ID下界: ", &criteria.idA);
                if (!get_integer_input("  请输入ID上界 (直接回车则为单值搜索/精确匹配): ", &criteria.idB)) {
                    // 如果用户在上界处输入无效（enter也算），则默认为精确搜索
                    criteria.idB = criteria.idA;
                }
            }

            if (flag & src_loca) {
                printf("--- 正在设置 [位置] 关键词 ---\n");
                get_string_input("  请输入位置中包含的关键词: ", criteria.loca, sizeof(criteria.loca));
            }

            if (flag & src_temp) {
                printf("--- 正在设置 [温度] 范围 ---\n");
                get_float_input("  请输入最低温度: ", &criteria.tempA);
                if (!get_float_input("  请输入最高温度 (直接回车则为单值搜索/精确匹配): ", &criteria.tempB)) {
                    criteria.tempB = criteria.tempA;
                }
            }

            if (flag & src_humi) {
                printf("--- 正在设置 [湿度] 范围 ---\n");
                get_float_input("  请输入最低湿度: ", &criteria.humiA);
                if (!get_float_input("  请输入最高湿度 (直接回车则为单值搜索/精确匹配): ", &criteria.humiB)) {
                    criteria.humiB = criteria.humiA;
                }
            }

            if (flag & src_pm25) {
                printf("--- 正在设置 [PM2.5] 范围 ---\n");
                get_float_input("  请输入最低PM2.5: ", &criteria.pm25A);
                if (!get_float_input("  请输入最高PM2.5 (直接回车则为单值搜索/精确匹配): ", &criteria.pm25B)) {
                    criteria.pm25B = criteria.pm25A;
                }
            }

            if (flag & src_wdspd) {
                printf("--- 正在设置 [风速] 范围 ---\n");
                get_float_input("  请输入最低风速: ", &criteria.wdspdA);
                if (!get_float_input("  请输入最高风速 (直接回车则为单值搜索/精确匹配): ", &criteria.wdspdB)) {
                    criteria.wdspdB = criteria.wdspdA;
                }
            }

            if (flag & src_date) {
                printf("--- 正在设置 [日期] 范围 ---\n");
                get_string_input("  请输入起始日期 (YYYY-MM-DD): ", criteria.dateA, sizeof(criteria.dateA));
                get_string_input("  请输入结束日期 (单日搜索则输入相同日期): ", criteria.dateB, sizeof(criteria.dateB));
            }

            // 对于单值精确匹配的字段
            if (flag & src_wddir) {
                printf("--- 正在设置 [风向] ---\n");
                get_integer_input("  请输入风向类别 (0-北, 1-东北... 7-西北): ", &criteria.wddir);
            }

            if (flag & src_pollv) {
                printf("--- 正在设置 [污染等级] ---\n");
                get_integer_input("  请输入污染等级 (0-5): ", &criteria.pollv);
            }


            //在执行搜索前，调用 qlf_s 进行一次总体验收
            if (qlf_s(&criteria)) {
                // 校验通过，执行搜索
                //ed* head_src = src(*head, &criteria, flag);               //DEBUG9: *head还是&head傻傻分不清楚，函数参数里面要的是*head
                //ed* head_src = src(&head, &criteria, flag);               //DEBUG9_ultra: 问题没这么简单
                                                                            //看看定义ed* src(const eds* bing, ed* head, unsigned int flag)
                                                                            //1.参数前后弄反  2.criteria不是指针，所以要&一下，但head并不用
                ed* head_src = src(&criteria, head, flag);
                printf("搜索成功！您可稍后在不同选项浏览或编辑搜索到的结果_:(´□`」 ∠):_\n");
            }
            else {
                // qlf_s 校验失败
                printf("\n呜哇！您输入的范围好像有点问题 (比如下界大于上界)，搜索已取消 (｡ŏ_ŏ)\n");
            }
            break;
        }
        case 4: {
            //修改检测数据
            if (head == NULL) {
                printf("不能没有数据哦..导入或者手动添加吧(´･ω･`)\n");
                break;
            }

            printf("要修改哪个数据呢？\n");
            printf("请输入对应ID(｢･ω･)｢\n");
            printf("======================================================================\n");
            
            //为了尽量衔接之前的搜索，还是把被搜索的id给出
                //排序交换数据...会不会导致恶果？不行不敢再想了
                //不过好像并不会...因为id似乎也交换了，只有next没有交换，但是无所谓
            //if (*head_src != NULL){           //DEBUG10: 运算的时候正常处理就行，知道他是*，但不用一直带着类型
            if (head_src != NULL) {
                //之前搜过，那就展示搜到的id
                printf("※如果想修改之前的节点，下面贴心的列出了上次搜索结果的id哦ρ(・ω・))\n");
                
                int count = 0;
                ed* i = head_src;

                while (i != NULL) {
                    printf("%-4d|", i->id);
                    count++;
                    if (count < 15) {
                        i = i->next;
                    }
                    else {
                        printf("...\n");//搜出来太多也没必要全显示
                        break;
                    }
                }
            }

            //我还忘了输入...不要东打一榔头西敲一棒子
            int id_to_edit;
            if (!get_integer_input("请输入您想修改的监测点ID: ", &id_to_edit)) {
                // 如果输入的ID格式就不对，直接返回【这个好，没想到那个输入函数还可以这么使唤，用输出条件控制是否成功】
                    //不过实际上只是封装，细想下也没啥
                printf("这真的是个id吗...记得下次要输整数哦！");
                break;
            }

            system("cls");
            unsigned int flag = 0;

            //找被改节点
            ed* i = head;
            while (i->id != id_to_edit) {
                //if (i = NULL) {       advancechange: ...劲爆尾杀
                if (i == NULL) {
                    //找到头还没有找到
                    printf("哇，没有找到ID为 %d 的数据啊...是不是手误了呢？",id_to_edit);
                    break;
                }
                i = i->next;
            }
            //i就是被改的节点

            while (flag == 0) {
                printf("想要修改哪些变量呢？( ￣□￣)\n");
                /*s*/Sleep(3000);

                unsigned int flag = 0;
                flag = flg(0);//获取修改指标中,非1表示不止限一项

                if (flag == 0) {
                    printf("不能没有指标哦...再选一次吧(´･ω･`)\n");
                    /*s*/Sleep(3000);
                    system("cls");
                    continue;
                }

                system("cls");
                tip(flag, 0);//展示提示文本

                //我竟然在最后debug时候忘记了这里还没补完...

                ede update_data;
                printf("\n--- 请为ID为 %d 的节点输入以下字段的新值 ---\n", id_to_edit);

                //【使用“活字”按需获取输入】
                //检查 flag 中的每一个“开关”，只对打开的开关进行提问

                if (flag & src_loca) {
                    get_string_input("  请输入新位置: ", update_data.loca, sizeof(update_data.loca));
                }

                if (flag & src_temp) {
                    // 为了健壮性，即使是修改，也最好用循环确保输入格式正确
                    while (!get_float_input("  请输入新温度: ", &update_data.temp));
                }

                if (flag & src_humi) {
                    while (!/*get_integer_input*/get_float_input("  请输入新湿度: ", &update_data.humi));//errorchange: humi是float
                }

                // ... 对 pm25, wdspd, wddir, date, pollv 重复类似的“按需提问”逻辑 ...

                if (flag & src_pm25) {
                    while (!get_float_input("  请输入新PM2.5: ", &update_data.pm25));
                }

                if (flag & src_wdspd) {
                    while (!get_float_input("  请输入新风速: ", &update_data.wdspd));
                }

                if (flag & src_wddir) {
                    while (!get_integer_input("  请输入新风向 (0-359): ", &update_data.wddir));
                }

                if (flag & src_date) {
                    get_string_input("  请输入新日期 (YYYY-MM-DD): ", update_data.date, sizeof(update_data.date));
                }

                if (flag & src_pollv) {
                    while (!get_integer_input("  请输入新污染等级 (0-5): ", &update_data.pollv));
                }

                //最终校验与执行】
                //对新输入的数据进行合法性校验
                if (qlf_e(&update_data, flag)) { // qlf_e需要知道哪些字段是有效的           //errorchange: qlf_e是直接复制的，没有加条件判断
                    //校验通过，调用核心的 edi 函数...吗？
                    //在此之前你得先找到那个节点...
                        //准确的说，你得在输入id的时候就开始找这个节点，找不到那么直接break

                    edi(i, &update_data, flag);
                }
                else {
                    printf("\n呜哇！您输入的部分数据范围不正确，修改操作已取消 (｡ŏ_ŏ)\n");
                }
                
                break;
            }
            break;
        }
        case 5: {
            //删除监测数据
            printf("按Enter清空...\n");
            printf("如果想反悔，就按任意不是Enter键的键_(:3 ⌒ﾞ)_\n");

            int key = _getch();
            if (key == 13) {
                printf("\n确认清空请按 y 键！请三思！\n");
                key = _getch();
                if (key == 'y') {
                    ed* i = head;
                    int count = 0;
                    if (i == NULL) {
                        printf("数据集本来就是空的呢(ﾟдﾟ)\n");
                        break;
                    }
                    while (i != NULL) {

                        ed* nextoneadvancechange = i->next;

                        del(&head, i->id);
                        count++;
                        //i = i->next;      advancechange: 你已经没有i了，得保存一份

                        i = nextoneadvancechange;
                    }
                    //printf("删除完成，共清空 %d 条数据...加载新数据吧！_(:3 ⌒ﾞ)_\n");
                    printf("删除完成，共清空 %d 条数据...加载新数据吧！_(:3 ⌒ﾞ)_\n",count);//errorchange 少了变量
                }
            }
            //printf("如果想按条件清空的话...请使用查询功能\n");
                //这里一细想就会有很恐怖的代码量...因为src得到的是另一串链表...算了
            break;
        }
        case 6: {
            //排序监测数据
            if (/***/head == NULL) {            //DEBUG12: 类似10
                printf("数据集是空的呢(ﾟдﾟ)\n");
                break;
            }

            unsigned int flag = 0;//DEBUG13改

            while (flag == 0) {         
                printf("想要以哪个变量为指标来排序呢？( ￣□￣)\n");
                printf("请注意：之后选择时只能选择一项哦(`・ω・´)\n");
                /*s*/Sleep(2500);
                printf("    悄悄话：即使你想选多项...也做不到哦(`へ´≠)\n");
                /*s*/Sleep(500);
                
                //unsigned int flag = 0;        //DEBUG13: flag初始化得在外边
                flag = flg(1);//获取排序指标中

                if (flag == 0) { 
                    printf("不能没有指标哦...再选一次吧\n");
                    /*s*/Sleep(3000);
                    system("cls");
                    continue;
                }

                system("cls");

                //支援搜索后排序
                ed** target = &head;
                if (/***/head_src != NULL) {        //DEBUG11: 类似10
                    printf("最后一步！是想对全体进行排序，还是想对上次搜索结果排序呢？");
                    printf("按Enter键对搜索结果排序，按其他任意键对全体数据排序！");
                    int key = _getch();
                    if (key == 13) {
                        target = &head_src;
                    }
                }
                //如果需要检查就询问，如果同意走A，不同意或者不需要检查走B
                printf("严肃排序中...\n");
                ord(target, flag);//排序

                printf("排序成功！之后查看数据的话，排序就是你想要的样子啦(╯✧∇✧)╯\n");
                break;
            }
        }
        case 7: {
			//朴素贝叶斯统计分析
            if(head==NULL){
                printf("数据集是空的呢(ﾟдﾟ)\n");
                break;
			}

			printf("接下来将利用朴素贝叶斯方法对未知污染等级的数据进行预测哦(｡･ω･｡)ﾉ♡\n");
			printf("会自动读取当前数据中的已知污染等级数据进行训练~\n");
			printf("如果要预测的数据不在当前数据集中，请先导入数据或手动添加数据哦~\n");
            printf("=======================================================================================================\n");

			//找到所有未知污染等级的数据
            {
				ed* i = head;
                while(i != NULL){
                    if(i->pollv == 0){
						//head_emp = ins(&head_emp);//插入到空污染数据链表中
                        //你要干啥，还记得怎么插节点吗，只动头有何意义，数据呢，还得复制啊

                        ed* new = ins(&head_emp); // 新节点已分配并插到 head_emp
                        if (new != NULL) {
                            // 拷贝字段
                            strncpy(new->loca, i->loca, sizeof(new->loca) - 1); new->loca[sizeof(new->loca) - 1] = '\0';
                            new->temp = i->temp;
                            new->humi = i->humi;
                            new->pm25 = i->pm25;
                            new->wdspd = i->wdspd;
                            new->wddir = i->wddir;
                            strncpy(new->date, i->date, sizeof(new->date) - 1); new->date[sizeof(new->date) - 1] = '\0';
                            new->pollv = i->pollv; // 应为0，但拷贝以保持一致
                        }
                    }
                    i = i->next;
				}
            }
            if (head_emp == NULL) {
				printf("当前没有未知污染等级的数据呢(ﾟдﾟ)\n");
                break;
            }
            sta(head,head_emp);
            break;
        }
        case 8: {
            //保存数据到文件
            sav(head);
            break;
        }
        case 9: {
            //从文件加载数据
            red(&head);
			printf("如果出现乱码，请将源文件用ANSI编码保存后再试一次哦~\n");
            break;
        }

        default: {
            printf("嘶...你来到了未知领域！送你回去吧(^_^)/~~\n");
            break;
        }
        }

        printf("按ESC键返回主菜单...\n");
        {
            char key;
            do {
                key = _getch();  // 获取按键，不显示在屏幕上
            } while (key != 27);  // 27是ESC键的ASCII码
        }
        system("cls");
    }

    //rls(head);//释放内存        //ERROR1:head和head_src都是指针，这个函数需要二级指针
    //rls(head_src);
    rls(&head);//释放内存        
    rls(&head_src);
	rls(&head_emp);
    return 0;
}
