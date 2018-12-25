#include<iostream>
#include<easyx.h>
#include<graphics.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<string>
#include<cmath>
#include<fstream>
#include<stdlib.h>
#include<algorithm>
//#include<mmsystem.h>
//#pragma comment(lib,"WINMM.lib")
using namespace std;

//宏定义													////////////////////////////
#define MAXSTAR					300						//星星总数
#define NUM						140						//玩家+子弹+敌人+道具 总数
#define SWIDTH					1080					//屏幕宽度
#define SHEIGTHT				640						//屏幕高度
#define PLANE_W					70						//玩家宽度
#define PLANE_H					70						//玩家高度
#define ENEMY_W					40						//敌人宽度--道具宽度
#define ENEMY_H					40						//敌人高度--道具高度
#define BULLET_W				15						//子弹宽度
#define BULLET_H				15						//子弹高度
#define PLANESPEED				18						//飞机移动速度
#define HPHELP					30						//道具恢复生命数
#define HPMAX					200						//生命值上限
#define HPDOWN					40						//敌机碰撞生命损失
#define SCOREUP					25						//正常加分
#define SCOREBURST				150						//大加分
#define SCOREDOWN				100						//分数下降

//全局变量声明											///////////////////////////
typedef struct object {
	int x, y;
	int type;
}object;

struct STAR
{
	double	x;
	int		y;
	double	step;
	int		color;
};
STAR star[MAXSTAR];

struct point
{
	char name[100];
	int date;
};

int score;
IMAGE img[16];
char p[15];

//函数声明								///////////////////////////////////////
int menu();								//绘制菜单界面
bool box();
void ifsave();							//绘制保存得分/是否保存界面
bool ifrestart();						//绘制是否重新开始界面
void readrank();						//绘制得分查询界面
void load();							//加载图片资源
void InitStar(int);						//背景星空绘制
void MoveStar(int);						//背景星空绘制
char *numtostr(int);					//数字转字符串，用于打印得分/生命值
int cmp(point, point);

//Game类									///////////////////////////////////////
class Game
{
public:
	//变量声明							///////////////////////////////////////
	object players[NUM];				//player  bullets  enemys  ...    helps
	int NumberOfEnemy;					//敌人数目
	int NumberOfBullet;					//子弹数目
	int NumberOfHelp;					//道具数目
	int NowEnemy;						//敌人数目变化参数
	int NowHelp;						//道具数目变化参数
	bool death;							//标记死亡	1 存活	0 死亡
	int cnt;

	//函数声明							///////////////////////////////////////
	void initplane();					//初始化玩家
	void initenemy();					//初始化敌人
	void initbullet();					//初始化子弹
	void inithelp();					//初始化道具
	void newenemy();					//调整敌人时初始化
	void newhelp();						//调整道具时初始化

	void move_plane(char);				//玩家移动(char)
	void move_plane(int);				//玩家移动(int)
	void move_enemy();					//敌人移动
	void move_bullet();					//子弹移动
	void move_help();					//道具移动


	void drawall();						//游戏画面绘制

	void init();						//游戏初始化
	void pause();						//暂停
	void playing();						//游戏主进程
	void endgame();						//死亡界面绘制
	void printhelp();					//暂停时绘制帮助界面

	bool judge(object,object);			//碰撞检测(object enemy_or_help,object player_or_bullet)

	void shoot();						//发射子弹
};

//函数定义								///////////////////////////////////////

