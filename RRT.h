#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<array>
#include<graphics.h>
#include<random>

#define StepSize 70
#define obs_num 10
#define min_r 10
#define max_r 30
#define t 20 //Ҳ�ǻ������������㵽��Բ�����յ�뾶��t��Բ��ʱ�㵽��


using Obs_arr = std::array<int, 3>;
using Obs_lst = std::array<Obs_arr, obs_num>;



int RandomNum(int init, int end)//ָ����Χ
{
	int rand_num;
	std::random_device seed;
	std::default_random_engine e1(seed());
	rand_num = e1()%end+init;
	return rand_num;
}

struct Node
{
	int num = -1;
	int x;
	int y;
	int father = -1;

	
};
class Tree;
class Map
{
	
public:
	Node init;
	Node end;
	int mx;
	int my;
	Obs_lst obs; //Բ�� x,y,�뾶r
public:
	void RandObs()
	{
		int rx, ry,r;
		
		Obs_arr ob;
		for (int i = 0; i < obs_num; i++)
		{
			r = RandomNum(min_r,max_r);
			rx = RandomNum(r ,mx - r);
			ry = RandomNum(r, my - r);
			ob[0] = rx;
			ob[1] = ry;
			ob[2] = r;
			obs[i] = ob;
		}
	}

	void DrawMap()
	{
		
		//1.ȷ������
		initgraph(mx,my,SHOWCONSOLE);
		setbkcolor(WHITE);
		cleardevice();
		//2.��ʼ��ͻ��ϰ���
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
		//����ϰ���
		RandObs();

	}
};

class Tree 
{
private:
	friend class Map;
	Node xrand;
	Node xnew;
	
	Node xmid; //xnew��xnear���е�
	//std::vector<std::pair<int, int>> ei;//���ӵ��ĵ���
public:
	std::vector<Node> branch;
	Node xnear;
public:
	Tree(Node xinit)
	{
		branch.push_back(xinit);
		
	}
public:
	void DrawBranch()
	{
		setlinestyle(PS_SOLID, 1);
		setlinecolor(BLUE);
		//1.����
		putpixel(xnear.x, xnear.y, BLACK);
		putpixel(xnew.x, xnew.y, BLACK);

		//2.����
		line(xnear.x, xnear.y, xnew.x, xnew.y);
	}
	void AddNode()
	{
		branch.push_back(xnew);
		//ei.push_back({ xnear.num, xnew.num });
		
	}


	void Sample(const Map* map)
	{
		int i = 0;
		
		
			xrand.x = RandomNum(0, map->mx);
			xrand.y = RandomNum(0, map->my);
		
			
		
	}

	void Near()
	{
		Node near_node=branch[0], node;
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
		xnear = near_node;
	}

	void Steer()
	{
		double dist = Distance(xrand,xnear);
		xnew.x = xnear.x + (xrand.x - xnear.x)*StepSize / dist;
		xnew.y = xnear.y + (xrand.y - xnear.y)*StepSize / dist;

	}
	double Distance(Node node1, Node node2)
	{
		double dist;
		dist = sqrt((node1.x - node2.x)*(node1.x - node2.x) + (node1.y - node2.y)*(node1.y - node2.y));
		return dist;
	}
	double Distance(const Obs_arr& ob, Node node) //�ϰ���Բ������ľ���
	{
		double dist;
		dist = sqrt((node.x - ob[0])*(node.x - ob[0]) + (node.y - ob[1])*(node.y - ob[1]));
		return dist;
	}
	double Distance(const Obs_arr& ob, Node node1, Node node2) //�ϰ���Բ�ĵ��滮ֱ�ߵľ���
	{
		double dist;
		dist = abs((node2.y - node1.y)*ob[0] + (node1.x - node2.x)*ob[1] + node2.x*node1.y - node1.x*node2.y) / Distance(node1, node2);
		return dist;
	}

	bool IsRandInObs(const Obs_lst& obs)//xrand�������룬�Ѿ���������
	{
		bool result = false;
		for (int i = 0; i < obs_num; i++)
		{
			if (Distance(obs[i], xrand) < t+ obs[i][2])
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
		for ( i = 0; i < obs_num; i++)
		{
			ob = obs[i];
			if ((Distance(ob, xmid) < t + ob[2])|| (Distance(ob, xnew) < t + ob[2]) || (Distance(ob, xnear) < t + ob[2])&& Distance(ob,xnew,xnear)<t+ob[2] )
			{
				break;
			}
		}
		if (i == obs_num)
		{
			xnew.num = branch.size() ;
			xnew.father = xnear.num;

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

std::vector<Node> Map::Path(const Tree* tree)//mapҲ������ģ���Ϊ�����ھ��Ѿ�������
//���ݣ�ei��Ҫ�������·������
{
	Node father, node=tree->branch[tree->branch.size()-1];
	std::vector<Node> path;
	while (node.father != init.num)
	{

		path.push_back(node);
		father = tree->branch[node.father];

		setlinestyle(PS_SOLID, 3);
		setlinecolor(GREEN);
		line(father.x, father.y, node.x, node.y);

		node = father;

	}
	path.push_back(init);
	line(init.x, init.y, node.x, node.y);
	return path;
}
