#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nominalvalidator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->count->setEnabled(false);
    ui->year->setEnabled(false);
    ui->century->setEnabled(false);

    QValidator* validator = new NominalValidator(this);
    ui->nominal->setValidator(validator);

    connect(ui->ourEra, SIGNAL(clicked()),this,SLOT(activateYear()));
    connect(ui->notOurEra,SIGNAL(clicked()),this,SLOT(activateCentury()));
    connect(ui->rarity,SIGNAL(clicked()),this,SLOT(activateCount()));
    connect(ui->fillBtn,SIGNAL(clicked()),this,SLOT(fillBaseDate()));
    connect(ui->deleteBtn,SIGNAL(clicked()),this,SLOT(deleteMoney()));
    connect(ui->createBtn,SIGNAL(clicked()),this,SLOT(createMoney()));
    connect(ui->BaseDate,SIGNAL(currentRowChanged(int)),this,SLOT(loadMoney()));
    connect(qApp , SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(saveMoney()));
    connect(qApp , SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(updateMoney()));

    setRedactorEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow:: fillBaseDate(){

    QString name[10] = {"1 Рубль","5 Копеек","Полполтины","Гривна","Полтина",
                        "Пробка","Империал","Червонец","Денга","Алтын"};
    bool rarity[2] = {0,1};
    int count[5] = {100, 200, 0, 1000, 250};
    QString origin[3] ={"IMPERIA","USSR", "RUSSIA"};
    QString nominal = "";
    QString era[2] = {"OUR_ERA", "NOTOUR_ERA"};
    int year[10] = {1900,2000,1732,1726,1813,2012,2000,1754,1912,1927};
    int century[4] = {18,19,20,21};
    QString features = "";
    ui->year->setEnabled(true);

    int lastIndex = lastMoneyIndex;
    for(int i =lastIndex; i<std::min(20,lastIndex + 10); i++){
        Numizmat numizmatTmp(name[rand()%10], rarity[rand()%2], count[rand()%5],
                 origin[rand()%3], era[0], year[rand()%10], century[rand()%4], nominal, features);
        numizmatTmp.setYear(year[rand()%10]);
        int index = 0;
        findPositionInBD(index, numizmatTmp);
        QString stringToBD = createStringForDB(numizmatTmp);


        ui->BaseDate->insertItem(index,stringToBD);
    }

}



Numizmat* MainWindow:: choiceMoneyEntry(){
    int index = ui->BaseDate->currentRow();
        Numizmat *numizmat;

    if(index != -1)
        numizmat = &numizmats[index];

    return numizmat;
}

void MainWindow:: createMoney(){
    if(lastMoneyIndex<MAX_MONEY_ENTRIES){
    Numizmat defaultMoneyEntry("Название",  true,  300, "IMPERIA" ,"OUR_ERA", 100, 0,"100 рублей","крутая монета");
    defaultMoneyEntry.setYear(100);
    ui->year->setEnabled(true);
    int index = 0 ;
    findPositionInBD(index, defaultMoneyEntry);
    QString stringToBD = createStringForDB(defaultMoneyEntry);
    ui->BaseDate->insertItem(index,stringToBD);
    ui->BaseDate->setCurrentRow(index);
    }
    updateMoney();
}


void MainWindow:: findPositionInBD(int &index, Numizmat &numizmat){
    while(index<lastMoneyIndex && numizmat >numizmats[index]){
        index++;
    }

    if(index>=MAX_MONEY_ENTRIES || lastMoneyIndex==MAX_MONEY_ENTRIES)
        return;

    for(int i = lastMoneyIndex; i>index; i--){
        numizmats[i] = numizmats[i-1];
    }
    numizmats[index] = numizmat;

    lastMoneyIndex++;

}

QString MainWindow:: createStringForDB(Numizmat &numizmat){

    QString name = numizmat.getName();
    int count = numizmat.getCount();
    if (numizmat.getEra()=="NOTOUR_ERA")
    {
        int century = numizmat.getCentury();
        QString centuryStr = QString::number(century);
        QString countStr = QString::number(count);
        QString stringToBD = name+"\t"+ centuryStr +" до н.э" +"\t" + countStr;
        return stringToBD;
    }
    else
    {
        int year = numizmat.getYear();
        QString yearStr = QString::number(year);
        QString countStr = QString::number(count);
        QString stringToBD = name+"\t" +" "+ yearStr +"\t" + countStr;
        return stringToBD;
    }
}

void MainWindow:: deleteMoney(){
    int index = ui->BaseDate->currentRow();

    if(index == -1) return;


    for(int i = index; i<lastMoneyIndex-1; i++){
        numizmats[i] = numizmats[i+1];
    }

    lastMoneyIndex--;
    if(ui->BaseDate->count()!=1) delete ui->BaseDate->takeItem(ui->BaseDate->currentRow());
    else{
        ui->BaseDate->blockSignals(true);
        ui->BaseDate->clear();
        ui->BaseDate->blockSignals(false);
        setRedactorEnabled(false);
    }
    if(index!=0)
        ui->BaseDate->setCurrentRow(index-1);

}

