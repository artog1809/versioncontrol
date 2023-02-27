#include "truthtablesystem.h"

/*!
 * \file
 * \brief Файл, содержащий реализацию TruthTableSystem
 */

TruthTableSystem::TruthTableSystem()
{
    variablesAmount = 0;
}

TruthTableSystem::~TruthTableSystem()
{
    for (int i = 0; i < nodes.length(); i++)
        delete nodes[i];
}

void TruthTableSystem::setXRootNode(QDomNode xNode)
{
    this->xRootNode = xNode;
}

short *TruthTableSystem::getTruthTable(QStringList &subExpressions, int &rowAmount, int &columnAmount)
{
    parseExpression(xRootNode); // составить векор узлов выражения на основе его корня

    // Составить список подвыражения на основе строковых представлений узлов
    for (int i = 0; i < nodes.length(); i++)
        subExpressions.append(nodes[i]->getStringId());

    // Составить таблицу истинности на основе векора узлов
    rowAmount = pow(2, variablesAmount);
    columnAmount = nodes.length();

    return makeTruthTable();
}

Node* TruthTableSystem::parseExpression(QDomNode &xNode)
{
    Node* result;
    if (xNode.toElement().tagName() == "variable") // если xml узел соответствует узлу переменной
    {
        // Создать новый узел переменной, со строковым представлением равным имени переменной
        errorController.isVariableCorrect(xNode);
        Variable* variable = new Variable();
        variable->setStringId(xNode.toElement().text().trimmed());
        result = variable;
    }
    else
    {
        // Создать операцию типа, содержащегося в атрибуте type xml узла
        errorController.isOperationCorrect(xNode);
        Operation* operation = Operation::getOperation(xNode.toElement().attributeNode("type").value());

        QDomNodeList children = xNode.childNodes();
        Node* childNodes[2] = {NULL, NULL};
        for (int i = 0; i < children.length(); i++) // для всех дочерних узлов xml узла
        {
            // Преобразовать дочерний узел xml узла в объект класса Node
            QDomNode child = children.item(i);
            childNodes[i] = parseExpression(child);
        }

        operation->setOperands(childNodes[0], childNodes[1]); // задать дочернии узлы операндами данной операции
        result = operation;
    }

    Node* desiredNode = findNode(result); // найти созданный узел в векторе
    if (desiredNode != NULL) // если узел найден
    {
        // Вернуть указатель на найденный узел
        delete result;
        return desiredNode;
    }
    else
    {
        // Добавить созданный узел в вектор
        variablesAmount += insertNode(result); // если была добавлена переменная, инкрементировать кол-во переменных
        return result; // вернуть указатель на созданный узел
    }
}

Node* TruthTableSystem::findNode(Node *node)
{
    Node* desiredNode = NULL; // считать узел ненайденным

    for (int i = 0; i < nodes.length() && desiredNode == NULL; i++) // для каждого элемента вектора
        if (nodes[i]->getStringId() == node->getStringId()) // если строковое представление текущего элемента равен искомому
            desiredNode = nodes[i]; // считать текущий элемент искомым

    return desiredNode;
}

int TruthTableSystem::insertNode(Node *node)
{
    int isVariable = 0; // считать заданный узел операцией
    if (dynamic_cast<Variable*>(node)) // если заданный узел - переменная
    {
        int index = 0;

        // Найти позицию первого узла не являющегося переменной
        while (index < nodes.length() && dynamic_cast<Variable*>(nodes[index]))
            index++;

        // Вставить узел на найденную позицию
        nodes.insert(index, node);
        isVariable = 1; // считать заданный узел переменной
    }
    else
    {
        // Добавить узел в конец вектора
        nodes.append(node);
    }
    return isVariable;
}

short *TruthTableSystem::makeTruthTable()
{
    // Считать кол-во строк в таблице равным 2^variableAmount
    int rowAmount = pow(2, variablesAmount);
    int columnAmount = nodes.length();

    short* values = (short*)malloc(rowAmount * columnAmount * sizeof (short));

    for (int i = 0; i < rowAmount; i++) // для каждого значения от 0 до кол-ва строк
    {
        // Преобразовать текущее значение в двоичной системе
        QString stringAmount = QString("%1").arg(i, variablesAmount, 2, QChar('0'));

        // Каждой переменной выражения присвоить соответствующую цифру двоичной записи текущего значения
        for (int j = 0; j < variablesAmount; j++)
            ((Variable*)nodes[j])->setValue(QString(stringAmount[j]).toInt());

        // Добавить в строку значения всех узлов выражения
        for (int j = 0; j < columnAmount; j++)
            *(values + i*columnAmount + j) = nodes[j]->getValue();

    }
    return values;
}
