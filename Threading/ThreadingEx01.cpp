#include <iostream>
#include <thread>
#include <vector>

using namespace std;
using std::cout;
using std::endl;


const int MatrixSize = 4;

int CalculateMatrixNumber(const vector<int> x, const vector<int> y)	//x = première matrice en vertkcal y = deuxièle matrice
{
	int finalInt = NULL;
	for (size_t i = 0; i < MatrixSize; i++)//Calcul chaque valeur
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


	for (size_t x = 0; x < MatrixSize; x++) // 
	{
		for (size_t y = 0; y < MatrixSize; y++) // 
		{
			matCol.clear();//On clear mat col pour pas qu'il en reste du passage précédant

			for (size_t i = 0; i < MatrixSize; i++)	//Fait une copie des valeur de colone de la 2 ème matrice
			{
				matCol.push_back(Matrix2[i][x]);
			}
			const vector<int> ligne(Matrix1[y], Matrix1[y] + sizeof(Matrix1[y]) / sizeof(Matrix1[y][0]));//Fait une copie de la ligne y de la matrice 1

			threads.emplace_back(	//On ajoute la fonction dans la liste de threads
				([=, &FinalMatrix]() {		//On passe le tout par copie, sauf pour la valeur final matrix	
					FinalMatrix[y][x] = CalculateMatrixNumber(ligne, matCol);//On set la valeur de la matrice au résultat 

					})
			);


		}
	}

	for (size_t i = 0; i < threads.size(); i++)//On rejoins tout les threads
	{
		threads[i].join();
	}
	cout << endl;


	return 0;

}