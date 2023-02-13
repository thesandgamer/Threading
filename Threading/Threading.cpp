#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using std::cout;
using std::endl;

const int MatrixSize = 4;

/*
class Task {
public:

	//Execution operator = operator ()
	void operator()() const {
		doSomething();
		doSomethingElse();
	}

private:

	void doSomething() const {
		cout << "Something" << "\n";
	}
	void doSomethingElse() const {
		cout << "Something else" << "\n";
	}
};
void doFunction() { cout << "Function" << "\n"; }
*/

/*
int CalculateMatrixNumber(const int* x, const int* y)	//x = première matrice en vertkcal y = deuxièle matrice
{
	
	cout << x[0] << " * " << y[0] << " +" << "\n";
	cout << x[1] << " * " << y[1] << " +" << "\n";
	cout << x[2] << " * " << y[2] <<"\n";


	cout <<" = " << x[0] * y[0] + x[1] * y[1] + x[2] * y[2];
	cout << "\n";
	cout << "\n";

	return x[0] * y[0] + x[1] * y[1] + x[2] * y[2];

};*/

int CalculateMatrixNumber(const vector<int> x, const vector<int> y)	//x = première matrice en vertkcal y = deuxièle matrice
{
	int finalInt = NULL; 
	for (size_t i = 0; i < MatrixSize; i++)
	{
		cout << x.at(i) << " * " << y.at(i) << " +" << "\n";
		finalInt += x.at(i) * y.at(i);
	}
	cout << finalInt << "\n" << "\n";

	return finalInt;

};

int main() {

	/*
	const int Matrix1[3][3] = {
   {1, 2, 3,} ,  
   {4, 5, 6,} ,  
   {7, 8, 9}
						};
	const int Matrix2[3][3] = {
   {10, 11, 12,} ,
   {13, 14, 15,} ,
   {16, 17, 18}
	};*/

	const int Matrix1[4][4] = {
   {1, 2, 3, 4 } ,
   {5, 6, 7, 8} ,
   {9 , 10 , 11, 12},
   {13 , 14 , 15, 16},
	};
	const int Matrix2[4][4] = {
   {17, 18, 19,20} ,
   {21, 22, 23,24} ,
   {25, 26, 27,28},
   {29, 30, 31,32},
	};

	vector<vector<int>> FinalMatrix{ MatrixSize, vector<int>(MatrixSize) };


	vector <std::thread> threads;

	vector<int> matCol;


	for (size_t x = 0; x < MatrixSize; x++) // Calculate the final position of maxtrix
	{
		for (size_t y = 0; y < MatrixSize; y++) // Calculate the final position of maxtrix
		{
			for (size_t i = 0; i < MatrixSize; i++)
			{
				matCol.push_back(Matrix2[i][x]);
			}
			const vector<int> ligne(Matrix1[y], Matrix1[y] + sizeof(Matrix1[y]) / sizeof(Matrix1[y][0]));
			
			threads.emplace_back(
				([=, &FinalMatrix]() {			
					FinalMatrix[y][x] = CalculateMatrixNumber(ligne, matCol);

					})
			);
			matCol.clear();

			
		}
	}
	
	for (size_t i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}
	cout << endl;





	/*
	// Thread with a class that implements () operator (uses rvalue)
	std::thread t0{ Task() };	//Créer un thread pour une classe : appel l'operator ()
	// Thread with lambda
	std::thread t1([] {			//Créer un thread pour une fonction
		doFunction();

		});
	// Launch several times your program to remark the order is not deterministic
	t0.join();
	t1.join();
	*/
	return 0;

}