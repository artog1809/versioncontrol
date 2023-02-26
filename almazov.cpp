/*!
*   \file
*   \brief Файл содержит реализацию функций вспомогательных функций
*   
* Данный файл содержит реализации функций, необходимых для идентификации элемента обратной польской записи,
* чтение и записи в файл,  функцию обработки ошибок, определение параметров оператора в обратной польской записи, 
*/



#include "ConvertToTEX.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include "boost/lexical_cast.hpp"




bool isNumber(const string& str, int nSignificantDigits)
{
    int minus = 0; // минус не найден
    int point = 0; // точка не найдена
    bool result = 1; // считать, что строка является номером
    int numberSize = str.size(); // количество значащих цифр

    try {
        double num = boost::lexical_cast<double> (str); // перевод из строки в число
    }
    catch (boost::bad_lexical_cast&) { // перевод не был осуществлен
        result = 0; // считать, что строка не является числом
    }


    minus = count(str.begin(), str.end(), '-'); // найти в строке минус

    if (minus != -1) { // минус найден
        numberSize -= minus; // уменьшить количество значащих цифр на единицу
    }

    point = str.find("."); // найти точку в строке

    if (point != -1) { // точка найдена
        numberSize--; // уменьшить количество значащих цифр на единицу
    }

    if (str.find("e") != -1 || str.find("E") != -1) // в строке присутствует знак экспоненты
    {
        result = 0; // считать, что строка не является числом
    }

    if (numberSize > nSignificantDigits) // количество значащих цифр в строке больше 20
        result = 0; // считать, что строка не является числом

    return result;

}


int isOperator(const string& str)
{
    int numberOfOperands = -1; // Считать, что строка не является оператором

    const vector<string> oneOperand = { "!", "all()", "exist()", "--"}; // массив операторов с одним операндом

    const vector<string> twoOperands = { "+", "-", "/", "*", "+-", "-+", "%", "=", "!=", ">=", "<=", ">", "<",
         "&&", "||", ",", "..", "&", "|", "pow()", "sqrt()", "in()", "noin()", "_", "frac()"}; // массив операторов с двумя операндами

    const vector <string> threeOperands = { "summator()" }; // массив операторов с тремя операндами

    for (int i = 0; i < oneOperand.size(); i++) // для всех операторов с одним операндом
        if (str == oneOperand[i]) // оператор равен строке
            numberOfOperands = 1; // считать, что количество операндов у оператора равна одному

    for (int i = 0; i < twoOperands.size(); i++) // для всех операторов с двумя операндами
        if (str == twoOperands[i]) //  оператор в строке
            numberOfOperands = 2; // считать, что количество операндов у оператора равна двум

    for (int i = 0; i < threeOperands.size(); i++) // для всех операторов с тремя операндами
        if (str == threeOperands[i]) // оператор равен строке
            numberOfOperands = 3; // считать, что количество операндов у оператора равна трем

    return numberOfOperands; // вернуть количество операндов у оператора, если строка является оператором, иначе вернуть -1
}

bool isGreekLetter(const string& str)
{
    string workingStr = "\\" + str; // добавить \ перед началом строки, для того, чтобы получить формат записи для TEX

    vector <string> upperGreekLetter = { "\\Alpha", "\\Beta", "\\Gamma", "\\Epsilon", "\\Zeta", "\\Eta", "\\Theta",
    "\\Iota", "\\Kappa", "\\Lambda", "\\Mu", "\\Nu", "\\Xi", "\\Omicron", "\\Pi", "\\Rho", "\\Sigma", "\\Tau", "\\Upsilon",
    "\\Phi", "\\Chi", "\\Psi", "\\Omega" }; // греческие заглавные буквы

    vector <string> lowerGreekLetter = { "\\alpha", "\\beta", "\\gamma", "\\epsilon", "\\zeta", "\\eta", "\\theta",
    "\\iota", "\\kappa", "\\lambda", "\\mu", "\\nu", "\\xi", "\\omicron", "\\pi", "\\rho", "\\sigma", "\\tau", "\\upsilon",
    "\\phi", "\\chi", "\\psi", "\\omega" }; // греческие маленькие буквы

    bool result = 0; // считать, что строка не является греческой буквой

    for (int i = 0; i < upperGreekLetter.size(); i++) {  // для всех заглавных греческих букв
        if (workingStr == upperGreekLetter[i]) //  строка является заглавной греческой буквой
            result = 1; // считать, что строка  является греческой буквой
    }

    for (int i = 0; i < lowerGreekLetter.size(); i++) { // для всех маленьких букв
        if (workingStr == lowerGreekLetter[i]) // строка является заглавной греческой буквой
            result = 1; // считать, что строка не является греческой буквой
    }

    return result;
}

bool isVar(const string& str) {

    bool result = 1; // Считать, что строка является переменной

    for (int i = 0; i < str.size(); i++) { // для всех элементов строки

        if (str[i] != '_' && !isalnum(str[i]))  // элемент строки не является буквой, числом или нижнем подчеркиванием
            result = 0; // Cчитать, что строка не является переменной
    }

    int varSize = str.size(); // длина строки
    if (isdigit(str[0])) result = 0; //если первый элемент строки является числом, считать, что строка не является переменной 
    if (varSize > 10) result = 0; //если длина строки больше 10 , считать, что строка не является переменной 


    return result;
}

