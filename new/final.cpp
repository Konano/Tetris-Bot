#include <cstdio>
#include <algorithm>
#include <cstring>
#include <queue>
#include <bitset>
#include <cmath>
#include <set>
#include <map>
#include <iostream>
#include <cstdlib>
#include <ctime>
 
using namespace std;
 
#define rep(i, l, r) for(int i=l; i<=r; i++)
#define dow(i, l, r) for(int i=l; i>=r; i--)
#define clr(x, c) memset(x, c, sizeof(x))
#define travel(x) for(edge *p=fir[x]; p; p=p->n)
#define all(x) (x).begin(),(x).end()
#define pb push_back
#define fi first
#define se second
#define l(x) Left[x]
#define r(x) Right[x]
#define lowbit(x) (x&-x)
 
typedef long long ll;
typedef pair<int,int> Pii;
typedef pair<ll,int> Pli;
typedef pair<double,int> Pdi;
typedef long double ld;
typedef unsigned long long ull;

int ED;

// 注意：x的范围是1~MAPWIDTH，y的范围是1~MAPHEIGHT
// 数组是先行（y）后列（c）
// 坐标系：原点在左下角
 
#define MAPWIDTH 10
#define MAPHEIGHT 20
 
// 先y后x，记录地图状态，0为空，1为以前放置，2为刚刚放置，负数为越界
// （2用于在清行后将最后一步撤销再送给对方）
int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2];  // backup
 
// 代表分别向对方转移的行
int trans[2][6][MAPWIDTH + 2];  // backup
 
// 转移行数
int transCount[2];  // backup
 
// 运行eliminate后的当前高度
int maxHeight[2];  // backup
 
// 总消去行数的分数之和
int elimTotal[2];  // backup

// 连续几回合发生过消去了
int elimCombo[2];  // backup
 
// 一次性消去行数对应分数
const int elimBonus[] = { 0, 1, 3, 5, 7 }; 
 
// 给对应玩家的各类块的数目总计
int typeCountForColor[2][7]; // backup
 
const int blockShape[7][4][8] = {
	{ { 0,0,1,0,-1,0,-1,-1 },{ 0,0,0,1,0,-1,1,-1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,-1,1 } },
	{ { 0,0,-1,0,1,0,1,-1 },{ 0,0,0,-1,0,1,1,1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,1,0,-1,-1,-1 } },
	{ { 0,0,1,0,0,-1,-1,-1 },{ 0,0,0,1,1,0,1,-1 },{ 0,0,-1,0,0,1,1,1 },{ 0,0,0,-1,-1,0,-1,1 } },
	{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,-1,1,0,1,1 },{ 0,0,1,0,0,1,-1,1 },{ 0,0,0,1,-1,0,-1,-1 } },
	{ { 0,0,-1,0,0,1,1,0 },{ 0,0,0,-1,-1,0,0,1 },{ 0,0,1,0,0,-1,-1,0 },{ 0,0,0,1,1,0,0,-1 } },
	{ { 0,0,0,-1,0,1,0,2 },{ 0,0,1,0,-1,0,-2,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,-1,0,1,0,2,0 } },
	{ { 0,0,0,1,-1,0,-1,1 },{ 0,0,-1,0,0,-1,-1,-1 },{ 0,0,0,-1,1,-0,1,-1 },{ 0,0,1,0,0,1,1,1 } }
}; // 7种形状(长L| 短L| 反z| 正z| T| 直一| 田格)，4种朝向(上左下右)，8:每相邻的两个分别为x，y

const int rotateBlank[7][4][10] = {
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { 1,1,0,0 },{ -1,1,0,0 },{ -1,-1,0,0 },{ 1,-1,0,0 } },
	{ { -1,-1,0,0 },{ 1,-1,0,0 },{ 1,1,0,0 },{ -1,1,0,0 } },
	{ { -1,-1,-1,1,1,1,0,0 },{ -1,-1,-1,1,1,-1,0,0 },{ -1,-1,1,1,1,-1,0,0 },{ -1,1,1,1,1,-1,0,0 } },
	{ { 1,-1,-1,1,-2,1,-1,2,-2,2 } ,{ 1,1,-1,-1,-2,-1,-1,-2,-2,-2 } ,{ -1,1,1,-1,2,-1,1,-2,2,-2 } ,{ -1,-1,1,1,2,1,1,2,2,2 } },
	{ { 0,0 },{ 0,0 } ,{ 0,0 } ,{ 0,0 } }
}; // 旋转的时候需要为空的块相对于旋转中心的坐标