void MainWindow:: updateMoney(){

    if(!ui->BaseDate->count() || ui->BaseDate->currentRow()==-1) return;

    int initialIndex = ui->BaseDate->currentRow();
    int index = initialIndex;
    saveMoney();


    if(activeMoney == numizmats[index]) return;
    numizmats[index] = activeMoney;


    while(index != 0 && (numizmats[index-1] > numizmats[index])){
        swapMoneyEnty(numizmats[index-1], numizmats[index]);
        index--;
    }

    while(index+1<lastMoneyIndex && (numizmats[index+1] < numizmats[index])){
        swapMoneyEnty(numizmats[index+1], numizmats[index]);
        index++;
    }

    int pasteIndex = index;
    QString stringForDB = createStringForDB(numizmats[index]);

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

void MainWindow:: swapMoneyEnty(Numizmat &money1, Numizmat &money2){


    Numizmat numizmatTmp =  money1;
    money1 = money2;
    money2 = numizmatTmp;
}


void  MainWindow:: saveMoney(){

    activeMoney.setName(ui->name->text());
    activeMoney.setNominal(ui->nominal->text());
    activeMoney.setYear(ui->year->value());
    activeMoney.setCentury(ui->century->value());
    activeMoney.setEra(getEra());
    activeMoney.setFeatures(ui->feature->text());
    activeMoney.setRarity(ui->rarity->isChecked());
    activeMoney.setCount(ui->count->value());
    activeMoney.setOrigin(getOrigin());
}

void MainWindow:: setRedactorEnabled(bool isEnable){

    ui->name->setEnabled(isEnable);
    ui->nominal->setEnabled(isEnable);
    ui->rarity->setEnabled(isEnable);
    ui->count->setEnabled(isEnable);
    ui->feature->setEnabled(isEnable);

    ui->ourEra->setEnabled(isEnable);
    ui->notOurEra->setEnabled(isEnable);
    ui->imperia->setEnabled(isEnable);
    ui->ussr->setEnabled(isEnable);
    ui->russia->setEnabled(isEnable);
 }

void MainWindow:: loadMoney(){

    Numizmat *numizmat = choiceMoneyEntry();

    ui->name->setText(numizmat->getName());
    ui->rarity->setChecked(numizmat->getRarity());
    ui->count->setValue(numizmat->getCount());
    ui->nominal->setText(numizmat->getNominal());
    ui->feature->setText(numizmat->getFeatures());
    ui->year->setValue(numizmat->getYear());
    ui->century->setValue(numizmat->getCentury());


    setEra();
    setOrigin();
    setRedactorEnabled(true);
}



void MainWindow:: setEra(){
    Numizmat* numizmat = choiceMoneyEntry();
    QString era = numizmat->getEra();

    if(era=="OUR_ERA") ui->ourEra->setChecked(true);
    if(era=="NOTOUR_ERA"){
        numizmat->setYear(-100*numizmat->getCentury());
        ui->notOurEra->setChecked(true);
    }
}


QString MainWindow:: getEra(){

    if(ui->ourEra->isChecked()) return "OUR_ERA";
    if(ui->notOurEra->isChecked()){
        activeMoney.setYear(-100*activeMoney.getCentury());
        ui->notOurEra->setChecked(true);
        return "NOTOUR_ERA";}
    return "OUR_ERA";
}

void MainWindow:: setOrigin(){
    Numizmat* numizmat = choiceMoneyEntry();
    QString origin = numizmat->getOrigin();

    if(origin=="IMPERIA") ui->imperia->setChecked(true);
    if(origin=="USSR") ui->ussr->setChecked(true);
    if(origin=="RUSSIA") ui->russia->setChecked(true);

}


QString MainWindow:: getOrigin(){

    if(ui->imperia->isChecked()) return "IMPERIA";
    if(ui->ussr->isChecked()) return "USSR";
    if(ui->russia->isChecked()) return "RUSSIA";
    return "RUSSIA";
}


void MainWindow:: activateYear(){

    if(ui->ourEra->isChecked())
    {
        ui->year->setEnabled(true);
        ui->century->setEnabled(false);
    }
    else
        ui->year->setEnabled(false);

}

void MainWindow:: activateCentury(){

    if(ui->notOurEra->isChecked())
    {
        ui->century->setEnabled(true);
        ui->year->setEnabled(false);
    }
    else
        ui->century->setEnabled(false);
}

void MainWindow:: activateCount(){

    if(ui->rarity->isChecked())
        ui->count->setEnabled(true);

    else
        ui->count->setEnabled(false);
}