void Game::playing()
{
	int bullet_speed = 1;				//子弹速度
	int enemy_speed = 2;				//敌人速度
	int help_speed = 2;					//道具速度
	int bullet = 0;						//将要发出的子弹数目
	int bulletrate = 30;				//子弹频率
	//count计数变量		////
	int countrate = 0;
	int count_bullet = 0;
	int count_enemy = 0;
	int count_help = 0;
	////////////////////////
	while (death)
	{
		//player move					///////////////////////////////////////玩家移动及按键检测
		if (_kbhit()) 
		{
			char x = _getch();
			switch (x)
			{
			case 'w':
			case 's':
			case 'a':
			case 'd':
				move_plane(x); break;
			case 72:
			case 80:
			case 75:
			case 77:
				move_plane((int)x); break;
			case ' ':
				bullet++;
				break;
			case 27:
				printhelp();
				pause();
			default:break;
			}
		//	pause();
		}

		//enemy move					///////////////////////////////////////敌人移动及碰撞检测
		if (count_enemy == enemy_speed)
		{
			count_enemy = 0;
			move_enemy();
			for (int i = NumberOfBullet + 1; i <= NumberOfBullet + NumberOfEnemy; i++)
			{
				if (players[i].x == -1 && players[i].y == -1)
				{
					continue;
				}
				if (judge(players[i], players[0]))
				{/*
					players[0].x = -1;
					players[0].y = -1;*/
					players[i].x = -1;
					players[i].y = -1;
					players[0].type -= HPDOWN;
					score -= SCOREDOWN;
					if (score < 0)
					{
						score = 0;
					}
				}
			}
		}
		count_enemy++;

		//help move						///////////////////////////////////////道具移动及碰撞检测
		if (count_help == help_speed)
		{
			count_help = 0;
			move_help();
			for (int i = NUM - 1; i >= NUM - NumberOfHelp; i--)
			{
				if (players[i].x == -1 && players[i].y == -1)
				{
					continue;
				}
				if (judge(players[i], players[0]))
				{/*
					players[0].x = -1;
					players[0].y = -1;*/
					players[i].x = -1;
					players[i].y = -1;
					players[0].type += HPHELP;
					if (players[0].type > HPMAX)
					{
						players[0].type = HPMAX;
					}
				}
			}
		}
		count_help++;

		//rate_control
		if (bullet > 2)
		{
			bullet = 1;
		}
		if (bullet > 0)
		{
			if (countrate == bulletrate)
			{
				shoot();
				bullet--;
				countrate = 0;
			}
			countrate++;
		}
		else
		{
			countrate = 0;
		}

		//bullet move					///////////////////////////////////////子弹移动及碰撞检测
		if (count_bullet == bullet_speed)
		{
			count_bullet = 0;
			move_bullet();
			for (int i = 1; i <= NumberOfBullet; i++)
			{
				if (players[i].x == -1 && players[i].y == -1)
				{
					continue;
				}
				for (int j = NumberOfBullet + 1; j <= NumberOfBullet + NumberOfEnemy; j++)
				{
					if (players[j].x == -1 && players[j].y == -1)
					{
						continue;
					}
					//printf("in playing %d", players[i].type);
					if (judge(players[j], players[i]))
					{
						players[j].x = -1;
						players[j].y = -1;
						players[i].x = -1;
						players[i].y = -1;
						score += SCOREUP;
					}
				}
				for (int j = NUM - 1; j >=NUM - NumberOfHelp; j--)
				{
					if (players[j].x == -1 && players[j].y == -1)
					{
						continue;
					}
					//printf("in playing %d", players[j].type);
					if (judge(players[j], players[i]))
					{
						players[j].x = -1;
						players[j].y = -1;
						players[i].x = -1;
						players[i].y = -1;
						score += SCOREBURST;
					}
				}
			}
		}
		count_bullet++;

		//if_live?						///////////////////////////////////////玩家存活检测
		if (players[0].type <= 0)
		{
			death = 0;
		}

		//number_change					///////////////////////////////////////敌人数目及道具数目随得分增加调整
		if (NumberOfEnemy + NumberOfHelp + NumberOfBullet + 1 < NUM)
		{
			NowEnemy = (int)log((double)(score / 75) + 1) + 7;
			NowHelp = (int)log((double)(score / 200) + 1) + 1;
			if (NowEnemy + NowHelp + NumberOfBullet + 1 <= NUM)
			{
				if (NowEnemy > NumberOfEnemy)
				{
					newenemy();
					NumberOfEnemy = NowEnemy;
				}
				if (NowHelp > NumberOfHelp)
				{
					newhelp();
					NumberOfHelp = NowHelp;
				}
			}
			else
			{
				NowEnemy = NumberOfEnemy;
				NowHelp = NumberOfHelp;
			}
		}

		//draw							///////////////////////////////////////屏幕绘制
		drawall();
		Sleep(1);
	}
}

