//#include"RRT.h"
#include"RRT_star.h"

int main()
{
	//1.����ͼ��ȷ�����յ�
	Node init = {0,10,10,0 };
	Node end = { 1, 300,300, -1 };
	Node xmin;
	Map map( 600,600,init,end);
	map.RandObs();
	map.DrawMap();
	
	//2.��ʼ����
	init.cost_h = Distance(init, end);
	init.cost_g = 0;
	init.cost_f = init.cost_g + init.cost_h;
	Tree tree(init);
	int n = 2000;
	double ctemp, cmin = 100000;

	for (int i = 0; i < n; i++)
	{ 
	//3. ����
		tree.Sample(&map);

		//4. �ҳ��ٽ��ڵ�
		tree.Nearest();
		
	//5. �����½ڵ�
		tree.Steer(); 
		while(1)
		{ 
		if (map.IsOutOfArea(tree.xnew.x, tree.xnew.y))
		{
			tree.Steer();
		}
		else break;
		}

	//6.�½ڵ����ٽ��ڵ������Ƿ񾭹��ϰ���
		if (tree.IsEdgeCrossObs(map.obs, tree.xnearest,tree.xnew))
		{
			continue;
		}
		else
		{	
			
			tree.AddNode();
			tree.Near(10*StepSize, map.obs);
			xmin = tree.xnearest;
			cmin = Calcu_Cost1(tree.xnearest, tree.xnew, end);
			

			for (int i = 0; i < tree.Xnear.size(); i++)
			{
				tree.xnear = tree.Xnear[i];
				ctemp = Calcu_Cost1(tree.xnear, tree.xnew, end);
				if (!tree.IsEdgeCrossObs(map.obs, tree.xnear) && (cmin > ctemp))
				{
					xmin = tree.xnear;
					cmin = Calcu_Cost1(tree.xnear, tree.xnew, end);
				}

			}
			//tree.DrawBranch(xmin, tree.xnew);
			tree.xnew = Calcu_Cost2(xmin, tree.xnew, end);
			tree.xnew.father = xmin.num;
			tree.branch[tree.xnew.num] = tree.xnew;

			for (int i = 0; i < tree.Xnear.size(); i++)
			{
				tree.xnear = tree.Xnear[i];
				ctemp = Calcu_Cost1(tree.xnear, tree.xnew, end);
				if (!tree.IsEdgeCrossObs(map.obs, tree.xnear) && (cmin > ctemp))
				{
					tree.xnear.father = tree.xnew.num;
					tree.xnear = Calcu_Cost2(tree.xnew, tree.xnear, end);
					tree.branch[tree.xnear.num] = tree.xnear;
					//tree.DrawBranch(tree.xnear, tree.xnew);
					
		
				}

			}
			
		}
		//tree.DrawBranch();
	//7.�ж��Ƿ񵽴��յ�
		if (tree.Arrived(&map))
		{
	
				map.Path(&tree);
		}

	}

	//8. ��·��
	

	map.FinishDraw();


	return 0;
}