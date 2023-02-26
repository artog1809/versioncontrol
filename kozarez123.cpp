
#include "determiningFunctionCreator.h"
#include <iostream>

int main()
{
    setlocale(LC_ALL, "Russian");
    int stringCount;
    char functionName[81];
    char programCode[MAX_STR_COUNT][MAX_STR_LEN];
    
    
    int inputError = inputProgramCode(&stringCount,functionName, programCode); //Вводим количество строк и програмный код 

    if (inputError) // присутсвуют ошибки ввода
    {
        puts("input Error");    //сообщаем пользователю об ошибке 
    }
    else
    {
        //Определяем является, ли заданная функция библиотечной
        FunctionCreator functionCreator;
        int isFound = determineFunctionCreator(programCode, stringCount, functionName, &functionCreator);
        outputFunctionCreator(isFound, functionCreator);

    }

}
123
void outputFunctionCreator(int isFound, FunctionCreator functinCreator)
{   
    char answerString[81];
    if (isFound)
    {
        if (functinCreator == LIBRARY)
        {
            strcpy_s(answerString, "library function");
        }
        else
        {
            strcpy_s(answerString, "user function");
        }
    }
    else
    {
        strcpy_s(answerString, "no function");
    }

    puts(answerString);
}

int inputProgramCode(int *stringCount, char functionName[], char programCode[MAX_STR_COUNT][MAX_STR_LEN])
{   
    int inputError = 0;
    gets_s((functionName), sizeof(char)*81);

    scanf_s("%d", stringCount);

    gets_s(programCode[0]);
    if (*stringCount < 1 || *stringCount > 20)
    {
        inputError = 1;
    }

    for (int i = 0; i < *stringCount && !inputError; i++)
    {
        gets_s(programCode[i]);
    }
    
    return inputError;
}

int determineFunctionCreator(const char programCode[MAX_STR_COUNT][MAX_STR_LEN], int stringCount, const char functionName[], FunctionCreator *functionCreator)
{

    int functionIsFound = 0;
    TypeOfEntryFunction typeOfEntryFunction;
    PositionInCode positionInCode;

    //Найти первое вхождение функции в коде и его вид вхождения
    positionInCode = findFunctionAndItTypeOfEntry(programCode, stringCount, functionName, &typeOfEntryFunction);

    if (positionInCode.positionInRow != -1) // функция найдена
    {
        functionIsFound = 1;

        if (typeOfEntryFunction == PROTOTYPE || typeOfEntryFunction == DECLARATION) // прототип или объявление функции найдены
        {
            *functionCreator = USER; //Считать, что функция является пользовательской
        }
        else 
        {
            *functionCreator = LIBRARY; //Считать, что функции является библиотечной
        }

    }

    return functionIsFound;
}
                                                                                                                                                                                

PositionInCode findFunctionAndItTypeOfEntry(const char programCode[MAX_STR_COUNT][MAX_STR_LEN], int stringCount, const char functionName[], TypeOfEntryFunction* typeOfEntryFunction) {
    
    //Найти первое вхождение функции в коде
    PositionInCode positionInCode = findFirstOccurrenceOfFunction(programCode, stringCount, functionName);
    
    if (positionInCode.row != -1)
    {   
        //Определить лексемы, что находятся слева и справа от функции
        int DataTypeFound = isDataType(programCode[positionInCode.row], positionInCode); 
        int BracesFound = isBracesFound(programCode, stringCount, positionInCode);

        if (DataTypeFound && BracesFound) //левая лексема является типом данных и правая лексема является фигурными скобками
        {
            *typeOfEntryFunction = DECLARATION; //Считать, что вид вхождения функции является объявлением
        }
        else if (DataTypeFound) //левая лексема является типом данных
        {
            *typeOfEntryFunction = PROTOTYPE; //Считать, что вид вхождения функции является прототипом
        }
        else
        {
            *typeOfEntryFunction = CALL; //Считать, что вид вхождения функции является вызовом
        }
    }
    
    return positionInCode;
}

PositionInCode findFirstOccurrenceOfFunction(const char programCode[MAX_STR_COUNT][MAX_STR_LEN], int stringCount, const char functionName[])
{   
    PositionInCode positionInCode = { -1,  -1 }; //Считать, что функция не найдена
    
    //Для все строк, пока не найдена функция в коде программе
    for (int i = 0; i < stringCount && positionInCode.row == -1; i++)
    {   
        int positionInString = findFunctionInString(programCode, stringCount, i, functionName);

        if (positionInString != -1)
        {
            positionInCode = { i, positionInString };
        }
    }

    return positionInCode;
}

