#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <algorithm>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->recipeText->setEnabled(false);
    connect(ui->recipeText,SIGNAL(textChanged()),this,SLOT(cropRecipeText()));
    connect(ui->deleteButton,SIGNAL(clicked()), this, SLOT( deleteRecipeEntry()));
    connect(ui->createButton,SIGNAL(clicked()), this, SLOT( createRecipeEntry()));
    connect(ui->fillButton,SIGNAL(clicked()), this, SLOT(fillBaseDate()));
    connect(ui->ingredientList,SIGNAL(textChanged()),this,SLOT(cropIngridientText()));
    connect(ui->ingredientList, SIGNAL(textChanged()),this,SLOT(activateRecipeText()));
    connect(ui->BaseDate, SIGNAL(currentRowChanged(int)), this, SLOT(loadRecipeEntry()));
    connect(qApp , SIGNAL(focusChanged(QWidget*, QWidget*)),this,SLOT(saveRecipeEntry()));
    connect(qApp , SIGNAL(focusChanged(QWidget*, QWidget*)),this,SLOT(updateRecipeEntry()));

    srand(time(0));

     setRedactorEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow:: fillBaseDate(){

    QString name[10] = {"Пирожок", "Цезарь", "Борщ", "Пюре", "Молочная каша","Курица", "Яичница","Жаренный карась", "Тортик", "Бутерброд"};
    bool isDietary[2] = {0,1};
    int energy[5] = {100, 500, 1000, 1200, 4500};
    QTime time1(1,15), time2(0,45), time3(2,10);
    QTime time[3] = {time1,time2,time3};
    QString typeOfDish[4] ={"GARNISH","SALAD", "SAUCE", "HOTDISH"};
    QString ingridient = "";
    QString recipe = "";

    int lastIndex = lastRecipeEntryIndex;
    for(int i =lastIndex; i<std::min(20,lastIndex + 10); i++){
        RecipeEntry recipeEntryTmp(name[rand()%10], isDietary[rand()%2], energy[rand()%5], time[rand()%3], typeOfDish[rand()%4],ingridient,recipe );
        int index = 0;
        findPositionInBD(index, recipeEntryTmp);
        QString stringToBD = createStringForDB(recipeEntryTmp);
        qDebug()<<index<<" "<<recipeEntryTmp.getName()<<" \n";

        ui->BaseDate->insertItem(index,stringToBD);
    }

}

RecipeEntry* MainWindow:: choiceRecipeEntry(){
    int index = ui->BaseDate->currentRow();
     RecipeEntry *recipeEntry;

    if(index != -1)
        recipeEntry = &recipeEntries[index];

    return recipeEntry;
}

void MainWindow:: createRecipeEntry(){

    if(lastRecipeEntryIndex<MAX_RECIPE_ENTRIES){
    QTime time(1,15);
    RecipeEntry defaultRecipeEntry("Название блюда",  false,  150,  time,"GARNISH","","");
    int index = 0 ;
    findPositionInBD(index, defaultRecipeEntry);
    QString stringToBD = createStringForDB(defaultRecipeEntry);
    ui->BaseDate->insertItem(index,stringToBD);
    ui->BaseDate->setCurrentRow(index);
    }

}

void MainWindow:: findPositionInBD(int &index, RecipeEntry &recipeEntry){
    while(index<lastRecipeEntryIndex && recipeEntry >recipeEntries[index]){
        index++;
    }

    if(index>=MAX_RECIPE_ENTRIES || lastRecipeEntryIndex==MAX_RECIPE_ENTRIES)
        return;

    for(int i = lastRecipeEntryIndex; i>index; i--){
        recipeEntries[i] = recipeEntries[i-1];
    }
    recipeEntries[index] = recipeEntry;

    lastRecipeEntryIndex++;

}

void MainWindow:: createStringWithNum(int num, QString string, QString firstAlternativeString, QString secondAlternativeString,QString &result){


    if ((num<=10 || num>= 19) && num%10 && num%10<5){

        if(num%10==1) result =  QString:: number(num)+" " + string;
            else if (num%10>1 && num%10<5) result = QString:: number(num)+ " " + firstAlternativeString;

    }  else result = QString:: number(num)+ " " + secondAlternativeString;

}

QString MainWindow:: createStringForDB(RecipeEntry &recipeEntry){

    QString name =recipeEntry.getName();
    QString  typeOfDish = recipeEntry.getTypeOfDish();
    QTime time = recipeEntry.getTime();

    QString hour;
    QString minute;

    if(time.hour()){

        createStringWithNum(time.hour(),QString("час"),QString("часа"),QString("часов"),hour);
    }

    if(time.minute()){
        createStringWithNum(time.minute(),QString("минута"),QString("минуты"),QString("минут"),minute);
    }

    QString stringToBD = name+"\t" + typeOfDish +"\t" + hour+" "+ minute;

    return stringToBD;
}


