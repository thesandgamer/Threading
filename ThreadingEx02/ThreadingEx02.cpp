#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <list>
#include <chrono>

using namespace std;
using std::cout;
using std::endl;
using std::chrono::high_resolution_clock;



//------------]To add add element in a list[-------------//
int FinalSum;
std::mutex m;

int SumList(vector<int> list)
{
	std::lock_guard<std::mutex> guard(m);
	int sum{ 0 };
	for (int var : list)
	{
		sum += var;
	}
	FinalSum += sum;

	return sum;
}
//----------------------------------------------------//


std::mutex m2;
void DisplayEvensNumbers()
{
	std::lock_guard<std::mutex> guard(m2);

	for (size_t i = 0; i <= 1000; i++)
	{
		if (i % 2 == 0)
		{
			cout << i << "\n";
		}
	}
}


int main() {

	vector<int> list{ 1,2,3,4,5,6,7};
	int sum {0};//Total

	vector <std::thread> threads;//To store the threads for sum


	//Divide List
	int dividingNumber = 3; //En combien de sous table on divise la liste
	vector<vector<int>> sublists;
	int old = 0;
	for (size_t i = 0; i < dividingNumber; i++)
	{
		vector<int> sublist;
		float endNumber =(i+1) * (float) ((float)list.size() / dividingNumber);
		sublist.assign(list.begin() + old, list.begin() + endNumber);
		sublists.push_back(sublist);
		old = endNumber;
	}

	//Get Sum for all sub lits
	for (vector<int> var : sublists)
	{
		threads.emplace_back(
			([=, &sum]() {
				sum += SumList(var);
				})
		);
		
	}

	for (size_t i = 0; i < threads.size(); i++)//On rejoins tout les threads
	{
		threads[i].join();
	}

	auto timeStart = high_resolution_clock::now();
	//Mettre ici la fonction à voir le temps
	auto timeEnd = high_resolution_clock::now();



	/*
	for (int var : list)
	{
		sum += var;
	}
	*/

	cout << sum << "\n";	
	cout << FinalSum << "\n" << endl; //The sum with mutex

	thread t1(DisplayEvensNumbers);
	thread t2(DisplayEvensNumbers);

	t1.join();
	t2.join();

	

	return 0;

}