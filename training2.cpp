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

#define NerM 433
#define NerN 11
#define NerNum 10
#define OPNum 1
#define HPinit 16
#define MaxNerNum 10000



inline double rand01(){return (1.0*rand()/RAND_MAX)+(1.0*rand()/RAND_MAX/RAND_MAX)+(1.0*rand()/RAND_MAX/RAND_MAX/RAND_MAX);}
inline double rand2(){return rand01()*2-1;}



struct Neuron
{
	int ActType;
	double weight[NerM], b0[NerN], theta[NerN], b1;
} Ner[10009]; int NerCnt, v[19], hp[19];

inline void NeuronInit(int a)
{
	v[a]=++NerCnt, hp[a]=HPinit;
	rep(i, 0, NerM-1) Ner[v[a]].weight[i]=rand2();
	rep(i, 0, NerN-1) Ner[v[a]].b0[i]=rand2(), Ner[v[a]].theta[i]=rand2();
	Ner[v[a]].b1=rand2();
	Ner[v[a]].ActType=rand()%8;
}

inline void NeuronVary(Neuron &a, double o)
{
	if (rand()%10<(int)o) a.ActType=rand()%8;
	if (rand()%10<(int)o) a.b1=rand2();
	while (rand()%(int)(o*NerN/10+1)) a.b0[rand()%NerN]=rand2();
	while (rand()%(int)(o*NerN/10+1)) a.theta[rand()%NerN]=rand2();
	while (rand()%(int)(o*NerM/10+1)) a.weight[rand()%NerM]=rand2();
}

inline void NeuronComb(Neuron &a, Neuron &b, Neuron &c, double o)
{
	int cnt=0;
	c.weight[cnt]=(rand01()>o?a:b).weight[cnt], cnt++;
	rep(i, 1, 5) for(int i=1,rd=rand01()>o; i<=4; i++) c.weight[cnt]=(rd?a:b).weight[cnt], cnt++;
	rep(i, 1, 4) c.weight[cnt]=(rand01()>o?a:b).weight[cnt], cnt++;
	for(int i=1,rd=rand01()>o; i<=3; i++) c.weight[cnt]=(rd?a:b).weight[cnt], cnt++;
	rep(i, 1, 58) for(int j=1,rd=rand01()>o; j<=7; j++) c.weight[cnt]=(rd?a:b).weight[cnt], cnt++;
	
	rep(i, 0, NerN-1) c.b0[i]=(rand01()>o?a:b).b0[i];
	rep(i, 0, NerN-1) c.theta[i]=(rand01()>o?a:b).theta[i];
	c.b1=(rand01()>o?a:b).b1;
	c.ActType=(rand01()>o?a:b).ActType;
}



inline void Death(int a)
{
	rep(i, a+1, NerNum) swap(v[i-1], v[i]), swap(hp[i-1], hp[i]);
	a=NerNum; v[a]=++NerCnt, hp[a]=HPinit;
	NeuronComb(Ner[v[rand()%4+1]], Ner[v[rand()%4+1]], Ner[v[a]], 0.5+rand2()*0.2);
	NeuronVary(Ner[v[a]], rand01()*2+1.1);
}



int initbot[10009], tot;

inline void OutputNer(int a) 
{
	rep(i, 0, NerM-1) printf("%.9lf%c", Ner[v[a]].weight[i], i==NerM-1?'\n':' ');
	rep(i, 0, NerN-1) printf("%.9lf%c", Ner[v[a]].b0[i], i==NerN-1?'\n':' ');
	rep(i, 0, NerN-1) printf("%.9lf%c", Ner[v[a]].theta[i], i==NerN-1?'\n':' ');
	printf("%.9lf\n", Ner[v[a]].b1);
	printf("%d\n", Ner[v[a]].ActType);
	puts("");
}

