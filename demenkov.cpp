#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <complex>
#include "knpo.h"

using namespace std;
int main(int argc, char* argv[])
{ 
		//Проверка ввода
		if (argc < 3) {
			cout << "Insufficient number of command line arguments";
			return 0;
		}

		string inputFile(argv[1]);
		string outputFile(argv[2]);
		
		char buff[MAXNUM_STR][MAXSIZE_STR]; //Буффер для получения входных данных
		//Получение данных из входного файла
		inputData(buff, MAXNUM_STR, inputFile);


		//Блок объявления используемых структур
		char chain[MAXNUM_STR][MAXSIZE_STR]; //Последовательность элементов эл.цепи
		complex<double> matrix[MAXNUM_STR][MAXSIZE_STR]; //Осоновная матрица
		complex<double> personalMatrix[MAXNUM_STR][MAXSIZE_STR]; //Побочная матрица
		float R[20] = { 0 }; //Массив сопротивлений
		float C[20] = { 0 }; //Массив емкостей
		float L[20] = { 0 }; //Массив резисторов
		int V = 0; //Частота
		int U = 0; //Наряжение
		int resistCount = 0; //Кол-во резисторов
		int condenCount = 0; //Кол-во конденсаторов
		int coilCount = 0; //Кол-во катушек
		int branchCount = 0; //Кол-во ветвей
		complex <double> Z[MAXNUM_STR]; //Полные сопротивления ветвей
		complex <double> determinant[MAXNUM_STR]; //Определители матриц
		complex <double> currentValues[MAXNUM_STR]; //Значения токов
		int Rk = 0; //Номер ветви в которой встречается резистор впервые
		int Ck = 0; //Номер ветви в которой встречается конденсатор впервые
		int Lk = 0; //Номер ветви в которой встречается катушка впервые
		//Упорядочивание входных данных
		orderingInputData(buff, MAXNUM_STR, R, L, C, &V, &U, &resistCount, &condenCount, &coilCount, chain, &Rk, &Lk, &Ck, &branchCount);

		//Вычислить полное сопротивление всех ветвей
		fullResistanceOfBranch(Z, R, C, L, V, Rk, Ck, Lk, branchCount);
		int num = 0;
		if (branchCount > 1)
		{
			//Создать основную матрицу
			createMainMatrix(matrix, MAXNUM_STR, Z, branchCount);
			//Посчитать определитель основной матрицы
			determinantOfMatrix(determinant, matrix, num, branchCount);

			//Для каждой побочной матрицы
			for (num = 0; num < branchCount; num++)
			{
				//Создать побочную матрицу
				createPersonalMatrix(personalMatrix, Z, U, num, branchCount);
				//Посчитать ее определитель
				determinantOfMatrix(determinant, personalMatrix, num + 1, branchCount);
			}
			//Вычислить значения токов в ветвях
			currentValue(determinant, currentValues, branchCount);
		}
		else
		{
			//Вычислить эл. ток в цепи с одной ветвью 
			complex <double> u(U, 0);
			currentValues[0] = u / Z[0];
		}

		//Вывод полученных значений в текстовый файл
		outputCurrentValues(currentValues, branchCount, outputFile);
}


/*!	Осуществить считывание входных данных из файла */
void inputData(char (*buff)[MAXSIZE_STR], size_t n, string inputFile) {
	ifstream fin(inputFile);
	if (fin.is_open())
	{
		for (int i = 0; i < 20; i++)
		{
			fin.getline(buff[i], 81);
		}
		fin.close();
	}
	else
	{
		cout << ("whoops");
	}
}


