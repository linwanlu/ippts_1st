#pragma once
#include<iostream>
#include<stdlib.h>
#include <random>
#include <time.h>
#include<math.h>
#include<algorithm>
#include<fstream>
using namespace std;

class processor {
public:
	int avail;//earliest time at which processor pj is ready for task exe.
};

class task : processor {
public:
	vector <processor> processor;
	int id = 0;
	int num_processor = 0;
	int select_processor = 0;
	int AFT = 0;
	double AEST = 0.0;
	double ALST = 0.0;

	vector <int> parents;
	vector <int> children;
	vector <int> ComputationCosts;
	vector <int> eft;
	vector <int> est;

	void rand_processor();

	// HEFT
	int rankuid = 0;
	double Ranku_size = 0.0;

	// IPPTS
	int Prankid = 0;
	double Prank_size = 0.0;
	vector <int> lhet;
	vector <int> Lhead;
	vector <int> pcm;

};

class DAG : public task {
public:
	int num_task = 0;
	vector <task> task;
	vector <double> avg_ComputationCost;
	vector<vector<int>> DAG;
	void DAG_generate();
	void DAG_outfile();
	void DAG_Lookahead_Test();
	void DAG_IPEFT_Test();
	void DAG_PEFT_Test();
	void DAG_IPPTS_Test();
	void DAG_print();
private:
	int edge = 0, edgeCost = 0;
	double Wdag = 0.0;
	int SET_V[5] = { 20,40,60,80,100 };//number of tasks
	double SET_CCR[6] = { 0.1, 0.5, 1.0, 2.0, 5.0, 10.0 };
	double SET_alpha[3] = { 0.5, 1.0, 2.0 };//shape parameter of the graph
	int SET_outdegree[5] = { 1,2,3,4,5 };
	double SET_beta[5] = { 0.1, 0.25, 0.5, 0.75, 1.0 };//range percentage of computation costs on processors
};

class scheduler : public DAG {
public:
	virtual void CreatDAG() = 0;
	double ranku(int);
	void sort_ranku();
	void sort_print();
	int EST(int, int);
	int EFT(int, int);
	void AEST(int);
	void ALST(int);
};

void task::rand_processor()
{
	//srand(time(NULL));
	//num_processor = 2 + (rand() % (32 - 2 + 1));
	num_processor = 3;
	//cout << "num_processor:" << num_processor << endl;
	processor.resize(num_processor);
}

