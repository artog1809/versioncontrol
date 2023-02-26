/*!
*	\file 
*	\brief Файл содержит реализации конструкторов, деструктора, а также методов дерева выражений
*/


#include "ExpressionTree.h"

ExpressionTree::ExpressionTree() : expressionElementType(VAR), value("x")
{
}

ExpressionTree::~ExpressionTree()
{
}

ExpressionTree::ExpressionTree(const string value)
{
	this->value = value; // присвоить значение вершине дерева

	if (isOperator(value) != -1) { // значение является оператором
		this->expressionElementType = OPERATOR; // считать, что тип вершины является оператором
		this->texFormat = convertOperatorToTex(value); //  присвоить tex-формат оператоа
		this->operatorPriority = getPriorityOfOperator(value); // определить приоритет у оператора
		this->operandsCount = isOperator(value); // определить количество операндов у оператора
	}
	else if (isNumber(value)) // значение является числом
	{
		this->expressionElementType = NUMBER; // считать, что тип вершины является числом
	}
	else if (isGreekLetter(value)) // значение является греческой буквой
	{
		this->expressionElementType = GREEKLETTER; //считать, что тип вершины является греческой буквой
		this->texFormat = "\\" + value; // присвоить tex-формат для греческой буквы
	}
	else if (isVar(value)) { // значение является переменной
		this->expressionElementType = VAR; // считать, что тип вершины является переменной
	}
	else
	{
		this->expressionElementType = UNDEFINED; // иначе считать, что тип вершины неопределен
	}
}

string ExpressionTree::getValue()
{
	return this->value;
}

int ExpressionTree::getOperandsCount()
{
	return this->operandsCount;
}

int ExpressionTree::getOperatorPriority()
{
	return this->operatorPriority;
}

ExpressionTree* ExpressionTree::getChild(int number)
{
	if (number > MAX_OPERAND_COUNT)
		return NULL;

	return this->children[number];
}

vector <ExpressionTree*> ExpressionTree::getChildren()
{
	return children;
}



ExpressionElementType ExpressionTree::getExpressionElementType()
{
	return this->expressionElementType;
}

string ExpressionTree::getTexFormat()
{
	return this->texFormat;
}


void ExpressionTree::addChild(ExpressionTree* child)
{
	this->children.push_back(child);
}

void ExpressionTree::addChild(string value)
{

	ExpressionTree* child = new ExpressionTree(value);
	this->addChild(child);

}

void ExpressionTree::deleteTree()
{
	for (int i = 0; i < this->operandsCount; i++) { // Для всех детей вершины
		children[i]->deleteTree(); // удалить ребенка вершины
	}

	delete this;
}