struct backupNode
{
	// 先y后x，记录地图状态，0为空，1为以前放置，2为刚刚放置，负数为越界
	// （2用于在清行后将最后一步撤销再送给对方）
	int gridInfo[2][MAPHEIGHT + 2][MAPWIDTH + 2];
	 
	// 代表分别向对方转移的行
	int trans[2][6][MAPWIDTH + 2];
	 
	// 转移行数
	int transCount[2];
	 
	// 运行eliminate后的当前高度
	int maxHeight[2];
	 
	// 总消去行数的分数之和
	int elimTotal[2];

	// 连续几回合发生过消去了
	int elimCombo[2];
	 
	// 给对应玩家的各类块的数目总计
	int typeCountForColor[2][7];
} backupQueue[4]; int backupNum;

class Tetris
{
public:
	const int blockType;   // 标记方块类型的序号 0~6
	int blockX;            // 旋转中心的x轴坐标
	int blockY;            // 旋转中心的y轴坐标
	int orientation;       // 标记方块的朝向 0~3
	const int(*shape)[8]; // 当前类型方块的形状定义
 
	int color;
	
	Tetris(int t, int color) : blockType(t), shape(blockShape[t]), color(color)
	{ }
 
	inline Tetris &set(int x = -1, int y = -1, int o = -1)
	{
		blockX = x == -1 ? blockX : x;
		blockY = y == -1 ? blockY : y;
		orientation = o == -1 ? orientation : o;
		return *this;
	}
 
	// 判断当前位置是否合法
	inline bool isValid(int x = -1, int y = -1, int o = -1)
	{
		x = x == -1 ? blockX : x;
		y = y == -1 ? blockY : y;
		o = o == -1 ? orientation : o;
		if (o < 0 || o > 3)
			return false;
 
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = x + shape[o][2 * i];
			tmpY = y + shape[o][2 * i + 1];
			if (tmpX < 1 || tmpX > MAPWIDTH ||
				tmpY < 1 || tmpY > MAPHEIGHT ||
				gridInfo[color][tmpY][tmpX] != 0)
				return false;
		}
		return true;
	}
	
	inline bool onTop()
	{
		if (!isValid()) return false;
		rep(i, 0, 3) if (blockY + shape[orientation][i*2+1] == MAPHEIGHT) return true;
		return false;
	}
	
	// 判断是否落地
	inline bool onGround()
	{
		if (isValid() && !isValid(-1, blockY - 1))
			return true;
		return false;
	}
 
	// 将方块放置在场地上
	inline bool place()
	{
		if (!onGround())
			return false;
 
		int i, tmpX, tmpY;
		for (i = 0; i < 4; i++)
		{
			tmpX = blockX + shape[orientation][2 * i];
			tmpY = blockY + shape[orientation][2 * i + 1];
			gridInfo[color][tmpY][tmpX] = 2;
		}
		return true;
	}
	
	// 检查能否向下移动
	inline bool movedown(){return isValid(-1, blockY - 1);}
	inline bool moveleft(){return isValid(blockX - 1);}
	inline bool moveright(){return isValid(blockX + 1);}
 
	// 检查能否逆时针旋转自己到o
	inline bool rotation(int o)
	{
		if (o < 0 || o > 3)
			return false;
 
		if (orientation == o)
			return true;
 
		int fromO = orientation;
		int i, blankX, blankY;
		while (true)
		{
			if (!isValid(-1, -1, fromO))
				return false;
 
			if (fromO == o)
				break;
	            
	        // 检查旋转碰撞
	        for (i = 0; i < 5; i++) {
	            blankX = blockX + rotateBlank[blockType][fromO][2 * i];
	            blankY = blockY + rotateBlank[blockType][fromO][2 * i + 1];
	            if (blankX == blockX && blankY == blockY)
	                break;
	            if (gridInfo[color][blankY][blankX] != 0)
	                return false;
	        }
 
			fromO = (fromO + 1) % 4;
		}
		return true;
	}
};