void DAG::DAG_generate() {
	rand_processor();
	int V, outdegree, height;
	//random paramenters
	double CCR, alpha, beta;
	double mean_value;
	srand(time(NULL));
	//V=SET_V[rand()%4];
	V = 30;
	//CCR=SET_CCR[rand()%5];
	CCR = 2.0;
	alpha = SET_alpha[rand() % 2];
	//beta = SET_beta[rand() % 4];
	beta = 1.0;

	//shape of DAG
	height = (int)floor(sqrt(V) / alpha);
	mean_value = alpha * sqrt(V);
	std::mt19937 generator;//randnum generator
	std::normal_distribution<double> distribution(mean_value, beta);//random width for each level
	//cout<<"height:"<<height<<endl;
	//cout<<endl;

	int generate_num = 0, lastadd_num = 0;

	for (int i = 0; i <= height; i++) {
		int new_nodes = (int)ceil(distribution(generator));
		//std::cout << "new_nodes:" << new_nodes << endl;
		if (i == 0 && new_nodes != 1) { //add start task
			generate_num = 1;
			lastadd_num = 1;
			DAG.resize(generate_num + new_nodes);
			task.resize(generate_num + new_nodes);
			//cout<<"tasksize:"<<DAG.size()<<endl;
			//cout<<endl;
		}
		//cout<<"generate_num:"<<generate_num<<endl;
		//cout<<"lastadd_num:"<<lastadd_num<<endl;
		//cout<<"new_nodes:"<<new_nodes<<endl;
		if (generate_num + new_nodes <= V) {
			DAG.resize(generate_num + new_nodes);
			task.resize(generate_num + new_nodes);
			//cout<<"tasksize:"<<DAG.size()<<endl;
			//cout<<endl;
		}
		else {
			new_nodes = V - generate_num;
			DAG.resize(generate_num + new_nodes);
			task.resize(generate_num + new_nodes);
			//	cout<<"tasksize:"<<DAG.size()<<endl;
			//	cout<<endl;
		}
		if (new_nodes != 0) {
			for (int j = 0; j < DAG.size(); j++) {
				DAG[j].resize(DAG.size(), 0);
				if (j >= generate_num - lastadd_num && j < generate_num) {
					outdegree = SET_outdegree[rand() % 4];//set outdegree of each task 
					if (outdegree > new_nodes) {
						outdegree = outdegree - new_nodes;
					}
					//cout<<"outdegree:"<<outdegree<<endl;
					if (new_nodes > 0) {
						for (int k = 0; k < outdegree; k++) {//set communication cost
							int child = rand() % new_nodes;
							//cout<<"child:"<<child<<endl;
							if (DAG[j][generate_num + child] == 0) {
								DAG[j][generate_num + child] = 5 + (rand() % 80);
								edge++;
								edgeCost += DAG[j][generate_num + child];
								//cout<<"randnum:"<<DAG[j][generate_num+child]<<endl;
							}
							task[generate_num + child].parents.push_back(j + 1);
							task[j].children.push_back(generate_num + child + 1);
							//cout<<"taskparents:"<<task[generate_num+child].parents.size()<<endl;
						}
					}
				}
			}
			if (generate_num > 0) {
				for (int i = generate_num; i < generate_num + new_nodes; i++) {
					if (task[i].parents.size() == 0) {
						int randtask = rand() % lastadd_num;
						int temp = generate_num - lastadd_num + randtask;
						DAG[temp][i] = 5 + (rand() % 80);
						edge++;
						edgeCost += DAG[temp][i];
						task[i].parents.push_back(temp + 1);
						task[temp].children.push_back(i + 1);
					}
				}
			}

			lastadd_num = new_nodes;
			//std::cout << "lastadd_num:" << lastadd_num << endl;
			generate_num = generate_num + new_nodes;
			//std::cout << "generate_num:" << generate_num << endl;
		}
	}
	//std::cout << "height:" << height << endl;



	num_task = task.size();
	for (int i = 0; i < num_task; i++) {
		task[i].id = i + 1;
	}
	//std::cout << "num_task:" << num_task << endl;

	//cout << "tasksize:" << task.size() << endl;
	/*
	for (int i = 0; i < DAG.size(); i++) {
		std::cout << "t[" << task[i].id << "]->";
		vector<int>::iterator it;
		for (it = task[i].children.begin(); it != task[i].children.end(); it++)
			std::cout << *it << " ";
		std::cout << endl;
	}
	*/


	//add EXIT node
	int temp = 0;
	for (int i = 0; i < task.size(); i++) {
		if (task[i].children.empty() == true) {
			temp++;
		}
	}
	//std::cout << "temp:" << temp << endl;

	if (temp > 1) {
		//std::cout << "add exit" << endl;
		DAG.resize(generate_num + 1);
		task.resize(generate_num + 1);
		for (int j = 0; j < DAG.size(); j++) {
			DAG[j].resize(DAG.size(), 0);
			if (j >= generate_num - lastadd_num && j < DAG.size() - 1) {
				DAG[j][generate_num] = 5 + (rand() % 80);
				edge++;
				edgeCost += DAG[j][generate_num];
				task[j].parents.push_back(generate_num);
			}
		}
		num_task = task.size();
		for (int i = 0; i < num_task; i++) {
			task[i].id = i + 1;
		}
	}


	/*std::cout << "num_task:" << num_task << endl;
	std::cout << "DAG:" << endl;
	for (int i = 0; i < num_task; i++) {
		for (int j = 0; j < num_task; j++) {
			std::cout << DAG[i][j] << " ";
		}
		std::cout << endl;
	}
	*/


	//set average computation cost of each task ni in the graph
	std::cout << endl;
	double avg_CommunicationCost = edgeCost / edge;
	//cout << "avg_CommunicationCost:" << avg_CommunicationCost << endl;
	//cout << "CCR:" << CCR << endl;
	Wdag = avg_CommunicationCost / CCR;
	//cout << "Wdag:" << Wdag << endl;
	avg_ComputationCost.resize(num_task);
	//std::cout << "ComputationCosts:" << endl;
	for (int i = 0; i < num_task; i++) {
		std::uniform_real_distribution<double> unif_wi(5.0, 2 * Wdag);
		avg_ComputationCost[i] = unif_wi(generator);

		task[i].ComputationCosts.resize(num_processor);
		int MIN = avg_ComputationCost[i] * (1 - (beta / 2));
		int MAX = avg_ComputationCost[i] * (1 + (beta / 2));
		for (int j = 0; j < num_processor; j++) {
			std::uniform_int_distribution<int> unif_wij(MIN, MAX);
			task[i].ComputationCosts[j] = unif_wij(generator);
			for (int k = 0; k < j; k++) {
				while (task[i].ComputationCosts[k] == task[i].ComputationCosts[j])
					task[i].ComputationCosts[j] = unif_wij(generator);
			}
			//std::cout << task[i].ComputationCosts[j] << "      ";
		}
		//std::cout << endl;
	}

	double total = 0.0;
	std::cout << endl;

	for (int i = 0; i < num_task; i++)
	{
		for (int j = 0; j < num_processor; j++)
		{
			total = task[i].ComputationCosts[j] + total;
		}
		//cout<<(double)total/P<< endl;
		avg_ComputationCost[i] = total / num_processor;
		total = 0.0;
		//std::cout << "avg_ComputationCost[" << i + 1 << "]:" << avg_ComputationCost[i] << endl;
	}
}

