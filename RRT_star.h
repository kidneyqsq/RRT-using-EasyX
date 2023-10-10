#pragma once
////不需要启发！！！直接算cost

#include<iostream>
#include<string>
#include<vector>
#include<array>
#include<graphics.h>
#include<random>


#define StepSize 30
#define obs_num 20
#define min_r 10
#define max_r 30
#define t 20 //也是缓冲区，采样点到达圆心是终点半径是t的圆内时算到了
#define Color_Num 6
int color[Color_Num] = { BLUE,RED,YELLOW,BROWN,GREEN, BLACK };

using Obs_arr = std::array<int, 3>;
using Obs_lst = std::array<Obs_arr, obs_num>;



int RandomNum(int init, int end)//指定范围2
{
	int rand_num;
	
	std::random_device rd;
	std::default_random_engine e1{ rd() };
	std::uniform_int_distribution<int> random(init, end);
	rand_num = random(e1);
	return rand_num;
}

struct Node
{
	int num = -1;
	int x;
	int y;
	int father = -1;
	double cost_f = 0;
	double cost_g = 0;
	double cost_h = 0;
	
};

class Tree;
class Map
{

public:
	Node init;
	Node end;
	int mx;
	int my;
	Obs_lst obs; //圆心 x,y,半径r
public:
	void RandObs()
	{
		int rx, ry, r;

		Obs_arr ob;
		for (int i = 0; i < obs_num; i++)
		{
			r = RandomNum(min_r, max_r);
			rx = RandomNum(r, mx - r);
			ry = RandomNum(r, my - r);
			ob[0] = rx;
			ob[1] = ry;
			ob[2] = r;
			obs[i] = ob;
		}
	}

	void DrawMap()
	{

		//1.确定区域
		initgraph(mx, my);
		setbkcolor(WHITE);
		cleardevice();
		//2.起始点和画障碍物
		//putpixel()
		setfillcolor(RED);
		solidcircle(init.x, init.y, t);
		solidcircle(end.x, end.y, t);
		Obs_arr ob;
		setfillcolor(BLACK);
		for (int i = 0; i < obs_num; i++)
		{
			ob = obs[i];
			solidcircle(ob[0], ob[1], ob[2]);
		}

	}

	std::vector<Node> Path(const Tree* tree);

	void FinishDraw()
	{
		getchar();
		closegraph();
	}
public:
	Map(int x, int y, Node init, Node end)
		:mx(x), my(y), init(init), end(end)
	{
		//随机障碍物
		RandObs();

	}

	bool IsOutOfArea(int x, int y)
	{
		bool flag = false;
		if (x > mx || x<0 || y>my || y < 0)
			flag = true;
		return flag;
	}

};

double Distance(Node node1, Node node2)
{
	double dist;
	dist = sqrt((node1.x - node2.x)*(node1.x - node2.x) + (node1.y - node2.y)*(node1.y - node2.y));
	return dist;
}
double Distance(const Obs_arr& ob, Node node) //障碍物圆心与点间的距离
{
	double dist;
	dist = sqrt((node.x - ob[0])*(node.x - ob[0]) + (node.y - ob[1])*(node.y - ob[1]));
	return dist;
}
double Distance(const Obs_arr& ob, Node node1, Node node2) //障碍物圆心到规划直线的距离
{
	double dist;
	dist = abs((node2.y - node1.y)*ob[0] + (node1.x - node2.x)*ob[1] + node2.x*node1.y - node1.x*node2.y) / Distance(node1, node2);
	return dist;
}

class Tree
{
public:
	friend class Map;
	Node xrand;
	Node xnew;
	Node xmid; //xnew与xnear的中点
	//std::vector<std::pair<int, int>> ei;//连接到的点编号

	std::vector<Node> branch;
	Node xmin;
	Node xnearest;
	Node xnear;
	std::vector<Node> Xnear;
public:
	Tree(Node xinit)
	{
		branch.push_back(xinit);
	}
public:
	void DrawBranch(Node xnear, Node xnew)
	{
		setlinestyle(PS_SOLID, 1);
		setlinecolor(BLUE);
		//1.画点
		putpixel(xnear.x, xnear.y, BLACK);
		putpixel(xnew.x, xnew.y, BLACK);

		//2.画边
		line(xnear.x, xnear.y, xnew.x, xnew.y);
	}
	void DrawBranch()
	{

		setlinestyle(PS_SOLID, 1);
		setlinecolor(BLUE);
		Node node, father;
		for (int i = 0; i < branch.size(); i++)
		{
			node = branch[i];
			father = branch[node.father];
			//1.画点
			putpixel(node.x, node.y, BLACK);
			putpixel(father.x, father.y, BLACK);

			//2.画边
			line(node.x, node.y, father.x, father.y);
		}
		

		
	}
	void AddNode()
	{
		xnew.num = branch.size();
		xnew.father = xnearest.num;
		xnew.cost_f = 0;
		xnew.cost_g = 0;
		xnew.cost_h = 0;
		branch.push_back(xnew);
		//ei.push_back({ xnear.num, xnew.num });

	}