void MainWindow:: deleteRecipeEntry(){
    int index = ui->BaseDate->currentRow();

    if(index == -1) return;


    for(int i = index; i<lastRecipeEntryIndex-1; i++){
        recipeEntries[i] = recipeEntries[i+1];
    }

    lastRecipeEntryIndex--;
    if(ui->BaseDate->count()!=1) delete ui->BaseDate->takeItem(ui->BaseDate->currentRow());
    else{
        ui->BaseDate->blockSignals(true);
        ui->BaseDate->clear();
        ui->BaseDate->blockSignals(false);
        setRedactorEnabled(false);
    }
}

void MainWindow:: updateRecipeEntry(){

    if(!ui->BaseDate->count() || ui->BaseDate->currentRow()==-1) return;

    int initialIndex = ui->BaseDate->currentRow();
    int index = initialIndex;
    saveRecipeEntry();

    if(!isRightRecipeEntry() || !checkAllIngredients()) return;
    if(activeRecipeEntry == recipeEntries[index]) return;
    recipeEntries[index] = activeRecipeEntry;

    while(index != 0 && (recipeEntries[index-1] > recipeEntries[index])){
        swapRecipeEnty(recipeEntries[index-1], recipeEntries[index]);
        index--;
    }

    while(index+1<lastRecipeEntryIndex && (recipeEntries[index+1] < recipeEntries[index])){
        swapRecipeEnty(recipeEntries[index+1], recipeEntries[index]);
        index++;
    }

    int pasteIndex = index;
    QString stringForDB = createStringForDB(recipeEntries[index]);

    if(ui->BaseDate->count()!=1) delete ui->BaseDate->takeItem(ui->BaseDate->currentRow());
    else{
        ui->BaseDate->blockSignals(true);
        ui->BaseDate->clear();
        ui->BaseDate->blockSignals(false);
         setRedactorEnabled(false);
    }

    ui->BaseDate->insertItem(pasteIndex,stringForDB);
    ui->BaseDate->setCurrentRow(pasteIndex);
}

void MainWindow:: swapRecipeEnty(RecipeEntry &recipeEntry1, RecipeEntry &recipeEntry2){


    RecipeEntry recipeEntryTmp =  recipeEntry1;
    recipeEntry1 = recipeEntry2;
    recipeEntry2 = recipeEntryTmp;
}

void  MainWindow:: saveRecipeEntry(){

    if(ui->ingredientList->toPlainText().isEmpty()){
        ui->recipeText->setText("");
    }

    activeRecipeEntry.setName(ui->dishNameEdit->text());
    activeRecipeEntry.setDietary(ui->dietCheckBox->isChecked());
    activeRecipeEntry.setEnergy(ui->energySpinBox->value());
    activeRecipeEntry.setIngridients(ui->ingredientList->toPlainText());
    activeRecipeEntry.setRecipe(ui->recipeText->toPlainText());
    activeRecipeEntry.setTime(ui->cookingTimeEdit->time());
    activeRecipeEntry.setTypeOfDish(getTypeOfDish());
}

void MainWindow:: setRedactorEnabled(bool isEnable){

   ui->dishNameEdit->setEnabled(isEnable);
   ui->dietCheckBox->setEnabled(isEnable);
   ui->energySpinBox->setEnabled(isEnable);
   ui->ingredientList->setEnabled(isEnable);
   ui->recipeText->setEnabled(isEnable);
   ui->cookingTimeEdit->setEnabled(isEnable);

   //Button
   ui->saladButton->setEnabled(isEnable);
   ui->garnishButton->setEnabled(isEnable);
   ui->sauceButton->setEnabled(isEnable);
   ui->hotDishButton->setEnabled(isEnable);
}

void MainWindow:: loadRecipeEntry(){

    RecipeEntry *recipeEntry = choiceRecipeEntry();

    ui->dishNameEdit->setText(recipeEntry->getName());
    ui->dietCheckBox->setChecked(recipeEntry->getDietary());
    ui->energySpinBox->setValue(recipeEntry->getEnergy());
    ui->ingredientList->setText(recipeEntry->getIngridients());
    ui->recipeText->setText(recipeEntry->getRecipe());
    ui->cookingTimeEdit->setTime(recipeEntry->getTime());

    setTypeOfDish();
    setRedactorEnabled(true);
}