int findFunctionInString(const char programCode[MAX_STR_COUNT][MAX_STR_LEN],int stringCount, int stringNumber, const char functionName[])
{
    int functionNamePositionInString = -1;

    const char* programCodeString = programCode[stringNumber];
    int commentPositionInString = strlen(programCodeString); // Считать, что комментарии находятся за последним элементом строки
    int commentsPositionGreaterFunctionPosition = 1;

    if (strstr(programCodeString, "//") != NULL) 
    {
        commentPositionInString = strstr(programCodeString, "//") - programCode[stringNumber]; // Найти комментрарии в строке
    }

    int wordIsKeyword = isKeyword(functionName);

    if (!wordIsKeyword)
    {
        while (strstr(programCodeString, functionName) != NULL && commentsPositionGreaterFunctionPosition) //Пока в строке есть подстроки равные, равные имени функции  и позиция подстроки имени функции меньше позиции комментрариев
        {

            int subStringPositionInString = strstr(programCodeString, functionName) - programCode[stringNumber]; // Найти первую подстроку, равную имени 
            commentsPositionGreaterFunctionPosition = subStringPositionInString < commentPositionInString;

            if (commentsPositionGreaterFunctionPosition) {
                PositionInCode positionFunctionNameInCode = { stringNumber, subStringPositionInString };
                int isValidTokenForIdentifier = isFirstCharBeforeFunctionNameValidTokenForIdentifier(programCode, stringCount, positionFunctionNameInCode); // Проверить, является ли первый символ перед именем функцией недопустимой лексемой
                int isParenthesis = isFirstCharAfterFunctionNameParenthesis(programCode, stringCount, functionName, positionFunctionNameInCode); // Проверить, является ли первый символ после имени функцией круглой скобкой

                if (!isValidTokenForIdentifier && isParenthesis) //символ перед строкой не является недопустимой лексемой и первый символ после имени функцией - круглая скобка
                {
                    functionNamePositionInString = subStringPositionInString; //Считать, что функция найдена
                }
            }

            programCodeString = programCode[stringNumber] + subStringPositionInString + 1;
        }
    }

    return functionNamePositionInString;
}


int isDataType(const char programCodeString[MAX_STR_LEN], PositionInCode positionFunctionInCode)
{
    int  positionOfLeftToken = -1;
    char leftToken = ' ';

    //Найти левую лексему в коде
    for (int i = positionFunctionInCode.positionInRow - 1; i >= 0 && positionOfLeftToken == -1; i--) // Пока  левая лексема не найдена, для всех позиций, меньших позиции начала имени функции
    {
        if (programCodeString[i] != ' ' &&  programCodeString[i] != '\t') // символ на позиции не является разделителем
        {   
            // Считать, что левая лексема найдена
            positionOfLeftToken =  i;  
            leftToken = programCodeString[i];
        }
    }
    
    int isDataType = 0;  // Считать, что левая лексема не является типом данных
    isDataType = (iswalnum(leftToken) || leftToken == '_');  // если символ на позиции не является разделителем, Считать, что левая лексема найдена
   
    return isDataType;
}

int isBracesFound(const char programCode[MAX_STR_COUNT][MAX_STR_LEN], int stringCount, PositionInCode positionFunctionInCode)
{
    PositionInCode positionOfClosingParentheses = { -1, -1 }; // Считать, что закрывающие скобки не найдены

    //Найти позицию закрывающих круглых скобок
    for (int i = positionFunctionInCode.row; i < stringCount && positionOfClosingParentheses.row == -1; i++) // Пока не найдены закрывающие скобки,для всех элементов, начиная с позиции имени функции
    {  
        int j = (i == positionFunctionInCode.row) ? positionFunctionInCode.positionInRow : 0;
        for ( j; j < strlen(programCode[i]) && positionOfClosingParentheses.row == -1; j++)
        {
            if (programCode[i][j] == ')') // символ на позиции равен  закрывающей круглой скобке
            {
                positionOfClosingParentheses = { i , j }; //Считать, что закрывающие скобки найдены
            }
        }

    }
   
    int bracesIsFound = areThisSymbolAndFirstTokenAfterPositionEqual(programCode, stringCount, positionOfClosingParentheses, '{', 0);  //Проверить, является ли первая лексема, после  позиции закрывающих скобок фигурными 
    return bracesIsFound;
}