	void Sample(const Map* map)
	{
		int i = 0;

		
			xrand.x = RandomNum(0, map->mx);
			xrand.y = RandomNum(0, map->my);
			

		
	}

	void Near(double yita, const Obs_lst& obs) //指定半径yita，以xnew为圆心的圆范围内的xnear才考虑
	{
		Node  node = xnearest;
		Xnear.empty();
		double r,dist = Distance(node, xnew);
		double gamma = 1500;//计算方法参见rrt*经典文章
		int card = branch.size();
		r = min(gamma*sqrt(log(card)/card),yita);
		int i;
		for (i = 0; i < card-1; i++)
		{
			node = branch[i];
			if (!IsEdgeCrossObs(obs, node, xnew))
			{
				dist = Distance(node, xnew);
					if (i != 0 )
						{
							if (dist > r)
								continue;
							else
								Xnear.push_back(node);
						}

			}
			
		}

	}

	void Nearest()
	{
		Node near_node = branch[0], node;
		double dist;
		for (int i = 0; i < branch.size(); i++)
		{
			node = branch[i];
			if (i != 0)
			{
				if (dist > Distance(node, xrand))
				{
					near_node = node;
				}
			}
			dist = Distance(node, xrand);

		}
		xnearest = near_node;
	}
	void Steer()
	{
		double dist = Distance(xrand, xnearest);
		xnew.x = xnearest.x + (xrand.x - xnearest.x)*StepSize / dist;
		xnew.y = xnearest.y + (xrand.y - xnearest.y)*StepSize / dist;
		
	}

	bool IsRandInObs(const Obs_lst& obs)//xrand不用输入，已经在类内了
	{
		bool result = false;
		for (int i = 0; i < obs_num; i++)
		{
			if (Distance(obs[i], xrand) < t + obs[i][2])
				result = true;
		}

		return result;
	}

	bool IsEdgeCrossObs(const Obs_lst& obs, Node xnear)
	{
		xmid.x = (xnear.x + xnew.x)*0.5;
		xmid.y = (xnear.y + xnew.y)*0.5;
		Obs_arr ob;
		int i;
		for (i = 0; i < obs_num; i++)
		{
			ob = obs[i];
			if (((Distance(ob, xmid) < t + ob[2]) || (Distance(ob, xnew) < t + ob[2]) || (Distance(ob, xnear) < t + ob[2])) || (Distance(ob, xnew, xnear) < t + ob[2]))
			{
				break;
			}
		}
		if (i == obs_num)
		{
			return false;
		}
		else
			return true;

	}
	bool IsEdgeCrossObs(const Obs_lst& obs, Node xnear, Node xnew)
	{
		xmid.x = (xnear.x + xnew.x)*0.5;
		xmid.y = (xnear.y + xnew.y)*0.5;
		Obs_arr ob;
		int i;
		for (i = 0; i < obs_num; i++)
		{
			ob = obs[i];
			if (((Distance(ob, xmid) < t + ob[2]) || (Distance(ob, xnew) < t + ob[2]) || (Distance(ob, xnear) < t + ob[2])) && (Distance(ob, xnew, xnear) < t + ob[2]))
			{
				break;
			}
		}
		if (i == obs_num)
		{
			return false;
		}
		else
			return true;

	}


	bool Arrived(const Map* map)
	{
		if (Distance(xnew, map->end) <= t)
		{

			return true;
		}
		else
			return false;
	}
};

std::vector<Node> Map::Path(const Tree* tree)//map也是输入的，因为在类内就已经包含了
//回溯：ei中要存有最短路径连接
{
	Node father, node = tree->branch[tree->branch.size() - 1];
	std::vector<Node> path;
	//设置随机颜色
	setlinecolor(color[RandomNum(-1, Color_Num)]);
	while (node.father != init.num)
	{

		path.push_back(node);
		father = tree->branch[node.father];

		setlinestyle(PS_SOLID, 3);
		//setlinecolor(color[BLACK]);
		line(father.x, father.y, node.x, node.y);

		node = father;

	}
	path.push_back(init);
	line(init.x, init.y, node.x, node.y);
	return path;
}
Node Calcu_Cost2(Node x1, Node x2, Node end)//from x1 to x2, return x2
{
	x2.cost_g = x1.cost_g + Distance(x1, x2);
	//x2.cost_h = Distance(x2, end);
	//x2.cost_f = x2.cost_g + x2.cost_h;
	return x2;
}
double Calcu_Cost1(Node x1, Node x2, Node end)//from x1 to x2, return x2
{
	x2.cost_g = x1.cost_g + Distance(x1, x2);
	//x2.cost_h = Distance(x2, end);
	//x2.cost_f = x2.cost_g + x2.cost_h;
	return x2.cost_g;
}