void MainWindow:: setTypeOfDish(){
    RecipeEntry* recipeEntry = choiceRecipeEntry();
    QString typeOfDish = recipeEntry->getTypeOfDish();

    if(typeOfDish=="SALAD") ui->saladButton->setChecked(true);
    if(typeOfDish=="SAUCE") ui->sauceButton->setChecked(true);
    if(typeOfDish=="GARNISH") ui->garnishButton->setChecked(true);
    if(typeOfDish=="HOTDISH") ui->hotDishButton->setChecked(true);
}

QString MainWindow:: getTypeOfDish(){

    if(ui->saladButton->isChecked()) return "SALAD";
    if(ui->sauceButton->isChecked()) return "SAUCE";
    if(ui->garnishButton->isChecked()) return "GARNISH";
    if(ui->hotDishButton->isChecked())  return "HOTDISH";
    return "NOTHING";
}

void MainWindow:: activateRecipeText(){

    QString ingridientText = ui->ingredientList->toPlainText();

    if(ingridientText.isEmpty())
        ui->recipeText->setEnabled(false);
    else
        ui->recipeText->setEnabled(true);

}

void MainWindow:: cropIngridientText(){
    if(ingridientChanged)  return;

    const int MAX_LINE_COUNT =20;
    const int MAX_LINE_LENGTH = 50;

    QTextCursor textCursor =  ui->ingredientList->textCursor();
    int position = ui->ingredientList->textCursor().position();

    QString str = ui->ingredientList->toPlainText();
    QStringList list = str.split('\n');
    QString str2;
    int listSize = list.size();

    for(int i = 0; i<listSize; i++){

        if(list[i].size()>MAX_LINE_LENGTH){
            list[i].remove(MAX_LINE_LENGTH,list[i].size());
        }

       if(i>MAX_LINE_COUNT-1)
           list[i].remove(0,list[i].size());

       if( i<listSize-1 && isEmptyString(list[i]))
           std:: swap(list[i], list[i+1]);

        str2+=list[i];
        str2+="";

        if(i<list.size()-1 && list[i].size()) str2+="\n";

    }

    ingridientChanged =1;
    ui->ingredientList->setText(str2);
    ingridientChanged = 0;
    textCursor.setPosition(position);
    ui->ingredientList->setTextCursor(textCursor);
}

void MainWindow:: cropRecipeText(){
   int MAX_TEXT_LENGTH = 1000;

   QTextCursor textCursor =  ui->recipeText->textCursor();
   int position = ui->recipeText->textCursor().position();
   if(position) position--;

   QString str = ui->recipeText->toPlainText();
   if(str.size()>MAX_TEXT_LENGTH){
       str.remove(MAX_TEXT_LENGTH,str.size()-1);
       ui->recipeText->setText(str);
       textCursor.setPosition(position);
   }

   ui->recipeText->setTextCursor(textCursor);
}

bool MainWindow:: checkAllIngredients(){
    bool isAllIngredients = 1;
    QStringList ingredients = ui->ingredientList->toPlainText().split('\n');
    QString recipe = ui->recipeText->toPlainText();

    for(int i = 0; i<ingredients.size(); i++){
        isAllIngredients *= ingridientIsFound(recipe, ingredients[i]);
    }

    return isAllIngredients;
}

bool MainWindow:: isEmptyString(QString string){
     bool isEmpty = 1;

     for(int i = 0; i<string.size(); i++){
         if(string[i]!='\n' && string[i]!=' ' && string[i]!='\t')
             isEmpty=0;
     }

     return isEmpty;
}

bool MainWindow:: ingridientIsFound(QString recipe, QString ingredient){
    bool isFound = 0;

    if(isEmptyString(ingredient)) isFound = 1;

    while(recipe.size() && !isFound){

        bool isNotSuffix = 1, isNotPrefix = 1;
        int index = recipe.indexOf(ingredient);
        int lastIndex = index+ingredient.size()-1;

        if(index==-1) break;
        if(index>0 && recipe[index-1].isLetterOrNumber()) isNotSuffix = 0;

        if(lastIndex<recipe.size()-1 && recipe[lastIndex+1].isLetterOrNumber()){
             isNotPrefix = 0;

             while(lastIndex<recipe.size()-1 && recipe[lastIndex+1].isLetterOrNumber()){
                 lastIndex++;
             }
        }

        isFound = isNotSuffix && isNotPrefix;
        recipe.remove(0,lastIndex+1);
     }

     return isFound;
}

bool MainWindow:: isRightRecipeEntry(){
    QString name = activeRecipeEntry.getName();
    QString  typeOfDish = activeRecipeEntry.getTypeOfDish();
    QTime time = activeRecipeEntry.getTime();

    return !(name.isEmpty() || (!time.hour() && !time.minute()) || typeOfDish=="NOTHING");
}