void DAG::DAG_outfile()
{
	ofstream file;
	file.open("DAG_random.txt", ios::out | ios::app);
	file << "num_task:" << num_task << endl;
	for (int i = 0; i < num_task; i++) {
		file << "DAG[" << i << "] = { ";
		for (int j = 0; j < num_task; j++) {
			if (j != num_task - 1) file << DAG[i][j] << ", ";
			else file << DAG[i][j] << " ";
		}
		file << "};";
		file << endl;
	}
	file << endl;
	file << "ComputationCosts:" << endl;
	for (int i = 0; i < num_processor; i++) {
		file << "p" << i + 1 << ":    ";
	}file << endl;
	for (int i = 0; i < num_task; i++) {
		file << "task[" << i << "].ComputationCosts = {";
		for (int j = 0; j < num_processor; j++) {
			if (j != num_processor - 1) file << task[i].ComputationCosts[j] << ", ";
			else file << task[i].ComputationCosts[j] << " ";
		}
		file << "};";
		file << endl;
	}
	file << endl;
	file.close();
}


void DAG::DAG_IPPTS_Test()
{
	num_task = 10;
	task::num_processor = 3;
	task::processor.resize(num_processor);
	DAG.resize(10);
	task.resize(10);
	for (int i = 0; i < num_task; i++) {
		DAG[i].resize(10);
		task[i].id = i + 1;
		task[i].ComputationCosts.resize(num_processor);
	}
	DAG[0] = { 0,43,89,34,0,0,0,0,0,0 };
	DAG[1] = { 0,0,0,0,85,0,0,0,0,0 };
	DAG[2] = { 0,0,0,0,0,67,0,0,0,0 };
	DAG[3] = { 0,0,0,0,0,0,58,0,0,0 };
	DAG[4] = { 0,0,0,0,0,0,0,46,0,0 };
	DAG[5] = { 0,0,0,0,0,0,0,0,63,0 };
	DAG[6] = { 0,0,0,0,0,0,0,0,59,0 };
	DAG[7] = { 0,0,0,0,0,0,0,0,0,77 };
	DAG[8] = { 0,0,0,0,0,0,0,0,0,19 };
	DAG[9] = { 0,0,0,0,0,0,0,0,0,0 };

	for (int i = 0; i < num_task; i++) {
		for (int j = 0; j < num_task; j++) {
			if (DAG[i][j] != 0) {
				task[j].parents.push_back(i + 1);
				task[i].children.push_back(j + 1);
			}
		}
	}
	task[0].ComputationCosts = { 64,26,18 };
	task[1].ComputationCosts = { 2,4,16 };
	task[2].ComputationCosts = { 7,17,9 };
	task[3].ComputationCosts = { 27,29,43 };
	task[4].ComputationCosts = { 84,68,17 };
	task[5].ComputationCosts = { 10,52,87 };
	task[6].ComputationCosts = { 48,37,8 };
	task[7].ComputationCosts = { 26,77,75 };
	task[8].ComputationCosts = { 21,20,5 };
	task[9].ComputationCosts = { 10,4,11 };


	//std::cout << endl;
	avg_ComputationCost.resize(num_task);
	double total = 0.0;
	for (int i = 0; i < num_task; i++)
	{
		for (int j = 0; j < num_processor; j++)
		{
			total = task[i].ComputationCosts[j] + total;
		}
		//cout<<(double)total/P<< endl;
		avg_ComputationCost[i] = total / num_processor;
		total = 0.0;
		//std::cout << "avg_ComputationCost[" << i + 1 << "]:" << avg_ComputationCost[i] << endl;
	}
	//std::cout << endl;
}
void DAG::DAG_print()
{
	std::cout << "num_task:" << num_task << endl;
	std::cout << endl;
	std::cout << "DAG:" << endl;
	for (int i = 0; i < num_task; i++) {
		for (int j = 0; j < num_task; j++) {
			std::cout << DAG[i][j] << " ";
		}
		std::cout << endl;
	}
	std::cout << endl;
	std::cout << "ComputationCosts:" << endl;
	for (int i = 0; i < num_processor; i++) {
		std::cout << "p" << i + 1 << ":    ";
	}std::cout << endl;
	for (int i = 0; i < num_task; i++) {
		for (int j = 0; j < num_processor; j++) {
			std::cout << task[i].ComputationCosts[j] << "     ";
		}
		std::cout << endl;
	}
	for (int i = 0; i < num_task; i++)
	{
		std::cout << "avg_ComputationCost[" << i + 1 << "]:" << avg_ComputationCost[i] << endl;

	}
	std::cout << endl;
}


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

