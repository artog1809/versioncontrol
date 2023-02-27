#include "constantInFunction.h"

constantInFunction::constantInFunction()
{
	this->name = "defaultConstantName"; //Задать имя функции
	this->countInFunction = 0; //Установить счётчик встреч константы за функцию равным 1
}


constantInFunction::constantInFunction(std::string _name, int _stringNum)
{
	this->name = _name; //Задать имя функции
	this->appearingStrings[0] = _stringNum; //Добавить первую встречу константы в заданной строке
	this->countInFunction = 1; //Установить счётчик встреч константы за функцию равным 1
}


constantInFunction::~constantInFunction()
{
}

//---------- ВЗАИМОДЕЙСТВИЕ С КОНСТАНТОЙ ----------


std::string constantInFunction::getName() //Получить имя константы
{
	return this->name; //Вернуть имя константы
}


/*
Получить число встреч данной константы внутри своей функции
*/
short constantInFunction::getAppearanceCount() 
{
	return this->countInFunction;
}

/*
* Получить номер строки,в которой константа встречается,из массива строк по индексу
\param[in] index - Индекс строки,где встречена константа, в массиве
*/
short constantInFunction::getAppearingString(int index) 
{
	if (index >= 0 && index <= countInFunction)
		return this->appearingStrings[index];
	else
		return -1;
}


/*
Добавить встречу константы на определённой строке
\param[in] string - номер строки,на которой встречена константа
*/
void constantInFunction::addNewAppearance(int string)	
{
	if (string > 0 && string<MAX_STRING_NUMBER)
		this->appearingStrings[countInFunction] = string;	//Добавить встречу константы на заданной строке,если она не ошибочна
	countInFunction += 1;	//Увеличить число встреч этой константы в функции на 1
}

/*
Задать имя константы
\param[in] name - имя,задаваемое константе
*/
void constantInFunction::setName(std::string _name)
{
	this->name = _name;
}
