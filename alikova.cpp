#include <iostream>
#include <conio.h>
#include "08_Oganesyan.h"
#include <string.h>

int main()
{		
	//Считываем название искомой переменной
	char variableName[MAXSIZE_STR]="";
	printf("Input variable name:\n");
	scanf("%s", &variableName);
	
	//Считываем количество строк программного кода
	int stringCount;
	printf("Input strings count:\n");
	scanf("%d", &stringCount);

	//Считываем программный код
	if (stringCount > 0 && stringCount < 21)
	{
		char programCode[MAXNUM_STR][MAXSIZE_STR];
		printf("Write your code:\n");
		for (int index = 0; index < stringCount; index++)
		{
			gets_s(programCode[index]);
		}

		//Выводим наличие ошибки
		int error = findUsingNoInitVar(programCode, stringCount, variableName);
		if (error == 0) 
		{
			printf("\nno error");
		}
		else if (error == 1)
		{
			printf("\nerror");
		}
		else if (error == 2)
		{
			printf("\nno variable");
		}
	}
	else
	{
		printf("invalid input data");
	}

}


/*!	Поиск обращения к неициализированной переменной
	\param[in] programCode - код программы
	\param[in] stringCount - количество строк программы
	\param[in] variableName - имя переменной	
	\return 0|1|2 - no error|error|no variable
*/
int findUsingNoInitVar( char programCode[MAXNUM_STR][MAXSIZE_STR], int stringCount, const char* variableName)
{
	int isVariableFound = 0;//Флаг - имеется одно обращение к искомой переменной
	int isVariableFoundSecond = 0;//Флаг - имеется два обращения к искомой переменнойы
	int isVariableInitialized = 0;//Флаг - переменная инициализирована
	int currentString = 0;//Индекс текущей строки
	int currentSymbol = 0;//Индекс текущего символа
	//Поиск объявления искомой переменной
	currentString = findDeclarationSearchVariable(programCode, variableName, stringCount, currentString, &isVariableFound);

	//Если объявление искомой переменной найдено
	if (isVariableFound == 1)
	{
		//Проверка строки с объявлением переменной на наличие инициализации
		currentString = checkStringWithDeclarationForInit(programCode, variableName, stringCount, currentString, &isVariableInitialized);

		//Если переменная объявлена без инициализации
		if (isVariableInitialized == 0)
		{
			//Проверка всех строк, после строки объявления на наличие обращения к искомой переменной
			currentString = findDeclarationSearchVariable(programCode, variableName, stringCount, currentString, &isVariableFoundSecond);

			//Если имеется обращение к переменной
			if (isVariableFoundSecond == 1)
			{
				
				//Проверка строки с обращением к переменной на инициализацию
				return checkStringWithUsingForInit(programCode, variableName, stringCount, currentString);

			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 2;
	}
	return 1;
}


//Поиск объявления искомой переменной
int findDeclarationSearchVariable(char programCode[MAXNUM_STR][MAXSIZE_STR], const char* variableName, int stringCount, int currentString, int *isVariableFound)
{
	//Для каждой строки программы и пока обращение к переменной не найдено
	for (currentString; currentString < stringCount && *isVariableFound == 0; currentString++)
	{

		//Если строка не закомментирована
		if (programCode[currentString][0] != '/')
		{
			//Разделить строку на лексемы
			char* res;
			char programCpy[MAXSIZE_STR] = "";
			strcpy(programCpy, programCode[currentString]);
			res = strtok(programCpy, " =;,+-*/<>()");

			//Проверить, есть ли в строке к обращение к переменной
			while (res != NULL) {
				//Если в строке есть обращение к переменной
				if (!strcmp(res, variableName)) {
					*isVariableFound = 1;
				}

				res = strtok(NULL, " =;,+-*/<>()");
			}
		}

	}
	return currentString;
}
	
//Проверка строки с объявлением переменной на наличие инициализации
int checkStringWithDeclarationForInit(char programCode[MAXNUM_STR][MAXSIZE_STR], const char* variableName, int stringCount, int currentString, int* isVariableInitialized)
{
	//Для каждого символа строки
	for (int currentSymbol = 0; currentSymbol < strlen(programCode[currentString - 1]); currentSymbol++)
	{
		//Если в строке есть знак "равно"
		if (programCode[currentString - 1][currentSymbol] == '=')
		{
			//Разделить текущую строку на лексемы
			char* stringCpySecond;
			char stringCpy[MAXSIZE_STR] = "";
			strcpy(stringCpy, programCode[currentString - 1]);
			stringCpySecond = strtok(stringCpy, " ");

			int countLexems = 0;//Количество лексем в строке
			while (stringCpySecond != NULL) {
				countLexems++;
				//Если в текущей строке есть инициализация искомой переменной
				if (!strcmp(stringCpySecond, variableName) && countLexems == 2) {
					*isVariableInitialized = 1;
				}
				else if (!strcmp(stringCpySecond, variableName) && countLexems > 2) {
					*isVariableInitialized = 0;
				}

				stringCpySecond = strtok(NULL, " =;,+-*/<>()");

			}
			//Если инициализирована другая переменная, проверить строку на наличие инициализации искомой переменной
			if (*isVariableInitialized == 0)
			{
				for (currentSymbol + 1; currentSymbol < strlen(programCode[currentString - 1]); currentSymbol++)
				{
					if (programCode[currentString - 1][currentSymbol+1] == '=')
					{
						*isVariableInitialized = 1;
					}
				}
			}

		}
	}
	return currentString;
}

//Проверка строки с обращением к переменной на инициализацию
 int checkStringWithUsingForInit(char programCode[MAXNUM_STR][MAXSIZE_STR], const char* variableName, int stringCount, int currentString)
 {
	 int countOfValues = 0;//Счетчик количества искомых переменных в строке
	 //Для каждого символа строки
	 for (int currentSymbol = 0; currentSymbol < strlen(programCode[currentString - 1]); currentSymbol++)
	 {
		 //Если строка не закомментирована
		 if (programCode[currentString - 1][0] != '/')
		 {
			 //Если в строке есть инициализация
			 if (programCode[currentString - 1][currentSymbol] == '=')
			 {
				int secondInit = currentSymbol+1;//Индекс элемента, следующим за "первым" знака равенства
				 
				//Если в искомая переменная встречается в строке 2 раза
				if (findCountOfUsedVariableInString(programCode, currentString, variableName) == 2)
					return 1;
				 
				//Для каждого символа, после "первой" инициализации в строке
				for (secondInit; secondInit < strlen(programCode[currentString - 1]); secondInit++)
				{
					countOfValues = 0;
					if (programCode[currentString - 1][secondInit] == '=')
					{
						if (findCountOfUsedVariableInString(programCode, currentString, variableName) == 2)
							return 1;
						else 
							return 0;
					}
					 
				}

				 //Разделить текущую строку на лексемы
				 char stringCpy[MAXSIZE_STR] = "";
				 strncpy_s(stringCpy, programCode[currentString - 1], currentSymbol);
				 char* stringCpySecond;
				 stringCpySecond = strtok(stringCpy, " ");
				 //Если в текущей строке есть инициализация искомой переменной
				 if (!strcmp(stringCpySecond, variableName))
				 {
					 return 0;
				 }
				 else
				 {
					 return 1;
				 }
			 }
			 
		 }
	 }
	 return 1;
 } 

 //Поиск вхождений искомой переменной в строку
 int findCountOfUsedVariableInString(char programCode[MAXNUM_STR][MAXSIZE_STR], int currentString, const char* variableName)
 {
	 int countOfValues = 0;//Счетчик количества вхождений искомой переменной в строке

	 //Разделить текущую строку на лексемы
	 char strCpy[MAXSIZE_STR] = "";
	 strcpy(strCpy, programCode[currentString - 1]);
	 char* stringCpyThird;
	 stringCpyThird = strtok(strCpy, " =;,+-*/<>()");
	 while (stringCpyThird != NULL)
	 {
		 //Если лексема совпадает с названием искомой переменной
		 if (!strcmp(stringCpyThird, variableName))
			 //Увеличить счетчик количества вхождений искомых переменных
			 countOfValues++;
		 stringCpyThird = strtok(NULL, " =;,+-*/<>()");
	 }
	 return countOfValues;
 }