namespace Util
{
	// 围一圈护城河
	inline void init()
	{
		int i;
		for (i = 0; i < MAPHEIGHT + 2; i++)
		{
			gridInfo[1][i][0] = gridInfo[1][i][MAPWIDTH + 1] = -2;
			gridInfo[0][i][0] = gridInfo[0][i][MAPWIDTH + 1] = -2;
		}
		for (i = 0; i < MAPWIDTH + 2; i++)
		{
			gridInfo[1][0][i] = gridInfo[1][MAPHEIGHT + 1][i] = -2;
			gridInfo[0][0][i] = gridInfo[0][MAPHEIGHT + 1][i] = -2;
		}
	}
 
	// 消去行
	void eliminate(int color)
	{
		int &count = transCount[color] = 0;
		int i, j, emptyFlag, fullFlag, firstFull = 1, hasBonus = 0;
		maxHeight[color] = MAPHEIGHT;
		for (i = 1; i <= MAPHEIGHT; i++)
		{
			emptyFlag = 1;
			fullFlag = 1;
			for (j = 1; j <= MAPWIDTH; j++)
			{
				if (gridInfo[color][i][j] == 0)
					fullFlag = 0;
				else
					emptyFlag = 0;
			}
			if (fullFlag)
			{
				if (firstFull && ++elimCombo[color] >= 3)
				{
					// 奖励行
					for (j = 1; j <= MAPWIDTH; j++)
						trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
					count++;
					hasBonus = 1;
				}
				firstFull = 0;
				for (j = 1; j <= MAPWIDTH; j++)
				{
					// 注意这里只转移以前的块，不包括最后一次落下的块（“撤销最后一步”）
					trans[color][count][j] = gridInfo[color][i][j] == 1 ? 1 : 0;
					gridInfo[color][i][j] = 0;
				}
				count++;
			}
			else if (emptyFlag)
			{
				maxHeight[color] = i - 1;
				break;
			}
			else
				for (j = 1; j <= MAPWIDTH; j++)
				{
					gridInfo[color][i - count + hasBonus][j] =
						gridInfo[color][i][j] > 0 ? 1 : gridInfo[color][i][j];
					if (count)
						gridInfo[color][i][j] = 0;
				}
		}
		if (count == 0)
			elimCombo[color] = 0;
		maxHeight[color] -= count - hasBonus;
		elimTotal[color] += elimBonus[count];
	}
 
