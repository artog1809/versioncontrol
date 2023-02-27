/*!
*	 \file
*	\brief Файл содержит реализации функций перевода обратной польской записи в tex-Формулу, а также конвертация обратной пользской записи в дерево выражений
*
*	Файл содержит реализации функций перевода обратной польской записи в tex-Формулу,
*	а также конвертация обратной пользской записи в дерево выражений. Данные операции выполняются рекурсивно
*/



#include "convertTreeToTEX.h"
#include <boost/algorithm/string.hpp>



ExpressionTree* convertReversePolishEntryToTree(vector<string>& reversePolishEntryElements)
{
	int lastElementNumber = reversePolishEntryElements.size() - 1; // номер последнего элемента в обратной польской записи


	if (lastElementNumber < 0) { // элементов больше нет
		throw LACK_OF_OPERANDS_EXCEPTION; // выбросить исключение
	}

	string value = reversePolishEntryElements[lastElementNumber];  // последний элемент обратной польской запии
	reversePolishEntryElements.pop_back(); // удаляем элемент в конце вектора
	ExpressionTree* current;

	if (isNumber(value, 20) && value[0] == '-') // элемент обратной польской записи является отрицательным числом
	{
		current = new ExpressionTree("--"); // создаем новую вершину в наше дерево со значением одиночного минуса
		value.erase(0,1); // удаляем минус из значение
		reversePolishEntryElements.push_back(value); // добавляем значение в вектор элементов обратной польской записи
	}
	else {
		current = new ExpressionTree(value); // создаем новую вершину в наше дерево
	}

	if (isNumber(value)  && !isNumber(value, 20) ) { // количество значимых цифр больше 20
		throw INCORRECT_DIAPOSON_EXCEPTION; // выбросить исключение
	}


	if (current->getExpressionElementType() == OPERATOR && reversePolishEntryElements.size() >= current->getOperandsCount()) { // новая вершина является оператором и количество элементов в векторе достаточно, чтобы заполнить их операндами для данной операции

		vector<ExpressionTree*> operands(current->getOperandsCount()); // определяем количество операндов оператора
		for (int i = current->getOperandsCount() - 1; i >= 0; i--) { // для всех операндов оператора
			auto p = convertReversePolishEntryToTree(reversePolishEntryElements); // находим операнд оператора
			operands[i] = p; 
		}

		for (int i = 0; i < operands.size(); i++) { // добавляем операнд оператору
			current->addChild(operands[i]);
		}
	}
	else if (current->getExpressionElementType() == OPERATOR && reversePolishEntryElements.size() < current->getOperandsCount()) // количество операндов меньше необходимого для данного оператора
	{
		throw LACK_OF_OPERANDS_EXCEPTION; // выбросить исключение
	}
	else if (current->getExpressionElementType() == UNDEFINED) // вершина дерева является неопределенным элементом обратной польской записи
	{
		throw INCORRECT_VAL_FORMAT_EXCEPTION; // выбросить исключение
	}

	return current;
}


