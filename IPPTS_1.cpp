#pragma once
#include "DAG.h"
class IPPTS : public scheduler {
public:
	double duration;
	int PCM(int, int);
	//double rankPCM(int);
	double Prank(int);
	void sort_Prank();
	void CreatDAG();
	void scheduling();
};


double scheduler::ranku(int index)
{
	double maxi = 0.0, rank_value = 0.0;

	for (int j = 0; j < num_task; j++)
	{
		if (DAG::DAG[index][j] != 0)
			maxi = max(maxi, DAG::DAG[index][j] + ranku(j));
	}
	rank_value = avg_ComputationCost[index] + maxi;
	return rank_value;
}



int IPPTS::PCM(int ni, int pk)
{
	int minPath = 0, Maxmin = 0;

	for (int i = 0; i < num_task; i++) {
		if (DAG::DAG[ni][i] != 0) {
			for (int k = 0; k < num_processor; k++) {
				if (pk == k) {
					if (minPath == 0)
						{ minPath = DAG::task[i].pcm[k] + DAG::task[i].ComputationCosts[k] + DAG::task[ni].ComputationCosts[k]; }
					else 
					{ minPath = min(minPath, DAG::task[i].pcm[k] + DAG::task[i].ComputationCosts[k] + DAG::task[ni].ComputationCosts[k]); }
				}
				else  {
					if (minPath == 0) 
						{ minPath = DAG::task[i].pcm[k] + DAG::task[i].ComputationCosts[k] + DAG::task[ni].ComputationCosts[k] + DAG::DAG[ni][i]; }
					else 
					{ minPath = min(minPath, DAG::task[i].pcm[k] + DAG::task[i].ComputationCosts[k] + DAG::task[ni].ComputationCosts[k] + DAG::DAG[ni][i]); }
				}
			}
			Maxmin = max(Maxmin, minPath);

		}

	}
	return Maxmin;
}


/*double IPPTS::rankPCM(int ni)
{
	double value = 0.0, total = 0.0;
	for (int k = 0; k < num_processor; k++) {
		total += PCM(ni, k);
	}
	value = (total / num_processor);
	return value;
}*/

double IPPTS::Prank(int ni)
{
	double value = 0.0, total = 0.0;
	double od = 1.0;
	for (int k = 0; k < num_processor; k++) {
		total += PCM(ni, k);
	}
	total /= num_processor;

	for(int i = 0; i < num_task; i++){
		if (DAG::DAG[ni][i] != 0)
			od += 1;
	}
	if( od > 1) 
		value = total * od;
	else
		value = total;
	
	return value;
}


void IPPTS::sort_Prank()
{
	double temp0;
	double temp1;
	for (int i = num_task - 1; i >= 0; i--)
	{
		DAG::task[i].Prankid = i + 1;
		DAG::task[i].Prank_size = Prank(i);
	}

	for (int i = 1; i < num_task; i++) //insertion sort
	{
		int j;
		temp0 = DAG::task[i].Prankid;
		temp1 = DAG::task[i].Prank_size;
		j = i - 1;
		while (temp1 > DAG::task[j].Prank_size)
		{
			DAG::task[j + 1].Prank_size = DAG::task[j].Prank_size;
			DAG::task[j + 1].Prankid = DAG::task[j].Prankid;
			j--;
			if (j == -1)
				break;
		}
		DAG::task[j + 1].Prank_size = temp1;
		DAG::task[j + 1].Prankid = temp0;
	}

}



void IPPTS::CreatDAG()
{
	task::rand_processor();
	DAG::DAG_generate();
}