int isFirstCharBeforeFunctionNameValidTokenForIdentifier(const char programCode[MAX_STR_COUNT][MAX_STR_LEN], int stringCount, PositionInCode positionFunctionNameInCode) 
{
    int isValidTokenForIdentifier = 0;
    
        if (positionFunctionNameInCode.positionInRow != 0)  //  имя функции не находится в начале строки
        {

            PositionInCode positionPredElement = { positionFunctionNameInCode.row, positionFunctionNameInCode.positionInRow - 1 }; //Считать, что предыдущий элемент находится на той же строчке, что и и имя функции  и ее позиция в строке на 1 
            char predElement = programCode[positionPredElement.row][positionPredElement.positionInRow];

            if (isalnum(predElement) || predElement == '_') // предыдущий элемент является буквой, цифрой или нижним подчеркиванием
            {
                isValidTokenForIdentifier = 1; // Считать, что первый символ  является недопустимой лексемой
            }

        }

    return isValidTokenForIdentifier;
}

int isFirstCharAfterFunctionNameParenthesis(const char programCode[MAX_STR_COUNT][MAX_STR_LEN], int stringCount, const char functionName[], PositionInCode positionFunctionNameInCode)
{
    PositionInCode startPosition = { positionFunctionNameInCode.row, positionFunctionNameInCode.positionInRow + strlen(functionName) - 1 }; //Считать, что начальная позиция - конец имени функции для 
    int isParenthesis = areThisSymbolAndFirstTokenAfterPositionEqual(programCode, stringCount, startPosition, '(', 1); // Проверить, является ли первая лексема, после начальной позиции  круглыми скобками

    return isParenthesis;
}


int areThisSymbolAndFirstTokenAfterPositionEqual(const char programCode[MAX_STR_COUNT][MAX_STR_LEN], int stringCount, PositionInCode positionInCode, char symbol, int commentsAreIgnored) 
{
    PositionInCode positionFirstToken = { -1, -1 };
    
    //Найти первую лексему после имени функции
    for (int i = positionInCode.row; i < stringCount && positionFirstToken.row == - 1; i++) // Для всех строк начиная с заданной позиции
    {   
        int commentsPositionInString = strlen(programCode[i]); // читать, что комментарии находятся за последним элементом
        commentsPositionInString = (!commentsAreIgnored && strstr(programCode[i], "//") != NULL) ? strstr(programCode[i], "//") - programCode[i] : commentsPositionInString; //Если комментарии не считаются символами и комментарии находятся в строк
                                                                                                                                                                            //...Найти позицию комментариев в строке 
        int j = (i == positionInCode.row) ? positionInCode.positionInRow + 1 : 0;
        for (j; j < commentsPositionInString && positionFirstToken.row == -1; j++) // Пока не найдена первая лексема после заданной позиции в строке , для всех позиций, что находятся после начальной и до позиции комментариев
        {
            if (programCode[i][j] != ' ' && programCode[i][j] != '\t') // символ не является разделителем
            {
                positionFirstToken = { i, j }; //Считать, что первая лексема найдена
            }
        }
    }

    int isEqual = 0; // Считать, что первая лексема не равна символу
    
    if (programCode[positionFirstToken.row][positionFirstToken.positionInRow] == symbol) //первая лексема является круглыми скобками
    {
        isEqual = 1; // cчитать, что первая лексема равна заданному символу
    }

    return isEqual;
}

int isKeyword(const char word[])
{
    char KeyWords[][81] = { "auto", "break", "case", "char", "const", "continue", "default",
   "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int",
     "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct",
      "switch", "typedef", "union", "unsigned", "void", "volatile", "while" };

    int sizeOfKeyWordsArray = 34;
    int wordIsKeyword = 0; //Считать, что слово не является ключевым словом

    for (int i = 0; i < sizeOfKeyWordsArray; i++)  //Для всех ключевых слов
    {
        if (!strcmp(word, KeyWords[i])) //слово равно ключевому слову
        {
            wordIsKeyword = 1; //Считать, что слово является ключевым словом
        }

    }

    return wordIsKeyword;
}


