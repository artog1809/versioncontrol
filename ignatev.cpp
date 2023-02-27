#include "lab4_Oganesyan.h"
#include <iostream>
using namespace std;

int main()
{
    int vector[MAX_VECTOR_LENGTH];
    int vectorLength = 0;
    scanf_s("%d", &vectorLength);
    if (vectorLength > MAX_VECTOR_LENGTH)
    {
        printf("Ошибка ввода");
    }
    else
    {
        for (int i = 0; i < vectorLength; i++)
        {
            scanf_s("%d", &vector[i]);
        }
    }
    
    sortVectorAscending(vector, vectorLength);

     
    for (int i = 0; i < vectorLength; i++)
    {
        printf("%d ", vector[i]); 
    }
}

//Упорядочивание вектора по возрастанию его элементов
 void sortVectorAscending(int vector[MAX_VECTOR_LENGTH], int length)
{
    int tmp;
    //Если в векторе два элемента
    if (length == 2)
    {
        //Поменять их местами, если они упорядочены
        if (vector[0] > vector[1])
        {
            tmp = vector[0];
            vector[0] = vector[1];
            vector[1] = tmp;
        }
    }
    else if (length > 2)
    {
        //Упорядочить первую половину вектора
        sortVectorAscending(vector, length / 2);
        //Упорядочить вторую половину вектора
        sortVectorAscending(vector + (length / 2), (length + 1) / 2);
        //Выполнить слияние половинок вектора, создав новый вектор
        int newVector[MAX_VECTOR_LENGTH] = {};
        int newLength = mergeSortedVectors(vector, length / 2, vector + length / 2, (length + 1) / 2, newVector);
        //Скопировать содержимое нового вектора в исходный
        for (int i = 0; i < newLength; i++)
        {
            vector[i] = newVector[i];
        }
    }
    

}

//Соединение упорядоченных векторов
int mergeSortedVectors(int vectorA[], int lengthA, int vectorB[], int lengthB, int mergedVector[])
{
    //Длина объединенного вектора
    int lengthMergedVector = lengthA + lengthB;

    for (int i = 0, j = 0; i < lengthA || j < lengthB; )
    {
        //Если первый вектор не завершился, а второй завершился или оба вектора не завершились
        //и текущий элемент из первого вектора меньше текущего элемента из второго
        
        if ((i < lengthA && j >= lengthB) || (vectorA[i] < vectorB[j] && j < lengthB && i < lengthA))
        {
            //Записать текущий элемент первого вектора в конец объединенного вектора 
            mergedVector[i + j] = vectorA[i];
            //Перейти к следующему элементу первого вектора
            i++;
        }
        else
        {
            //Записать текущий элемент второго вектора в конец объединенного вектора 
            mergedVector[i + j] = vectorB[j];
            //Перейти к следующему элементу второго вектора
            j++;
        }
    }
    
    return lengthMergedVector;
}

