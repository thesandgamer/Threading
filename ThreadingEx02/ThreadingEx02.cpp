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
#pragma region Sum a list

int GLOBALSUM;
std::mutex m;

int SumList(vector<int> list)
{
	int sum{ 0 };
	for (int var : list)
	{
		sum += var;
	}
	GLOBALSUM += sum;

	return sum;
}

int SumListWithoutMutex(vector<int> list)
{
	return SumList(list);
}

int SumListWithMutex(vector<int> list)
{
	std::lock_guard<std::mutex> guard(m);
	return SumList(list);
}


vector<vector<int>> DivideList(vector<int>& list, int dividingNumber = 3)
{
	int old = 0;
	vector<vector<int>> sublists;

	for (size_t i = 0; i < dividingNumber; i++)
	{
		vector<int> sublist;
		float endNumber = (i + 1) * (float)((float)list.size() / dividingNumber);
		sublist.assign(list.begin() + old, list.begin() + endNumber);
		sublists.push_back(sublist);
		old = endNumber;
	}
	return sublists;
}

#pragma endregion

//----------------------------------------------------//

//------------]To display odd and even numbers[-------//
#pragma region Odd And Evens Number

int i = -1; //variable globale pour que les boucles sont sync
std::mutex m2;
void DisplayEvensNumbers()
{
	//cout << "Display evens number with mutex:" << "\n";
	while ( i < 1000 )
	{
		if (i % 2 == 0)
		{
			std::lock_guard<std::mutex> guard(m2);//On lock sur le même mutex que celui d'avant

			i++;
		
			cout << "Function1: ";
			cout << i << "\n";
		}
	}
	cout << endl;
}

void DisplayOddNumbers()
{
	//cout << "Display odd number with mutex:" << "\n";

	while (i < 1000)
	{
		if (i % 2 != 0)
		{
			std::lock_guard<std::mutex> guard(m2);//On lock sur le même mutex que celui d'avant

			i++;
		
			cout << "Function2: ";
			cout << i << "\n";

		}
	}
	cout << endl;
}

#pragma endregion
//----------------------------------------------------//


int main() {

	vector<int> list{ 1,2,3,4,5,6,7};

	int sum {0};//Total
	vector <std::thread> threads;//To store the threads for sum


	//Divide List
	vector<vector<int>> sublists = DivideList(list);


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
	sum = 0;
	for (int var : list)
	{
		sum += var;
	}
	*/

	cout <<"Sum with local variable: " << sum << "\n";
	cout <<"Sum with global variable: " << GLOBALSUM << "\n" << endl;

	thread t1(DisplayEvensNumbers);
	thread t2(DisplayOddNumbers);

	t1.join();
	t2.join();

	

	return 0;

}