int getPriorityOfOperator(const string& str) {

    // вектор векторов нулевой вектор - это вектор элементов с приоритетом 0, первый вектор - вектор элементов с приоритетом - 1 и т.д.
    vector <vector<string>> priorityOperators{
     { "all()", "exist()", "_", "!", "summator()"},
     { "in()", "noin()"},
     {"pow()", "sqrt()"},
     { "*", "/", "%", "frac()", "&"},
     { "+", "-", "+-", "-+", "|", "--"},
     { ">=", "<=", ">", "<" },
     { "=", "!=" },
     { "&&" },
     { "||" },
     { ",", ".." }
    }; 

    int priority = -1; // считать, что строка не является оператором

    for (int i = 0; i < priorityOperators.size(); i++) { // для всех приоритетов
        for (int j = 0; j < priorityOperators[i].size(); j++) { // для всех операторов с приоритетом i
            if (priorityOperators[i][j] == str) // строка равна оператору
                priority = i; // приоритет строки равен приоритету оператора
        }
    }

    return priority;
}

void handleExceptions(Exception exception)
{   
    setlocale(LC_ALL, "Russian");
    switch (exception)
    {
        case EMPTY_STRING_EXCEPTION:
            cerr << "Вводится пустая строка" << endl;
            break;
    
        case FILE_IN_NOT_FOUND_EXCEPTION:
            cerr << "Неверно указан файл с входными данными. Возможно, файл не существует" << endl;
            break;

        case INCORRECT_EXTENSION_EXCEPTION:
            cerr << "Неверно указано расширение файла. Файл должен иметь расширение .txt " << endl;
            break;

        case INCORRECT_VAL_FORMAT_EXCEPTION:
            cerr << "Неопознанный элемент обратной польской записи " << endl;
            break;

        case INCORRECT_DIAPOSON_EXCEPTION:
            cerr << "Количество значимых цифр в числе больше максимально возможного " << endl;
            break;

        case EXCESS_OF_OPERANDS_EXCEPTION:
            cerr <<"Количество операций  недостаточно для данного количества операндов "<< endl;
            break;

        case LACK_OF_OPERANDS_EXCEPTION:
            cerr << "Количество операций больше необходимого количества " << endl;
            break;
        
        case LACK_OF_CONSOLE_ARGUMENT:
            cerr << "Недостаточное количество аргументов" << endl;
            break;

        case INCORRECT_EXTENSION_OUTPUT_EXCEPTION:
            cerr << "Неправильное расширение файла. Файл должен иметь расширение .tex" << endl;
            break;

        case VERY_LONG_STRING_EXCEPTION:
            cerr << "Количество элементов в обратной польской записи больше 400" << endl;
            break;

        case UNKNOWN_EXCEPTION:
            cerr << "НЕИЗВЕСТНАЯ ОШИБКА" << endl;
            break;

    }
    

}



string convertOperatorToTex(const string& str)
{
    // словарь оператор. Ключ - оператор в Си формате, значение - оператор в TEX-формате
    map <string, string> operatorTex = { {"+", "+"}, {"-", "-"}, {"/", "\\div"}, {"*", "\\cdot"},
        {"+-", "\\pm"}, {"-+","\\mp"}, {"%", "\\mod"}, {"=", "="}, {">=", "\\geq"}, {"<=", "\\leq"},
        {"!=", "\\neq"}, {">",">"}, {"<","<"}, {"&&", "\\wedge"},{"||","\\vee"}, {"&","\\cap"}, {"|","\\cup"}, {",", ","},
        {"..", ".."}, {"frac()", "\\frac"}, {"pow()","^"}, {"_","_"}, {"sqrt()","\\sqrt"}, {"in()", "\\in"}, {"noin()", "\\notin"},
        {"!","\\overline"}, {"all()", "\\forall"}, {"exist()", "\\exists"}, {"summator()","\\sum"}, {"--","-"} };


    return operatorTex[str]; 
}


void readFile(string& fileName, string& reversePolishEntry) {

    string extension = ".txt"; // допустимое расширение файла

    bool isEndWith = boost::algorithm::ends_with(fileName, extension); 

    if (isEndWith) { // файл имеет необходимое расширение
        ifstream fin(fileName); // открыть файл

        if (fin.fail()) { // файла не существует
            throw FILE_IN_NOT_FOUND_EXCEPTION; // выбросить исключение
        }

        getline(fin, reversePolishEntry); // считать содержимое файла

        if (reversePolishEntry.empty()) { // считанная строка пустая 
            throw EMPTY_STRING_EXCEPTION; // выбросить исключение
        }
    }
    else
    {
        throw INCORRECT_EXTENSION_EXCEPTION; // выбросить исключение
    }

}

void writeToFile(string& fileName, string& texFormula) {

    ofstream fout; // поток
    fout.open(fileName); // открыть/создать файл
    fout << texFormula; // записать данные в файл
}