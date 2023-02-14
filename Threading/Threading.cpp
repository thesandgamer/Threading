#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using std::cout;
using std::endl;


int SumList(vector<int> list)
{
	int sum{ 0 };
	for (int var : list)
	{
		sum += var;
	}
	return sum;
}

int main() {

	vector<int> list{ 1,2,3,4,5,6,7};
	int sum {0};


	
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
		sum += SumList(var);
	}


	/*
	for (int var : list)
	{
		sum += var;
	}
	*/

	cout << sum << endl;

	return 0;

}