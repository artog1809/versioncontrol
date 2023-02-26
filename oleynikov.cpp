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
    ui->choiceMoney->setEnabled(false);

    QValidator* validator = new NominalValidator(this);
    ui->nominal->setValidator(validator);

    connect(qApp , SIGNAL(focusChanged(QWidget*,QWidget*)),this,SLOT(saveMoney()));
    connect(ui->money1,SIGNAL(clicked()), this, SLOT( changeFocus()));
    connect(ui->money2,SIGNAL(clicked()), this, SLOT( changeFocus()));
    connect(ui->ourEra, SIGNAL(clicked()),this,SLOT(activateYear()));
    connect(ui->notOurEra,SIGNAL(clicked()),this,SLOT(activateCentury()));
    connect(ui->rarity,SIGNAL(clicked()),this,SLOT(activateCount()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

Numizmat* MainWindow:: choiceMoney(){
    Numizmat* numizmat;

    numizmat = &numizmats[focus-1];

    return numizmat;
}

void  MainWindow:: changeFocus(){

     QObject* button = QObject::sender();
    if((focus==1 && ui->money2==button) || (focus==2 && ui->money1==button)){
       changeMoneyEntry();
    }
}

void MainWindow:: changeMoneyEntry(){

    std::swap(predFocus,focus);
    loadMoneyEntry();
    ui->choiceMoney->setText(QString::number(focus));

}

void  MainWindow:: saveMoney(){
    Numizmat *numizmat = choiceMoney();

    numizmat->setName(ui->name->text());
    numizmat->setNominal(ui->nominal->text());
    numizmat->setEra(getEra());
    numizmat->setRarity(ui->rarity->isChecked());
    numizmat->setCount(ui->count->value());
    numizmat->setOrigin(getOrigin());
    numizmat->setFeatures(ui->feature->text());

}

void MainWindow:: loadMoneyEntry(){

    Numizmat *numizmat = choiceMoney();
    ui->name->setText(numizmat->getName());
    ui->nominal->setText(numizmat->getNominal());
    ui->rarity->setChecked(numizmat->getRarity());
    ui->count->setValue(numizmat->getCount());
    ui->feature->setText(numizmat->getFeatures());

    setEra();
    setOrigin();

}

void MainWindow:: setEra(){
    Numizmat* numizmat = choiceMoney();
    QString era = numizmat->getEra();

    if(era=="OUR_ERA") ui->ourEra->setChecked(true);
    if(era=="NOTOUR_ERA") ui->notOurEra->setChecked(true);

}


QString MainWindow:: getEra(){

    if(ui->ourEra->isChecked()) return "OUR_ERA";
    if(ui->notOurEra->isChecked()) return "NOTOUR_ERA";
    return "OUR_ERA";
}

void MainWindow:: setOrigin(){
    Numizmat* numizmat = choiceMoney();
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