inline void OutputData()
{
	char filename[16]="data\\train2.txt";
	
	freopen(filename, "w", stdout);
	rep(o, 1, NerNum) printf("%d %d\n", v[o], hp[o]), OutputNer(o);
	printf("%d\n", tot);
	rep(i, 1, tot) putchar(initbot[i]+'0');
	puts("");
	fclose(stdout);
}

inline void InputNer(int a) 
{
	scanf("%d%d", &v[a], &hp[a]);
	rep(i, 0, NerM-1) scanf("%lf", &Ner[v[a]].weight[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[v[a]].b0[i]);
	rep(i, 0, NerN-1) scanf("%lf", &Ner[v[a]].theta[i]);
	scanf("%lf", &Ner[v[a]].b1);
	scanf("%d", &Ner[v[a]].ActType);
}

char s[10009];
inline void InputData()
{
	char filename[16]="data\\train2.txt";
	
	freopen(filename, "r", stdin);
	rep(o, 1, NerNum) InputNer(o);
	scanf("%d%s", &tot, s);
	rep(i, 1, tot) initbot[i]=s[i-1]-'0';
	fclose(stdin);
	
	NerCnt=v[NerNum];
}

inline void Game(int player0, int player1)
{
	freopen("player0.txt", "w", stdout);
	OutputNer(player0);
	fclose(stdout);
	
	freopen("player1.txt", "w", stdout);
	OutputNer(player1);
	fclose(stdout);
	
	system("offline.exe");
	
	int Result=0;
	freopen("result.txt", "r", stdin);
	scanf("%d", &Result);
	fclose(stdin);
	
	if (player0 && player1)
	{
		if (Result>0) hp[player0]+=1, hp[player1]-=2;
		if (Result<0) hp[player0]-=2, hp[player1]+=1;
	}
	else
	{
		if (!player0) swap(player0, player1), Result*=-1;
		initbot[++tot]=Result+1;
		if (Result>0) hp[player0]+=5;
		if (Result<0) hp[player0]-=1;
		
	}
}

int TIM=0;
int main()
{
	srand(time(NULL));
	
	int tmp; scanf("%d", &tmp);
	if (tmp) InputData(); else rep(i, 1, NerNum) NeuronInit(i); Ner[0].ActType=-1;
	
	int TIM=0; while (NerCnt<=MaxNerNum)
	{
		int A=0, B=0;
		while (A==B)
		{
			A=rand()%(NerNum+OPNum), B=rand()%(NerNum+OPNum);
			if (!min(A,B) && max(A,B)>4) A=B=0;
		}
		Game(A,B); if (A && hp[A]<=0) Death(A); if (B && hp[B]<=0) Death(B);
		
		TIM++; if (TIM%10==0)
		{
			OutputData();
			
			freopen("con", "w", stdout);
			
			puts("Alive:");
			rep(i, 1, NerNum) printf("%d%c", v[i], i==NerNum?'\n':'\t');
			rep(i, 1, NerNum) printf("%d%c", hp[i], i==NerNum?'\n':'\t');
			puts("");
			
			if (tot>=100)
			{
				int sum=0;
				rep(i, tot-100+1, tot) sum+=initbot[i];
				printf("recent 100: %.0lf%%\n", (1.0*sum/100)/2*100);
			}
			if (tot>=50)
			{
				int sum=0;
				rep(i, tot-50+1, tot) sum+=initbot[i];
				printf("recent 50: %.0lf%%\n", (1.0*sum/50)/2*100);
			}
			if (tot>=20)
			{
				int sum=0;
				rep(i, tot-20+1, tot) sum+=initbot[i];
				printf("recent 20: %.0lf%%\n", (1.0*sum/20)/2*100);
			}
			if (tot>=10)
			{
				int sum=0;
				rep(i, tot-10+1, tot) sum+=initbot[i];
				printf("recent 10: %.0lf%%\n", (1.0*sum/10)/2*100);
				puts("");
			}
		}
	}
	
	return 0;
}