void scheduler::sort_ranku()
{
	double temp0;
	double temp1;
	for (int i = num_task - 1; i >= 0; i--)
	{
		DAG::task[i].rankuid = i + 1;
		DAG::task[i].Ranku_size = ranku(i);
		//cout<<Rank_size[i][0]+1<<" "<<Rank_size[i][1]<< endl;
	}

	for (int i = 1; i < num_task; i++) //insertion sort
	{
		int j;
		temp0 = DAG::task[i].rankuid;
		temp1 = DAG::task[i].Ranku_size;
		j = i - 1;
		while (temp1 > DAG::task[j].Ranku_size)
		{
			DAG::task[j + 1].Ranku_size = DAG::task[j].Ranku_size;
			DAG::task[j + 1].rankuid = DAG::task[j].rankuid;
			j--;
			if (j == -1)
				break;
		}
		DAG::task[j + 1].Ranku_size = temp1;
		DAG::task[j + 1].rankuid = temp0;
	}

}

void scheduler::sort_print()
{
	std::cout << endl;
	std::cout << "ranku:" << endl;
	for (int i = 0; i < num_task; i++)
	{
		std::cout << "n" << DAG::task[i].rankuid << "  " << DAG::task[i].Ranku_size << endl;
	}
	std::cout << endl;
}

int scheduler::EST(int ni, int pj)
{
	int Rtime = 0;
	for (int i = 0; i < num_task; i++)
	{
		if (DAG::DAG[i][ni] != 0)
		{
			if (DAG::task[i].select_processor == pj) {
				//cout << "Rtime: " << Rtime << " AFT: " << i << " " << DAG::task[i].AFT << endl;
				Rtime = max(Rtime, DAG::task[i].AFT);
			}
			else {
				//cout << "Rtime: " << Rtime << " AFT+communi: " << i << " " << DAG::task[i].AFT << "�@" << DAG::DAG[i][ni] << endl;
				Rtime = max(Rtime, DAG::task[i].AFT + DAG::DAG[i][ni]);
			}
		}
	}
	Rtime = max(processor[pj].avail, Rtime);
	DAG::task[ni].est[pj] = Rtime;
	//cout << "n" << ni+1 << "p" << pj +1<< " Rtime: " << Rtime << endl;
	return Rtime;

}

int scheduler::EFT(int ni, int pj)
{
	int eftTEMP = DAG::task[ni].ComputationCosts[pj] + EST(ni, pj);
	//cout << "n" << ni << "p" << pj << " eft: " << eftTEMP << endl;
	return eftTEMP;
}

void scheduler::AEST(int ni)
{
	double Rtime = 0.0;//tast ni�ثe�̤jAEST�ƭ�
	for (int i = 0; i < num_task; i++)
	{
		if (DAG::DAG[i][ni] != 0) {
			Rtime = max(Rtime, DAG::task[i].AEST + avg_ComputationCost[i] + DAG::DAG[i][ni]);
		}
	}
	//cout << "n" << ni <<" AEST: " << Rtime << endl;
	DAG::task[ni].AEST = round(Rtime * 100) / 100;
}

// Actual Start time
void scheduler::ALST(int ni)
{
	bool isExit = 1;//�T�{�O�_��Exit node.
	bool isEntry = 1;
	double Rtime = 0.0;
	for (int i = 0; i < num_task; i++)
	{
		if (DAG::DAG[i][ni] != 0) {
			isEntry = 0;
		}
		if (DAG::DAG[ni][i] != 0) {
			isExit = 0;//�D Exit node
			if (Rtime == 0.0) {
				Rtime = DAG::task[i].ALST - DAG::DAG[ni][i];
			}
			else {
				Rtime = min(Rtime, DAG::task[i].ALST - DAG::DAG[ni][i]);
			}
		}
	}
	if (isExit == 1 && isEntry == 0) {
		Rtime = DAG::task[ni].AEST;
	}
	else if (isExit == 0 && isEntry == 0) {
		Rtime = Rtime - avg_ComputationCost[ni];
	}
	else if (isExit == 0 && isEntry == 1) {
		Rtime = 0.0;
	}


	//cout << "n" << ni <<" ALST: " << Rtime << endl;
	DAG::task[ni].ALST = round(Rtime * 100) / 100;
}