/*!	Упорядочить полученные на входе значения, т.е присвоить переменным нужные значения в нужном формате */
void orderingInputData(char (*buff)[MAXSIZE_STR],size_t n, float* R, float* L, float* C, int* V, int* U, int *resistCount,
	int* condenCount, int* coilCount, char (*chain)[MAXSIZE_STR], int *Rk, int *Lk, int *Ck, int *branchCount){

	//Преобразование данных
	char buffer[20];
	*V = atof(&buff[0][2]); //Частота
	*U = atof(&buff[1][2]); //Напряжение
	strcpy_s(buffer, &buff[2][12]);
	*resistCount = atoi(buffer); //Количество резисторов
	strcpy_s(buffer, &buff[3][12]);
	*condenCount = atoi(buffer); //Количество конденсаторов
	strcpy_s(buffer, &buff[4][10]);
	*coilCount = atoi(buffer); //Количество катушек
	*Rk = atoi(&buff[5][1]); //Номер ветви в которой встречается резистор впервые
	n = *Rk;
	for (int i = 5; i < 5 + *resistCount; i++) //Определить величины сопротивления резисторов
	{
		R[n-1] = atof(&buff[i][3]);
		n++;
	}
	*Ck = atoi(&buff[*resistCount+5][1]); //Номер ветви в которой встречается конденсатор впервые
	n = *Ck;
	for (int i = *resistCount + 5; i < 5 + *resistCount + *condenCount; i++) //Определить величины емкости конденсаторов
	{
		C[n-1] = atof(&buff[i][3]);
		n++;
	}
	*Lk = atoi(&buff[*resistCount + *condenCount + 5][1]); //Номер ветви в которой встречается катушка впервые
	n = *Lk;
	for (int i = *resistCount + *condenCount + 5; i < 5 + *resistCount + *condenCount + *coilCount; i++) //Определить величины индуктивности катушек
	{
		L[n-1] = atof(&buff[i][3]);
		n++;
	}

	int k = 0;
	for (int i = *resistCount + *condenCount + *coilCount + 5; i < 20; i++)
	{
		strcpy_s(chain[k], buff[i]);
		k++;

	}


	//Определить количество ветвей в цепи
	if (strlen(chain[0]) < 4)
		*branchCount = 1;
	else if (strlen(chain[0]) == 5)
		*branchCount = 2;
	else if (strlen(chain[0]) == 8)
		*branchCount = 3;
}


/*! Получить полные сопротивления каждой ветви */
void fullResistanceOfBranch(complex<double>* Z, float* R, float* C, float* L, int V, int Rk, int Ck, int Lk, int branchCount) {
	float Xl[MAXSIZE_STR]; //Массив реактивных сопротивлений катушек
	float Xc[MAXSIZE_STR]; //Массив реактивных сопротивлений конденсаторов
	float Xlc[MAXSIZE_STR]; //Массив разниц реактивных сопротивлений катушек и конденсаторов,возведенных в квадрат
	//Для каждой катушки
	for (int i = 0; i < branchCount; i++)
	{
		Xl[i] = L[i] * 2 * PI * V; //Вычислить реактивное сопротивление
	}
	//Для каждого конденсатора
	for (int i = 0; i < branchCount; i++)
	{
		if (C[i] != 0) 
		{
			Xc[i] = 1 / (C[i] * 2 * PI * V); //Вычислить реактивное сопротивление
		}
		else 
		{
			Xc[i] = 0;
		}
		
	}

	//Вычислить разницу реактивных сопротивлений катушек и конденсаторов и возвести в квадрат
	for (int i = 0; i < branchCount; i++)
	{
		Xlc[i] = pow((Xl[i] - Xc[i]), 2);
	}

	//Для каждой ветви вычислить полное сопротивление
	for (int i = 0; i < branchCount; i++)
	{
		Z[i] = sqrt(complex<double>(pow(R[i],2), Xlc[i]));
		if (Z[i].real() - Z[i].imag() < 0.001)
		{
			Z[i].real(0);
		}
	}

}


/*!	Создать основную матрицу для вычисления ее определителя */
void createMainMatrix(complex<double> (*matrix)[MAXSIZE_STR],size_t n, complex<double>* Z, int branchCount)
{
	//Если количество ветвей = 3
	if (branchCount == 3)
	{
		//Заполнить первую строку матрицы
		matrix[0][0] = complex<double>(1, 0);
		matrix[0][1] = complex<double>(1, 0);
		matrix[0][2] = complex<double>(-1, 0);

		//Заполнить вторую строку матрицы
		matrix[1][0] = Z[0];
		matrix[1][1] = Z[1];
		matrix[1][2] = complex<double>(0, 0);

		//Заполнить третью строку матрицы
		matrix[2][0] = complex<double>(0, 0);
		matrix[2][1] = Z[1];
		matrix[2][2] = Z[2];
	}

	//Если количество ветвей = 2
	else if (branchCount == 2)
	{
		//Заполнить первую строку матрицы
		matrix[0][0] = complex<double>(1, 0);
		matrix[0][1] = complex<double>(1, 0);
		
		//Заполнить вторую строку матрицы
		matrix[1][0] = Z[0];
		matrix[1][1] = Z[1];
	}
}