string convertSubFormulaToTex(ExpressionTree* current, int& curPriority)
{

	string subFormula; // считать, что подстрока не найдены

	if (current->getExpressionElementType() == OPERATOR) { // если проверяемая вершина является оператором

		curPriority = current->getOperatorPriority(); // корректный приоритет
		string operands[MAX_OPERAND_COUNT] = {}; // массив операндов
		int priority[MAX_OPERAND_COUNT] = {}; // массив  приоритета операндов

		for (int i = current->getOperandsCount() - 1; i >= 0; i--) { // для всех операндов оператора
			operands[i] = convertSubFormulaToTex(current->getChild(i), priority[i]); // находим операнд и его приоритет
		}


		int currentOperandsCount = current->getOperandsCount(); // количество операндов у оператора
		string operatorTex = current->getTexFormat(); // переводим оператор в tex-формат

		if (currentOperandsCount == 1 && current-> getValue() != "--") { // количество операндов равно  1 и оператор не является одиночным минусом
			subFormula = operatorTex + " {" + operands[0] + "}"; // перевести подстроку в tex-формат
		}

		if (currentOperandsCount == 1 && current->getValue() == "--") { // количество операндов равно  1 и оператор  является одиночным минусом
			subFormula = operatorTex + operands[0]; // перевести подстроку в tex-формат
		}

		if (currentOperandsCount == 2) // количество операндов равно 2
		{
			string value = current->getValue(); // значение вершины

			if (value == "sqrt()") { // значение является корнем

				if (operands[0] == "2") // корень второй степени
				{
					subFormula = operatorTex + " { " + operands[1] + " }"; // опустить первый операнд
				}
				else {
					subFormula = operatorTex + " [ " + operands[0] + " ]" + " { " + operands[1] + " }"; // перевести подстроку в tex-формат'

				}
			}
			else if (value == "frac()") { // значение является дробью
				subFormula = operatorTex + " { " + operands[0] + " }" + " { " + operands[1] + " }"; // перевести подстроку в tex-формат
			}
			else {

				if (curPriority < priority[0] && (operands[0][0] != '-' || value == "pow()")) operands[0] = "(" + operands[0] + ")"; // если приоритет текущей операции выше приоритета операции левого операнда и отстутсвует одинарный минус в начале операнда ИЛИ производится возведение в степень, взять левый операнд в скобки
				
				bool is_number = isNumber(operands[0]) ||  isNumber(operands[1]); // один из операндов число

				bool is_var = current->getChild(0)->getExpressionElementType() == VAR || current->getChild(1)->getExpressionElementType() == VAR; // один из операндов переменная

				bool is_greek_letter = current->getChild(0)->getExpressionElementType() == GREEKLETTER  || current->getChild(1)->getExpressionElementType() == GREEKLETTER; // один из операндов греческая буква
				

				if (value == "*"  && (is_var || is_greek_letter)  && is_number ) { // происходит умножение  числа на переменную/число

					if (isNumber(operands[1])) { // правый операнд число
						swap(operands[0], operands[1]); // поменять местами операнды
					}

					subFormula = operands[0] + operands[1]; // подстрока равна соединению левого операнда и права оператора
				}
				else if (value == "pow()") // оператор является возведением в степень
				{
					subFormula = operands[0] + " " + operatorTex + " {" + operands[1] + "}";
				}
				else 
				{
					if (curPriority < priority[1]) operands[1] = "(" + operands[1] + ")"; // если приоритет текущей операции выше приоритета операции правого операнда, взять правый операнд в скобки
					if (operands[1][0] == '-') { // второй операнд начинается с минуса, то оборачиваем выражение опернад в скобки 
						if (operands[1].find(' ') != -1) { // граница операнда найдена
							int position = operands[1].find(" "); // найти позицию разделителя
							operands[1].insert(position, ")");  // вставить скобку после операндаы
							operands[1] = "(" + operands[1]; // вставить скобку перед операндом
						}
						else // иначе
						{
							operands[1] = "(" + operands[1] + ")"; // окружить операнд скобками
						}
					}
					subFormula = operands[0] + " " + operatorTex + " " + operands[1]; // создаем подформулу
				}
			}

		}

		if (currentOperandsCount == 3) { // количество операндов равно 3
			subFormula = operatorTex + "_ {" + operands[0] + "} ^ {" + operands[1] + "}" + " {" + operands[2] + "}"; // перевести подстроку в tex-формат
		}

	}

	else if (current->getExpressionElementType() == GREEKLETTER) { // строка является греческой буквой
		subFormula = current->getTexFormat(); // перевести в TEX-формат
	}
	else
	{
		subFormula = current->getValue(); // иначе считать, что подформула равна значнию
	}

	return subFormula;
}


string convertFormulaToTex(const string& reversePolishEntry)
{
	string seps = " \t"; // разделители
	vector <string> reversePolishEntryElements; // вектор элементов обратной польской записи
	
	string workingReversePolishEntry = reversePolishEntry; // создаем копию обратной польской записи, с которой и будем работать
	

	while (seps.find(workingReversePolishEntry[workingReversePolishEntry.size() - 1]) != -1) // пока в конце строки обратной польской записи присутствует разделитель
	{
		workingReversePolishEntry.pop_back(); // удаляем разделитель
	}

	
	boost::split(reversePolishEntryElements, workingReversePolishEntry,boost::is_any_of(seps), boost ::token_compress_on); // преобразуем строку обратной  польской записи в вектор элементов обратной польской записи

	ExpressionTree* tree = NULL; // создаем вершину дерева


	if (reversePolishEntryElements.size() > 400) { // количество элементов в обратной польской записи больше допустимого
		throw VERY_LONG_STRING_EXCEPTION; // выбросить исключение
	}

	try {
		tree = convertReversePolishEntryToTree(reversePolishEntryElements); // преобразуем обратную польскую запись в дерево выражений
	}
	catch (Exception ex) {
		throw ex;
	}

	if (reversePolishEntryElements.size() != 0) { // в векторе обратной польской  польской остались непреобразованные элементы
		throw EXCESS_OF_OPERANDS_EXCEPTION; // кинуть исключение
	}


	int maxPriority = 0; // приоритет
	string texFormula = "$ " + convertSubFormulaToTex(tree, maxPriority) + " $"; // преобразуем дерево tex-формулу. Спереди и сзади дописываем знак $.

	tree->deleteTree(); // удаляем дерево

	return texFormula;
}