	// 转移双方消去的行，返回0表示继续，否则返回输者
	int transfer()
	{
		int color1 = 0, color2 = 1;
		if (transCount[color1] == 0 && transCount[color2] == 0)
			return 0;
		if (transCount[color1] == 0 || transCount[color2] == 0)
		{
			if (transCount[color1] == 0 && transCount[color2] > 0)
				swap(color1, color2);
			int h2;
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
			if (h2 > MAPHEIGHT)
				return 1<<color2;
			int i, j;
 
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
			return 0;
		}
		else
		{
			int h1, h2;
			maxHeight[color1] = h1 = maxHeight[color1] + transCount[color2];//从color1处移动count1去color2
			maxHeight[color2] = h2 = maxHeight[color2] + transCount[color1];
			
			if (h1 > MAPHEIGHT && h2 > MAPHEIGHT)
			{
				if (elimTotal[color1]!=elimTotal[color2]) 
					return 1<<(elimTotal[color1]<elimTotal[color2]?color1:color2); 
				else 
					return 3;
			}
			if (h1 > MAPHEIGHT) return 1<<color1;
			if (h2 > MAPHEIGHT) return 1<<color2;
 
			int i, j;
			for (i = h2; i > transCount[color1]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = gridInfo[color2][i - transCount[color1]][j];
 
			for (i = transCount[color1]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color2][i][j] = trans[color1][i - 1][j];
 
			for (i = h1; i > transCount[color2]; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = gridInfo[color1][i - transCount[color2]][j];
 
			for (i = transCount[color2]; i > 0; i--)
				for (j = 1; j <= MAPWIDTH; j++)
					gridInfo[color1][i][j] = trans[color2][i - 1][j];
 
			return 0;
		}
	}
	
	inline void backup()
	{
		rep(o, 0, 1) rep(i, 1, MAPHEIGHT) rep(j, 1, MAPWIDTH)
			backupQueue[backupNum].gridInfo[o][i][j]=gridInfo[o][i][j];
			
		rep(o, 0, 1) rep(i, 0, 3) rep(j, 1, MAPWIDTH)
			backupQueue[backupNum].trans[o][i][j]=trans[o][i][j];
		
		rep(o, 0, 1)
			backupQueue[backupNum].transCount[o]=transCount[o],
			backupQueue[backupNum].maxHeight[o]=maxHeight[o],
			backupQueue[backupNum].elimTotal[o]=elimTotal[o],
			backupQueue[backupNum].elimCombo[o]=elimCombo[o];
		
		rep(o, 0, 1) rep(i, 0, 6)
			backupQueue[backupNum].typeCountForColor[o][i]=typeCountForColor[o][i];
		
		backupNum++;
	}
	
	inline void recover()
	{
		backupNum--;
		
		rep(o, 0, 1) rep(i, 1, MAPHEIGHT) rep(j, 1, MAPWIDTH)
			gridInfo[o][i][j]=backupQueue[backupNum].gridInfo[o][i][j];
			
		rep(o, 0, 1) rep(i, 0, 3) rep(j, 1, MAPWIDTH)
			trans[o][i][j]=backupQueue[backupNum].trans[o][i][j];
		
		rep(o, 0, 1)
			transCount[o]=backupQueue[backupNum].transCount[o],
			maxHeight[o]=backupQueue[backupNum].maxHeight[o],
			elimTotal[o]=backupQueue[backupNum].elimTotal[o],
			elimCombo[o]=backupQueue[backupNum].elimCombo[o];
		
		rep(o, 0, 1) rep(i, 0, 6)
			typeCountForColor[o][i]=backupQueue[backupNum].typeCountForColor[o][i];
	}
 
	// 打印场地用于调试
	inline void printField()
	{
#ifndef _BOTZONE_ONLINE
		static const char *i2s[] = {
			"~~",
			"~~",
			"  ",
			"[]",
			"##"
		};
		cout << "~~：墙，[]：块，##：新块" << endl;
		for (int y = MAPHEIGHT + 1; y >= 0; y--)
		{
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[0][y][x] + 2];
			for (int x = 0; x <= MAPWIDTH + 1; x++)
				cout << i2s[gridInfo[1][y][x] + 2];
			cout << endl;
		}
#endif
	}
}








struct Neuron
{
	double a0[4][3][3], a1[7][4][3][3], w0[43][7], w1[7][7], t[7], b0[4], b1[7], wb0[7], wb1[7], tb;
} Ner[2];

inline void GetNer(int a)
{
	char filename[12]="player0.txt";
	
	filename[6]='0'+a;
	freopen(filename, "r", stdin);
	
	rep(o, 0, 3) rep(i, 0, 2) rep(j, 0, 2) scanf("%lf", &Ner[a].a0[o][i][j]);
	rep(o, 0, 6) rep(k, 0, 3) rep(i, 0, 2) rep(j, 0, 2) scanf("%lf", &Ner[a].a1[o][k][i][j]);
	rep(i, 0, 42) rep(j, 0, 6) scanf("%lf", &Ner[a].w0[i][j]);
	rep(i, 0, 6) rep(j, 0, 6) scanf("%lf", &Ner[a].w1[i][j]);
	rep(i, 0, 6) scanf("%lf", &Ner[a].t[i]);
	
	rep(i, 0, 3) scanf("%lf", &Ner[a].b0[i]);
	rep(i, 0, 6) scanf("%lf", &Ner[a].b1[i]);
	rep(i, 0, 6) scanf("%lf", &Ner[a].wb0[i]);
	rep(i, 0, 6) scanf("%lf", &Ner[a].wb1[i]);
	scanf("%lf", &Ner[a].tb);
	
	fclose(stdin);
}

double input[12][22], net1[4][7][11], net3[43], net4[7], net5[7], output, tmp;
inline double sigmoid(double x){return 1.0/(1.0+exp(-x));}
inline double Value(int color, int NerID)
{
	//Util::printField();
	
	rep(i, 0, MAPWIDTH+1) rep(j, 0, MAPHEIGHT+1) input[i][j]=(!gridInfo[color][j][i])?0:1;
	rep(o, 0, 3) for(int x=0; x+2<12; x++) for(int y=0; y+2<22; y++)
	{
		tmp=Ner[NerID].b0[o];
		rep(i, 0, 2) rep(j, 0, 2) tmp+=input[x+i][y+j]*Ner[NerID].a0[o][i][j];
		net1[o][1+x/2][1+y/2]=max(net1[o][1+x/2][1+y/2], max(tmp,0.0));
	}
	rep(o, 0, 6) for(int x=0; x+2<7; x+=2) for(int y=0; y+2<11; y+=2)
	{
		tmp=Ner[NerID].b1[o];
		rep(k, 0, 3) rep(i, 0, 2) rep(j, 0, 2) tmp+=net1[k][x+i][y+j]*Ner[NerID].a1[o][k][i][j];
		net3[o*5+y/2]=max(net3[o*5+y/2], max(tmp,0.0));
	}
	int mn=0;
	rep(o, 0, 6) mn=min(mn,typeCountForColor[color][o]);
	rep(o, 0, 6) net3[35+o]=mn+2-typeCountForColor[color][o]; net3[42]=elimCombo[color];
	rep(i, 0, 6)
	{
		net4[i]=Ner[NerID].wb0[i];
		rep(j, 0, 42) net4[i]+=net3[j]*Ner[NerID].w0[j][i];
		net4[i]=sigmoid(net4[i]);
	}
	rep(i, 0, 6)
	{
		net5[i]=Ner[NerID].wb1[i];
		rep(j, 0, 6) net5[i]+=net4[j]*Ner[NerID].w1[j][i];
		net5[i]=sigmoid(net5[i]);
	}
	output=Ner[NerID].tb; rep(i, 0, 6) output+=net5[i]*Ner[NerID].t[i];
	return sigmoid(output);
}




/*
10*20
12*22
10*20 7*3*3
5*10
7*11
3*5 7*3*3
1*5
35+7=42

42*7
7*7
*/







int blockType, typePosX, typePosY, typePosO, blockForEnemy, nextTypeForColor[2], currTypeForColor[2];

bool vis[MAPWIDTH+2][MAPHEIGHT+2][4];

struct node{int x,y,o;};

inline Tetris Determine(int currBotColor, int type, int NerColor)
{
	Tetris block(type, currBotColor), BestAction(type, currBotColor); BestAction.set(1,1,-1);
	
	queue<node>q; clr(vis,0);
	
	rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT-3, MAPHEIGHT) rep(o, 0, 3) if (block.set(x,y,o).onTop())
		vis[x][y][o]=1, q.push((node){x,y,o});
	