void Game::drawall()
{
	cleardevice();
	BeginBatchDraw();
	//draw stars
	/*
	static int countstar = 0;
	int starrate = 15;
	if (countstar == starrate)
	{
		static int k = 0;
		printf("%d\n",k++);
		for (int i = 0; i < MAXSTAR; i++)
			MoveStar(i);
		countstar = 0;
	}
	else
	{
		for (int i = 0; i < MAXSTAR; i++)
			putpixel((int)star[i].x, star[i].y, star[i].color);
	}
	countstar++;

	*/
	for (int i = 0; i < MAXSTAR; i++)
		MoveStar(i);
	//draw enemy
	for (int i = NumberOfBullet + 1; i <= NumberOfBullet + NumberOfEnemy; i++)
	{
		if (players[i].x != -1 && players[i].y != -1)
		{
			putimage(players[i].x, players[i].y, &img[4], SRCAND);
			putimage(players[i].x, players[i].y, &img[3], SRCPAINT);
		}
	}
	//draw help

	for (int i = NUM - 1; i >= NUM - NumberOfHelp; i--)
	{
		if (players[i].x != -1 && players[i].y != -1)
		{
			putimage(players[i].x, players[i].y, &img[10], SRCAND);
			putimage(players[i].x, players[i].y, &img[9], SRCPAINT);
		}
	}

	//draw player
	if (players[0].type >= 0)
	{
		putimage(players[0].x, players[0].y, &img[2], SRCAND);
		putimage(players[0].x, players[0].y, &img[1], SRCPAINT);
	}
	//draw bullet
	for (int i = 1; i <= NumberOfBullet; i++)
	{
		if (players[i].x != -1 && players[i].y != -1 && players[0].type > 0)
		{
			putimage(players[i].x, players[i].y, &img[6], SRCAND);
			putimage(players[i].x, players[i].y, &img[5], SRCPAINT);
		}
	}
	//draw score
	RECT r[8] = { {60,0,120,30} ,{120,0,180,30},{200,0,260,30}, {260,0,320,30}, {360,0,720,30}, {780,0,810,30},{810,0,815,30},{815,0,855,30} };
	char *s;
	char *life;
	if (players[0].type > 0)
	{
		life = numtostr(players[0].type);
		drawtext(_T("HP:"), &r[2], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(_T(life), &r[3], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		drawtext(_T("HP:"), &r[2], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		drawtext(_T(" 0 "), &r[3], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	s = numtostr(score);
	drawtext(_T("得分:"), &r[0], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T(s), &r[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("按ESC以暂停/获得帮助"), &r[4], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	if (death == 1)
	{
		cnt+=5;
		char *second;
		char *milesecond;
		if (cnt < 1000)
		{
			second = numtostr(0);
			drawtext(_T(second), &r[5], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			milesecond = numtostr((int)(cnt / 100));
			drawtext(_T(milesecond), &r[7], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		else
		{
			second = numtostr((int)(cnt / 1000));
			drawtext(_T(second), &r[5], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			milesecond = numtostr((int)((cnt / 100) % 10));
			drawtext(_T(milesecond), &r[7], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		
		drawtext(_T("."), &r[6], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		

	}
	EndBatchDraw();
}

void Game::init()
{
	int i;
	//初始化星星
	for (i = 0; i < MAXSTAR; i++)
	{
		InitStar(i);
		star[i].x = rand() % SWIDTH;
	}
	//游戏初始化数据
	NumberOfEnemy = 7;
	NumberOfBullet = 25;
	NumberOfHelp = 1;
	NowEnemy = 7;
	NowHelp = 1;
	score = 600;
	initplane();
	initbullet();
	initenemy();
	inithelp();
	death = 1;
	cnt = 1;
}

void Game::initplane()
{
	//player	0
	players[0].x = 0 + rand() % 100;
	players[0].y = SHEIGTHT / 2;
	players[0].type = 100;
}

void Game::initenemy()
{
	int i;
	//enemy		2
	for (i = NumberOfBullet + 1; i <= NumberOfBullet + NumberOfEnemy; i++) {
		players[i].x = rand() % 400 + SWIDTH;
		players[i].y = rand() % (SHEIGTHT - PLANE_H - 30) + PLANE_H / 2 - ENEMY_H / 2 + 30;
		printf("%d\n", players[i].y);
		players[i].type = 2;
	}
}

void Game::inithelp()
{
	int i;
	//help		3
	for (i = NUM - 1; i >= NUM -NumberOfHelp; i--) {
		players[i].x = rand() % 400 + SWIDTH;
		players[i].y = rand() % (SHEIGTHT - PLANE_H - 30) + PLANE_H / 2 - ENEMY_H / 2 + 30;
		printf("---%d\n", players[i].y);
		players[i].type = 3;
	}
}

void Game::initbullet()
{
	int i;
	//bullet	1
	for (i = 1; i <= NumberOfBullet; i++)
	{
		players[i].x = -1;
		players[i].y = -1;
		players[i].type = -1;
	}
}

void Game::newenemy()
{
	for (int i = NumberOfBullet + NumberOfEnemy + 1; i <= NumberOfBullet + NowEnemy; i++)
	{
		players[i].x = rand() % 400 + SWIDTH;
		players[i].y = rand() % (SHEIGTHT - PLANE_H - 30) + PLANE_H / 2 - ENEMY_H / 2 + 30;
		printf("new_enemy->%d\n", players[i].y);
		players[i].type = 2;
	}
}

void Game::newhelp()
{
	for (int i = NUM - NumberOfHelp - 1; i >= NUM - NowHelp; i--)
	{
		players[i].x = rand() % 400 + SWIDTH;
		players[i].y = rand() % (SHEIGTHT - PLANE_H - 30) + PLANE_H / 2 - ENEMY_H / 2 + 30;
		printf("new_help->%d\n", players[i].y);
		players[i].type = 3;
	}
}

void Game::move_plane(char x)
{
	int dir[4][2] = { {0,-PLANESPEED},{0,PLANESPEED},{-PLANESPEED,0},{PLANESPEED,0} };//w 0//s 1//a 2//d 3
	int dirx=0;
	switch (x)
	{
	case 'w':dirx = 0; break;
	case 's':dirx = 1; break;
	case 'a':dirx = 2; break;
	case 'd':dirx = 3; break;
	}
	players[0].x += dir[dirx][0];
	players[0].y += dir[dirx][1];
	if (players[0].x > SWIDTH - PLANE_W) 
	{
		players[0].x = SWIDTH - PLANE_W;
	}
	if (players[0].x < 0 )
	{
		players[0].x = 0;
	}
	if (players[0].y > SHEIGTHT - PLANE_H)
	{
		players[0].y = SHEIGTHT - PLANE_H;
	}
	if (players[0].y < 30 )
	{
		players[0].y = 30;
	}
}

void Game::move_plane(int x)
{
	int dir[4][2] = { {0,-PLANESPEED},{0,PLANESPEED},{-PLANESPEED,0},{PLANESPEED,0} };//w 0//s 1//a 2//d 3
	int dirx = 0;
	switch (x)
	{
	case 72:dirx = 0; break;
	case 80:dirx = 1; break;
	case 75:dirx = 2; break;
	case 77:dirx = 3; break;
	}
	players[0].x += dir[dirx][0];
	players[0].y += dir[dirx][1];
	if (players[0].x > SWIDTH - PLANE_W)
	{
		players[0].x = SWIDTH - PLANE_W;
	}
	if (players[0].x < 0)
	{
		players[0].x = 0;
	}
	if (players[0].y > SHEIGTHT - PLANE_H)
	{
		players[0].y = SHEIGTHT - PLANE_H;
	}
	if (players[0].y < 30)
	{
		players[0].y = 30;
	}
}

void Game::move_bullet()
{
	for (int i = 1; i <= NumberOfBullet; i++)
	{
		if (players[i].x == -1 && players[i].y == -1)
		{
			continue;
		}
		players[i].x += 5;
		if (players[i].x > SWIDTH)
		{
			players[i].x = -1;
			players[i].y = -1;
		}
	}
}

void Game::move_enemy()
{
	for (int i = NumberOfBullet + 1; i <= NumberOfBullet + NumberOfEnemy; i++)
	{
		if (players[i].x == -1 && players[i].y == -1 && players[i].type == 2)
		{
			players[i].x = rand() % 400 + SWIDTH;
			players[i].y = rand() % (SHEIGTHT - PLANE_H - 30) + PLANE_H / 2 - ENEMY_H / 2 + 30;
		}
		else
		{
			players[i].x -= 4;
			if (players[i].x < -ENEMY_W)
			{
				players[i].x = -1;
				players[i].y = -1;
			}
		}
	}

}

void Game::move_help()
{
	for (int i = NUM - 1; i >= NUM - NumberOfHelp; i--)
	{
		if (players[i].x == -1 && players[i].y == -1 && players[i].type == 3)
		{
			players[i].x = rand() % 400 + SWIDTH;
			players[i].y = rand() % (SHEIGTHT - PLANE_H - 30) + PLANE_H / 2 - ENEMY_H / 2 + 30;
		}
		else
		{
			players[i].x -= 3;
			if (players[i].x < -ENEMY_W)
			{
				players[i].x = -1;
				players[i].y = -1;
			}
		}
	}

}

void Game::shoot()
{
	for (int i = 1; i <= NumberOfBullet; i++)
	{
		if (players[i].x == -1 && players[i].y == -1 && players[i].type == -1 )
		{
			players[i].x = players[0].x + PLANE_W;
			players[i].y = players[0].y + PLANE_H / 2-BULLET_H/2-1;
			break;
		}
	}
}

void Game::pause()
{
	char c = _getch();
	while (c != 27 && c != ' ' && c != 101)
		c = _getch();
	if (c == 101)
	{
		death = 0;
	}
}

bool Game::judge(object enemy, object porb)
{
	//printf("in judge %d", porb.type);
	if (porb.type >= 0)
	{
		int enemy_x = enemy.x + ENEMY_W / 2;
		int enemy_y = enemy.y + ENEMY_H / 2;
		int plane_x = porb.x;
		int plane_y = porb.y;
		if (enemy_x - plane_x <= ENEMY_W / 2 && enemy_x - plane_x >= -(ENEMY_W / 2) && enemy_y >= plane_y - ENEMY_W / 2 && enemy_y <= plane_y + PLANE_H - 1 + ENEMY_W / 2)
		{
			return 1;
		}
		else if (enemy_y >= plane_y && enemy_y <= (plane_y + PLANE_H / 2 - 1) && enemy_x - ENEMY_W <= plane_x + 2 * (enemy_y - plane_y) && enemy_x + ENEMY_W >= plane_x + 2 * (enemy_y - plane_y))
		{
			return 1;
		}
		else if (enemy_y >= plane_y + PLANE_H / 2 - 1 && enemy_y <= (plane_y + PLANE_H - 1) && enemy_x - ENEMY_W <= plane_x + 2 * (plane_y + PLANE_H - 1 - enemy_y) && enemy_x + ENEMY_W >= plane_x + 2 * (plane_y + PLANE_H - 1 - enemy_y))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (porb.type == -1)
	{
		int bullet_x = porb.x+BULLET_W/2;
		int bullet_y = porb.y+BULLET_H/2;
		int enemy_x = enemy.x + ENEMY_W / 2;
		int enemy_y = enemy.y + ENEMY_H / 2;
		if ((bullet_x - enemy_x)*(bullet_x - enemy_x) + (bullet_y - enemy_y)*(bullet_y - enemy_y) <= (BULLET_W / 2 + ENEMY_W / 2)*(BULLET_W / 2 + ENEMY_W / 2))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void Game::endgame()
{
	int times = 30;
	drawall();
	while (times--)
	{
		Sleep(100);
		drawall();
	}
	BeginBatchDraw();
	putimage(0, 0, &img[8], SRCAND);
	putimage(0, 0, &img[7], SRCPAINT);
	EndBatchDraw();
	while (getchar() != '\n');
}

void Game::printhelp()
{
	putimage(0, 0, &img[12], SRCAND);
	putimage(0, 0, &img[11], SRCPAINT);
}

int menu()
{
	int nowj;//当前选项框所在位置右下角纵坐标
	RECT r[3] = { {661, 300, SWIDTH, 370},{661, 370, SWIDTH, 440},{661, 440, SWIDTH, 510} };
	putimage(0, 0, &img[13]);
	settextstyle(28, 0, _T("微软雅黑"));
	nowj = 0;
	drawtext(_T("> 开始游戏PLAY <"), &r[0], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("查看排名READ"), &r[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext(_T("退出游戏EXIT"), &r[2], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//	settextstyle(32, 0, _T("Consolas"));
//	outtextxy(0, 0, _T("AB测试"));
	while (1)
	{
		if (_kbhit())
		{
			char x = _getch();
			if (x < 0)
				x = _getch();
			switch (x) {
			case 'w':
			case 72:
				if (nowj == 1)
				{
					nowj = 0;
					drawtext(_T("> 开始游戏PLAY <"), &r[0], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					drawtext(_T("     查看排名READ     "), &r[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else if (nowj == 2)
				{
					nowj = 1;
					drawtext(_T("> 查看排名READ <"), &r[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					drawtext(_T("     退出游戏EXIT     "), &r[2], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				printf("W or Up is pressed\n"); 
				break;
			case 's':
			case 80:
				if (nowj == 0)
				{
					nowj = 1;
					drawtext(_T("     开始游戏PLAY     "), &r[0], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					drawtext(_T("> 查看排名READ <"), &r[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				else if (nowj == 1)
				{
					nowj = 2;
					drawtext(_T("     查看排名READ     "), &r[1], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					drawtext(_T("> 退出游戏EXIT <"), &r[2], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
				printf("S or Down is pressed\n"); 
				break;
			case 'k':
			case 32:
			case 13:
				return nowj + 1;
			default :
				printf("%d", x);
				break;
			}
		}
	}
}

void load()
{
	//预加载
	loadimage(&img[0], _T("./pic/openimg.jpg"), SWIDTH, SHEIGTHT);
	loadimage(&img[1], _T("./pic/plane.png"), PLANE_W, PLANE_H);
	loadimage(&img[2], _T("./pic/planemask.png"), PLANE_W, PLANE_H);
	loadimage(&img[3], _T("./pic/enemy.png"), ENEMY_W, ENEMY_H);
	loadimage(&img[4], _T("./pic/enemymask.png"), ENEMY_W, ENEMY_H);
	loadimage(&img[5], _T("./pic/bullet.png"), BULLET_W, BULLET_H);
	loadimage(&img[6], _T("./pic/bulletmask.png"), BULLET_W, BULLET_H);
	loadimage(&img[7], _T("./pic/death.png"), SWIDTH, SHEIGTHT);
	loadimage(&img[8], _T("./pic/deathmask.png"), SWIDTH, SHEIGTHT);
	loadimage(&img[9], _T("./pic/help.png"), ENEMY_W, ENEMY_H);
	loadimage(&img[10], _T("./pic/helpmask.png"), ENEMY_W, ENEMY_H);
	loadimage(&img[11], _T("./pic/pause.png"), SWIDTH, SHEIGTHT);
	loadimage(&img[12], _T("./pic/pausemask.png"), SWIDTH, SHEIGTHT);
	loadimage(&img[13], _T("./pic/background.png"), SWIDTH, SHEIGTHT);
	loadimage(&img[14], _T("./pic/rankbg.png"), SWIDTH, SHEIGTHT);
	putimage(0, 0, &img[0]);
	getchar();
	cleardevice();
}

void readrank()
{
	int i, j, k, row;
	char rankstr[100];
	char name[100];
	int data;
	char *ntos;
	cleardevice();
	putimage(0, 0, &img[14]);
	char buffer[256];
	ifstream myfile("./save.txt");
	if (!myfile)
	{
		cout << "Unable to open myfile";
		exit(1); // terminate with error
	}
	i = 750;
	j = 840;
	k = 910;
	row = 190;
	while (!myfile.eof())
	{
		myfile.getline(buffer, 40);
		if (buffer[0] == '\0')
		{
			break;
		}
		sscanf_s(buffer, "%[^-]-%d-%s", rankstr, (unsigned int)sizeof(rankstr), &data, name, (unsigned int)sizeof(name));
		outtextxy(i, row, rankstr);
		ntos = numtostr(data);
		outtextxy(j, row, ntos);
		outtextxy(k, row, name);
		row = row + 30;
	}
	myfile.close();
	char c = _getch();
	while (c != 13 && c != ' ')
		c = _getch();
}

bool box()
{
	int now;
	now = 0;
	while (1)
	{
		if (_kbhit())
		{
			char x = _getch();
			if (x == 'y')
			{
				return 1;
			}
			else if (x == 'n')
			{
				return 0;
			}
		}
	}
}

void ifsave()
{
	point ranks[11];
	char buffer[256];
	char str[100];
	int k = 0;
	char name[100];
	char rank[10][100] = { {"No.01"},{"No.02"},{"No.03"},{"No.04"},{"No.05"},{"No.06"},{"No.07"},{"No.08"},{"No.09"},{"No.10"} };
	ifstream myfile("./save.txt");
	if (!myfile)
	{
		cout << "Unable to open myfile";
		exit(1); // terminate with error
	}
	
	while (!myfile.eof())
	{
		myfile.getline(buffer, 40);
		if (buffer[0] == '\0')
		{
			break;
		}
		sscanf_s(buffer, "%[^-]-%d-%s", str, (unsigned int)sizeof(str), &ranks[k].date, ranks[k].name, (unsigned int)sizeof(ranks[k].name));
		k++;
	}
	myfile.close();
	readrank();
	if (k == 0)
	{
		if (score == 0)
		{
			//抱歉，您的分数过低，无法计入排行榜
			printf("sorry low score");
		}
		else
		{
			if (box())
			{
				InputBox(name, 100, "请输入你的名字");
				ofstream yourfile("./save.txt");
				if (!yourfile)
				{
					cout << "Unable to open myfile";
					exit(1); // terminate with error
				}
				yourfile << "first" << "-" << score << "-" << name;
				yourfile.close();
				readrank();
			}
		}
	}
	else if(k < 10)
	{
		if (score == 0)
		{
			//分数太低，无法计入
			printf("sorry low score");
		}
		else
		{
			if (box())
			{
				InputBox(name, 100, "请输入你的名字");
				ranks[k].date = score;
				strcpy_s(ranks[k].name, name);
				sort(ranks, ranks + k + 1, cmp);
				ofstream yourfile("./save.txt");
				for (int i = 0; i <= k; i++)
				{
					if (i < k) yourfile << rank[i] << "-" << ranks[i].date << "-" << ranks[i].name << endl;
					else yourfile << rank[i] << "-" << ranks[i].date << "-" << ranks[i].name;
				}
				yourfile.close();
				readrank();
			}
		}
	}
	else
	{
		if (score <= ranks[9].date)
		{
			//分数太低，无法计入
			printf("sorry low score");
		}
		else
		{
			if (box())
			{
				InputBox(name, 100, "请输入你的名字");
				ranks[k].date = score;
				strcpy_s(ranks[k].name, name);
				sort(ranks, ranks + k + 1, cmp);
				ofstream yourfile("./save.txt");
				for (int i = 0; i <= k; i++)
				{
					if (i < k) yourfile << rank[i] << "-" << ranks[i].date << "-" << ranks[i].name << endl;
					else yourfile << rank[i] << "-" << ranks[i].date << "-" << ranks[i].name;
				}
				yourfile.close();
				readrank();
			}
		}
	}
}

char *numtostr(int n)
{
	int m = n;
	int j = 0;
	if (n == 0)
	{
		p[0] = '0';
		p[1] = '\0';
		return p;
	}
	while (n != 0)
	{
		j++;
		n = n / 10;
	}
	j--;
	n = j;
	while (m != 0)
	{
		p[j--] = m % 10+'0';
		m = m / 10;
	}
	p[n + 1] = '\0';
	return p;
}

int cmp(point a, point b)
{
	return a.date > b.date;
}

/////////////////////////stars

// 初始化星星
void InitStar(int i)
{
	star[i].x = SWIDTH;
	star[i].y = rand() % SHEIGTHT;
	star[i].step = (rand() % 5000) / 1000.0 + 1;
	star[i].color = (int)(star[i].step * 255 / 6.0 + 0.5);	// 速度越快，颜色越亮
	star[i].color = RGB(star[i].color, star[i].color, star[i].color);
}

// 移动星星
void MoveStar(int i)
{
	// 计算新位置
	star[i].x -= star[i].step;
	if (star[i].x < 0)	InitStar(i);

	// 画新星星
	putpixel((int)star[i].x, star[i].y, star[i].color);
}

//////////////////////////////

/////////////////////////////////////////////////////////////////////
int main()
{
	int choose;
	srand((unsigned)time(NULL));
	Game game;
	printf("测试");
	initgraph(SWIDTH, SHEIGTHT, SHOWCONSOLE);
	load();
	//	getchar();
	while (1)
	{
		choose = menu();
		if (choose == 1)
		{
				while (1) {
					game.init();
					game.playing();
					game.endgame();
					ifsave();
					/*
					ifsave();
					if (!ifrestart()) {
						break;
					}*/
				}
		}
		else if (choose == 2)
		{
				readrank();
		}
		else if (choose == 3)
		{
			break;
		}
	}
	closegraph();
	return 0;
}



///////////////////////////////////////////
/*



if (_kbhit())
		{
			char x = _getch();
			if (x < 0)
				x = _getch();
			switch (x) {
			case 'w':
			case 72:
				printf("wwwwww"); break;
			case 's':
			case 80:
				printf("ssssss"); break;
			case 'a':
			case 75:
				printf("aaaaaa"); break;
			case 'd':
			case 77:
				printf("dddddd"); break;
			case 'k':
			case 32:
				printf("space!"); break;
			default :
				printf("%d", x);
			}
		}














*/