void IPPTS::scheduling() {
	//CreatDAG();
	DAG::DAG_IPPTS_Test();
	DAG::DAG_print();
	int tasknow = 0;

	for (int i = 0; i < num_task; i++) {
		DAG::task[i].est.resize(num_processor);
		DAG::task[i].eft.resize(num_processor);
		DAG::task[i].lhet.resize(num_processor);
		DAG::task[i].Lhead.resize(num_processor);
		DAG::task[i].pcm.resize(num_processor);
	}

	for (int i = num_task-1; i >=0; i--) {
		for (int j = 0; j < num_processor; j++) {
			DAG::task[i].pcm[j] = PCM(i, j);
			cout << DAG::task[i].pcm[j] << " ";
		}cout << endl;
	}
	
	//cout << PCM(0,1) << " ";
	
	sort_Prank();

	std::cout << "IPPTS:  " << endl;
	std::cout << "task: processor: execution time:  " << endl;
	for (int i = 0; i < num_task; i++) {
		tasknow = DAG::task[i].Prankid - 1;
		//cout << tasknow + 1 << "  :";
		for (int j = 0; j < num_processor; j++) {
			DAG::task[tasknow].eft[j] = EFT(tasknow, j);//EFT
			DAG::task[tasknow].lhet[j] = PCM(tasknow, j)
			- DAG::task[tasknow].ComputationCosts[j];
			DAG::task[tasknow].Lhead[j] = PCM(tasknow, j)
			- DAG::task[tasknow].ComputationCosts[j] + EFT(tasknow, j);

			int min = DAG::task[tasknow].Lhead[0];
			DAG::task[tasknow].select_processor = 0;

			for (int k = 1; k < num_processor; k++) {
				if (DAG::task[tasknow].Lhead[k] < min) {
					min = DAG::task[tasknow].Lhead[k];
					DAG::task[tasknow].select_processor = k;
				}
			}
		}
		DAG::task[tasknow].AFT =
			DAG::task[tasknow].eft[DAG::task[tasknow].select_processor];
		processor[DAG::task[tasknow].select_processor].avail = DAG::task[tasknow].eft[DAG::task[tasknow].select_processor];

		std::cout << "n" << tasknow + 1 << "        " << "p" << DAG::task[tasknow].select_processor + 1
			<< "        " << DAG::task[tasknow].AFT << endl;
	}

	
}

void IPPTS::CreatDAG()
{
//	task::rand_processor();
	DAG::DAG_generate();
}

void IPPTS::scheduling() {
	clock_t  Begin = 0, End = 0;
	CreatDAG();
	//DAG::DAG_IPPTS_Test();
	DAG::DAG_print();
	int tasknow = 0;

	for (int i = 0; i < num_task; i++) {
		DAG::task[i].est.resize(num_processor);
		DAG::task[i].eft.resize(num_processor);
		DAG::task[i].lhet.resize(num_processor);
		DAG::task[i].Lhead.resize(num_processor);
		DAG::task[i].pcm.resize(num_processor);
	}
	Begin = clock();
	for (int i = num_task-1; i >=0; i--) {
		for (int j = 0; j < num_processor; j++) {
			DAG::task[i].pcm[j] = PCM(i, j);
			cout << DAG::task[i].pcm[j] << " ";
		}cout << endl;
	}
	
	//cout << PCM(0,1) << " ";
	
	sort_Prank();

	std::cout << "IPPTS_min:  " << endl;
	std::cout << "task: processor: execution time:  " << endl;
	for (int i = 0; i < num_task; i++) {
		tasknow = DAG::task[i].Prankid - 1;
		//cout << tasknow + 1 << "  :";
		for (int j = 0; j < num_processor; j++) {
			DAG::task[tasknow].eft[j] = EFT(tasknow, j);//EFT
			DAG::task[tasknow].lhet[j] = PCM(tasknow, j)
			- DAG::task[tasknow].ComputationCosts[j];
			DAG::task[tasknow].Lhead[j] = PCM(tasknow, j)
			- DAG::task[tasknow].ComputationCosts[j] + EFT(tasknow, j);

			int min = DAG::task[tasknow].Lhead[0];
			DAG::task[tasknow].select_processor = 0;

			for (int k = 1; k < num_processor; k++) {
				if (DAG::task[tasknow].Lhead[k] < min) {
					min = DAG::task[tasknow].Lhead[k];
					DAG::task[tasknow].select_processor = k;
				}
			}
		}
		DAG::task[tasknow].AFT =
			DAG::task[tasknow].eft[DAG::task[tasknow].select_processor];
		processor[DAG::task[tasknow].select_processor].avail = DAG::task[tasknow].eft[DAG::task[tasknow].select_processor];

		std::cout << "n" << tasknow + 1 << "        " << "p" << DAG::task[tasknow].select_processor + 1
			<< "        " << DAG::task[tasknow].AFT << endl;
	}
	End = clock();
	duration = double(End - Begin) / CLK_TCK;
	cout << "exe.time: " << duration << endl;
	
	ofstream File;
	File.open("outfile.csv", ios::out | ios::app);
	//oFile << "DAG_id" << "," << "number of tasks" << "," << "number of processors" << "," << "makespan" << "," << "execution time"<<endl;
	File <<"min" << "," << DAG::num_task << "," << DAG::num_processor << "," << DAG::task[(DAG::num_task - 1)].AFT << "," << duration << endl;
	File.close();

	
}