	double mx=-1e30, tmp;
	while (!q.empty())
	{
		int x=q.front().x, y=q.front().y, o=q.front().o; q.pop();
		block.set(x,y,o);
		if (block.moveleft() && !vis[x-1][y][o]) vis[x-1][y][o]=1, q.push((node){x-1,y,o});
		if (block.moveright() && !vis[x+1][y][o]) vis[x+1][y][o]=1, q.push((node){x+1,y,o});
		if (block.movedown() && !vis[x][y-1][o]) vis[x][y-1][o]=1, q.push((node){x,y-1,o});
		rep(i, 0, 3) if (block.rotation(i) && !vis[x][y][i]) vis[x][y][i]=1, q.push((node){x,y,i});
		
		if (block.onGround())
		{
			Util::backup(); block.place(); Util::eliminate(currBotColor); // 消行，对方不加行 
			if ((tmp=Value(currBotColor,NerColor))>mx) mx=tmp, BestAction.set(block.blockX,block.blockY,block.orientation);
			Util::recover();
		}
	}
	
	return BestAction;
}

inline void WorstDetermine(int color)
{
	// 再看看给对方什么好
 
	int maxCount = 0, minCount = 99;
	rep(type, 0, 6)
	{
		if (typeCountForColor[color][type] > maxCount)
			maxCount = typeCountForColor[color][type];
		if (typeCountForColor[color][type] < minCount)
			minCount = typeCountForColor[color][type];
	}
	
	blockForEnemy=0; double mn=1e30, tmp;
	
	rep(type, 0, 6)
	{
		if (typeCountForColor[color][type] == maxCount && maxCount - minCount == 2) continue;
		typeCountForColor[color][type]++;
		
		Tetris block=Determine(color, type, color^1); if (block.orientation<0) {blockForEnemy=type; return;}
		Util::backup(); block.place(); Util::eliminate(color); 
		if ((tmp=Value(color,color^1))<mn) mn=tmp, blockForEnemy=type;
		Util::recover();
		
		typeCountForColor[color][type]--;
	}
}

