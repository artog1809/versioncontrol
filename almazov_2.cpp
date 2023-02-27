#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "Header.h"

using namespace std;

int main(const int argc, char** argv)
{
    setlocale(LC_ALL, "Russian");
    ifstream fin;
    ofstream fout;
    try
    {
        fin.open(argv[1]);
        //Выдать ошибку, если входной файл не указан в аргументах командной строки
        //Выдать ошибку, если входной файл невозможно открыть
        if (fin.is_open() == 0)
            throw ("Неверно указан файл с входными данными. Возможно, файл не существует");
        int i = 0;
        int amountOfNumbers = 0;
        int numberOfFixedPoints = 0;
        string data;
        //Считать числа из файла...
        while (!fin.eof())
        {
            fin >> data;
            //Выдать ошибку, если входной файл имеет некорректные символы
            for (int i = 0;i < data.length();i++)
            {
                if (!isdigit(data[i]))
                    throw "Недопустимые входные данные: ожидались натуральные числа, а введены символы";
            }
            if (fin.eof() && data.empty())
                throw "Входной файл является пустым.";

            if (i == 0)
                amountOfNumbers = std::stoi(data);
            if (i == 1)
                numberOfFixedPoints = std::stoi(data);
            i++;
        }
        fin.close();

        fout.open(argv[2]);
        if (fout.is_open() == 0)
            throw "Неверно указан файл для выходных данных. Возможно, указанного расположения не существует";
        try
        {
            //Вычислить количество перестановок с неподвижными точками
            //Записать результат вычислений в выходной файл
            fout << numberOfTranspositionWithFixedPoints(amountOfNumbers, numberOfFixedPoints);
        }
        catch (const int value)
        {
            if (value == 1)
                cout << "Один из операндов не является натуральным числом" << endl;
            if (value == 2)
                cout << "Неверные входные данные: один из операндов не принадлежит диапазону, указанному в требованиях" << endl;
        }
        fout.close();
    }
    catch (const char* e)
    {
        cout << e << endl;
    }

    return 0;
}

int numberOfTranspositionWithoutFixedPoints(int amountOfNumbersWithoutFixedPoints)
{
    //Считать, что нет перестановок одного числа без неподвижных точек
    int transpositionOneNumber = 0;
    //Считать, что есть единственная перестановка двух чисел без неподвижных точек
    int tranpositionTwoNumbers = 1;
    //Для количества чисел, начиная от двух, без неподвижных точек…
    for (int i = 2; i < amountOfNumbersWithoutFixedPoints; ++i)
    {

        //Если количество чисел нечетно, Изменить значение числа перестановок двух чисел без неподвижных точек на количество чисел,
        //умноженной на сумму перестановок одного числа и двух чисел без неподвижных точек
        if (i % 2) {
            tranpositionTwoNumbers = i * (transpositionOneNumber + tranpositionTwoNumbers);
        }
        //Иначе, изменить значение числа перестановок одного числа без неподвижных точек на количество чисел,
        //умноженной на сумму перестановок одного числа и двух чисел без неподвижных точек
        else {
            transpositionOneNumber = i * (transpositionOneNumber + tranpositionTwoNumbers);
        }
    }
    //Если количество чисел нечетное, то вернуть количество перестановок с одним числом без неподвижных точек
    //Иначе вернуть количество перестановок с двумя числами без неподвижных точек
    return amountOfNumbersWithoutFixedPoints % 2 ? transpositionOneNumber : tranpositionTwoNumbers;
}

int choiceOfFixedPoints(int totalAmountOfNumbers, int numberOfChoicesNumbers)
{
    int argumentsDifference = totalAmountOfNumbers - numberOfChoicesNumbers;
    //Если число неподвижных точек больше разности числа неподвижных точек и количества чисел...
    if (numberOfChoicesNumbers > argumentsDifference)
        //Считать, что число неподвижных точек есть разность числа неподвижных точек и количества чисел
        numberOfChoicesNumbers = argumentsDifference;
    //Если числа неподвижных точек нет, вернуть значение 1
    if (!numberOfChoicesNumbers)
        return 1;
    int multiplicationResult = argumentsDifference = totalAmountOfNumbers - numberOfChoicesNumbers + 1;
    argumentsDifference++;
    //Для всех чисел, начиная от двух, до числа неподвижных точек...
    for (int i = 2; i <= numberOfChoicesNumbers; i++, argumentsDifference++)
        //Вычислить число сочетаний как произведение чисел, начиная от инкрементной разности количества всех чисел и числа неподвижных точек
        multiplicationResult = multiplicationResult / i * argumentsDifference + multiplicationResult % i * argumentsDifference / i;
    //Вернуть число сочетаний с неподвижными точками
    return multiplicationResult;
}

int numberOfTranspositionWithFixedPoints(int amountNumbers, int amountFixedPoints)
{
    //Выдать ошибку, если во входных параметрах один из операндов не является натуральным числом
    if (amountNumbers < 0 || amountFixedPoints < 0)
        throw 1;
    //Выдать ошибку, если входные параметры выходят за пределы разрешенного диапазона
    if (amountNumbers > 9 || amountFixedPoints > amountNumbers)
        throw 2;
    //Найти количество выборов неподвижных точек и число перестановок, не содержащих неподвижных точек...
    //Вернуть количество перестановок с неподвижными числами как произведение количества выборов неподвижных точек
    //на число перестановок, не содержащих неподвижные точки
    return choiceOfFixedPoints(amountNumbers, amountFixedPoints) * numberOfTranspositionWithoutFixedPoints(amountNumbers - amountFixedPoints);
}