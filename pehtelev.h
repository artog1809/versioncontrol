/*!
*	\file
*	\brief Файл содержит перечисление типов элементов обратной польской записи, а также класс дерева выражений
* 
*	Данный файл содержит класс дерева выражений с объявлеными методами, конструкторами, а также перечисление
*	типов элементов обратной польской записи
*/

#pragma once
#include "ConvertToTEX.h"
#include <iostream>
using namespace std;

//! Типы элементов обратной польской записи
enum ExpressionElementType {
	NUMBER, ///< Число
	VAR, ///< Переменная
	OPERATOR, ///< Оператор
	GREEKLETTER, ///< Греческая буква
	UNDEFINED ///< Неопределенный элемент обратной польской записи
};


/*!	\brief  Дерево выражений
*	
*	Класс дерева выражений. Класс содержит значение, tex-формат значения, количество операндов и приоритет оператора.
*/
class ExpressionTree
{
private:

	//! Тип элемета обратной польской записи
	ExpressionElementType expressionElementType;
	
	//! Значение дерева выражений
	string value;

	//! Значение в tex-формате
	string texFormat;

	//! Количество операндов
	int operandsCount = 0;

	//! Приоритет оператора
	int operatorPriority;

	//!	Вектор указателей на  детей вершины
	vector <ExpressionTree*> children;
public:

	/*!
	*	\brief Конструктор по умолчанию
	*/
	ExpressionTree();

	/*!	\brief Конструктор, который принимает значение
	*	\param[in] value - значение вершины
	*/
	ExpressionTree(const string value);

	/*!	\brief Деструктор по умолчанию
	*/
	~ExpressionTree();

	/*! \brief Возвращает значение вершины
	*	\return - значение вершины
	*/
	string getValue();

	/*!	\brief Возвращает количество операндов
	*	\return - количество операндов
	*/
	int getOperandsCount();

	/*!	\brief Возвращает приоритет оператора
	*	\return - приоритет оператора
	*/
	int getOperatorPriority();

	/*!	\brief Возвращает указатель на ребенка
	*	\param[in] number - номер ребенка
	*	\return - указатель на ребенка
	*/
	ExpressionTree* getChild(int number);

	/*!	\brief Возвращает вектор указателей на детей
	*	\return - вектор указателей на детей
	*/
	vector<ExpressionTree*> getChildren();
	
	/*!	\brief Возвращает тип элемента обратной польской записи
	*	\return - тип элемента обратной польской записи
	*/
	ExpressionElementType getExpressionElementType();

	/*!	\brief Возвращает tex-формат значения вершины
	*	\return - tex-формат значения вершины
	*/
	string getTexFormat();

	/*!	\brief Добавляет ребенка вершины
	*	\param[in] value - значение ребенка вершнины
	*/
	void addChild(const string value);

	/*!	\brief Добавляет ребенка вершины
	*	\param[in] child - указатель на вершину-ребенка  
	*/
	void addChild(ExpressionTree* child);

	/*!	\brief Удаление вершины и всех его детей
	*/
	void deleteTree();
};