inline void PlayerAction(int currBotColor)
{
	int enemyColor=1^currBotColor;
	
	Tetris currB=Determine(currBotColor, currTypeForColor[currBotColor],currBotColor);
	Tetris enemyB=Determine(enemyColor, currTypeForColor[enemyColor],currBotColor);
	
	Util::backup();
	
	currB.place(); enemyB.place();
	Util::eliminate(0);
	Util::eliminate(1);
	int tmp=Util::transfer();
	
	if (!tmp) WorstDetermine(enemyColor); else
	{
		int maxCount = 0, minCount = 99;
		rep(type, 0, 6)
		{
			if (typeCountForColor[enemyColor][type] > maxCount)
				maxCount = typeCountForColor[enemyColor][type];
			if (typeCountForColor[enemyColor][type] < minCount)
				minCount = typeCountForColor[enemyColor][type];
		}
		
		rep(type, 0, 6) if (typeCountForColor[enemyColor][type] != maxCount || maxCount - minCount != 2) 
		{
			blockForEnemy=type;
			break;
		}
	}
	
	Util::recover();
	
	typePosX=currB.blockX, typePosY=currB.blockY, typePosO=currB.orientation;
}

inline int TestCanPlace()
{
	int fg=0;
	
	Tetris block0(currTypeForColor[0],0);
	rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT-3, MAPHEIGHT) rep(o, 0, 3) if (block0.set(x,y,o).onTop()) fg|=1;
	Tetris block1(currTypeForColor[1],1);
	rep(x, 1, MAPWIDTH) rep(y, MAPHEIGHT-3, MAPHEIGHT) rep(o, 0, 3) if (block1.set(x,y,o).onTop()) fg|=2;
	
	fg^=3; if (fg==3 && elimTotal[0]!=elimTotal[1]) fg=(elimTotal[0]<elimTotal[1]?1:2);
	
	return fg;
}




 
int main()
{
	GetNer(0); GetNer(1);

	istream::sync_with_stdio(false);
	
#ifndef _BOTZONE_ONLINE	
	freopen("seed.txt", "r", stdin); 
	int seed; cin >> seed;
	srand(seed); fclose(stdin);
	freopen("seed.txt", "w", stdout);
	printf("%d\n", rand()*(RAND_MAX+1)+rand()+(int)(time(NULL)%10000));
	fclose(stdout);
#endif
	
#ifdef _BOTZONE_ONLINE	
	srand(time(NULL));
#endif

	Util::init(); // 围护城河
	
	blockType = rand()%7;
	nextTypeForColor[0] = blockType;
	nextTypeForColor[1] = blockType;
	typeCountForColor[0][blockType]++;
	typeCountForColor[1][blockType]++;
	
	int turnID = 0; while (++turnID)
	{
		currTypeForColor[0] = nextTypeForColor[0];
		currTypeForColor[1] = nextTypeForColor[1];
		
		if ((ED=TestCanPlace())) goto Final;
 
		// 先读自己的输出，也就是自己的行为
		// 自己的输出是自己的最后一步
		// 然后模拟最后一步放置块
		PlayerAction(0);
 
		// 我当时把上一块落到了 x y o！
		Tetris myBlock(currTypeForColor[0], 0);
		myBlock.set(typePosX, typePosY, typePosO);
 
		// 我给对方什么块来着？
		nextTypeForColor[1] = blockForEnemy;
 
		// 然后读自己的输入，也就是对方的行为
		// 裁判给自己的输入是对方的最后一步
		PlayerAction(1);
 
		// 对方当时把上一块落到了 x y o！
		Tetris enemyBlock(currTypeForColor[1], 1);
		enemyBlock.set(typePosX, typePosY, typePosO);
 
		// 对方给我什么块来着？
		nextTypeForColor[0] = blockForEnemy;
		
		myBlock.place();
		enemyBlock.place();
		typeCountForColor[0][nextTypeForColor[0]]++;
		typeCountForColor[1][nextTypeForColor[1]]++;
 
		// 检查消去
		Util::eliminate(0);
		Util::eliminate(1);
 
		// 进行转移
		if ((ED=Util::transfer())) goto Final;
		
		//Util::printField();
	}
	
	Final:freopen("result.txt", "w", stdout);
	if (ED==1) puts("-1");
	if (ED==2) puts("1");
	if (ED==3) puts("0");
	fclose(stdout);
 
	return 0;
}