/*!	Создать частную матрицу для вычисления ее определителя */
void createPersonalMatrix(complex<double>(*matrix)[MAXSIZE_STR], complex<double>* Z, float U, int num, int branchCount)
{
	//Если кол-во ветвей = 3
	if (branchCount == 3)
	{
		//Создать матрицу с номером 0
		if (num == 0)
		{
			//Заполнить первую строку матрицы
			matrix[0][0] = complex<double>(0, 0);
			matrix[0][1] = complex<double>(1, 0);
			matrix[0][2] = complex<double>(-1, 0);

			//Заполнить вторую строку матрицы
			matrix[1][0] = complex<double>(U, 0);
			matrix[1][1] = Z[1];
			matrix[1][2] = complex<double>(0, 0);

			//Заполнить третью строку матрицы
			matrix[2][0] = complex<double>(0, 0);
			matrix[2][1] = Z[1];
			matrix[2][2] = Z[2];
		}
		//Создать матрицу с номером 1
		else if (num == 1)
		{
			//Заполнить первую строку матрицы
			matrix[0][0] = complex<double>(1, 0);
			matrix[0][1] = complex<double>(0, 0);
			matrix[0][2] = complex<double>(-1, 0);

			//Заполнить вторую строку матрицы
			matrix[1][0] = Z[0];
			matrix[1][1] = complex<double>(U, 0);
			matrix[1][2] = complex<double>(0, 0);

			//Заполнить третью строку матрицы
			matrix[2][0] = complex<double>(0, 0);
			matrix[2][1] = complex<double>(0, 0);
			matrix[2][2] = Z[2];
		}
		//Создать матрицу с номером 1
		else if (num == 2)
		{
			//Заполнить первую строку матрицы
			matrix[0][0] = complex<double>(1, 0);
			matrix[0][1] = complex<double>(1, 0);
			matrix[0][2] = complex<double>(0, 0);

			//Заполнить вторую строку матрицы
			matrix[1][0] = Z[0];
			matrix[1][1] = Z[1];
			matrix[1][2] = complex<double>(U, 0);

			//Заполнить третью строку матрицы
			matrix[2][0] = complex<double>(0, 0);
			matrix[2][1] = Z[1];
			matrix[2][2] = complex<double>(0, 0);
		}
	}
	//Если кол-во ветвей = 2
	else if (branchCount == 2)
	{
		//Создать матрицу с номером 0
		if (num == 0)
		{
			//Заполнить первую строку матрицы
			matrix[0][0] = complex<double>(0, 0);
			matrix[0][1] = complex<double>(1, 0);

			//Заполнить вторую строку матрицы
			matrix[1][0] = complex<double>(U, 0);
			matrix[1][1] = Z[1];

		}
		//Создать матрицу с номером 1
		else if (num == 1)
		{
			//Заполнить первую строку матрицы
			matrix[0][0] = complex<double>(1, 0);
			matrix[0][1] = complex<double>(0, 0);

			//Заполнить вторую строку матрицы
			matrix[1][0] = Z[0];
			matrix[1][1] = complex<double>(U, 0);

		}
	}
	
}


/*!	Вычислить определитель переданной матрицы */
void determinantOfMatrix(complex<double>* determinant, complex<double>(*matrix)[MAXSIZE_STR], int num, int branchCount)
{
	//Если кол-во ветвей = 3
	if (branchCount == 3)
	{
		//Вычислить миноры матрицы
		complex<double>firstMinor = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]);
		complex<double>secondMinor = matrix[1][0] * (matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1]);
		complex<double>thirdMinor = matrix[2][0] * (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]);
		//Вычислить определитель матрицы	
		determinant[num] = firstMinor - secondMinor + thirdMinor;
	}

	//Если кол-во ветвей = 2
	if (branchCount == 2)
	{
		//Вычислить определитель матрицы
		determinant[num] = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
	}
}


/*!	Функция для подсчета тока в ветви */
void currentValue(complex<double>* determinant, complex<double>* currentValue, int branchCount)
{
	int k = 1;
	//Для каждой ветви
	for (int i = 0; i < branchCount; i++)
	{
		//Поделить определитель k матрицы на определитель 0 матрицы(где k = 1, соответсвует 1 ветви, k = 2, соотв. 2 ветви и т.д)
		currentValue[i] = determinant[k] / determinant[0];
		k++;
	}
}

/*!	Записать величины полученных токов в выходной файл */
void outputCurrentValues(complex<double>* currentValue, int branchCount, string outputFile)
{
	//Открыть файл для записи
	ofstream myfile(outputFile);

	if (!myfile)  //если файл не открылся
	{
		cout << "Sorry" << endl;
	}
	else
		for (int i = 0; i < branchCount; i++)	 
		{
			myfile << currentValue[i] << endl;
		}
	//Закрыть файл для записи
	myfile.close();
}