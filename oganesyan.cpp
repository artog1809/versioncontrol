#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cat.h"
#include "fraction.h"
#include <QDate>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QDataStream>
#include <QDate>
#include <myellipse.h>
#include <intstack.h>

QDataStream& operator<<(QDataStream& stream, const Cat & cat)
{
    stream << cat.name << cat.length << cat.weigthValue;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, Cat& cat)
{
    stream >> cat.name >> cat.length >> cat.weigthValue;
    return stream;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->save, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->load, SIGNAL(clicked()), this, SLOT(load()));

    this->cat.Meow();
    this->cat.Meow("Hey!");

    MyCircle goodCircle;
    goodCircle.setLength(30);
    goodCircle.getLength();



    IntStack stack;
    stack.push(12);
    stack.push(54);
    stack.push(42);

    qDebug() << stack.size();

    int firstValue = stack.pop();
    int secondValue = stack.pop();

    qDebug() << firstValue << " " << secondValue;

    qDebug() << stack.size();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::save()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Открыть файл",
                                                    "D:\\Vstu",
                                                    "TXT (*.txt);; Все файлы (*.*)");
    QTextStream conout(stdout);
    conout << "qwerty" << 123;

    QMap<QString, Cat> cats;
    cats.insert("Волгоград", Cat("cat1", 10));
    cats.insert("Волжский", Cat("cat2", 20));
    if (!filename.isEmpty())
    {
        QFile txtFile(filename);
        bool isOpenCorrect = txtFile.open(QIODevice::WriteOnly);
        if(isOpenCorrect)
        {
            QDataStream output(&txtFile);
            qDebug() << "Stream is created";
            output << cats;
        }

        txtFile.close();
    }
    qDebug() << filename;
}

void MainWindow::load()
{
    //ui->name->setText(this->cat.name);
    //ui->weigth->setValue(this->cat.weigth);
    QMessageBox mBox;

    Cat catTwo;
    mBox.setText("Loaded");
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Открыть файл",
                                                    "D:\\Vstu",
                                                    "TXT (*.txt);; Все файлы (*.*)");
    QMap<QString, Cat> cats;
    if (!filename.isEmpty())
    {
        QFile txtFile(filename);
        bool isOpenCorrect = txtFile.open(QIODevice::ReadOnly);
        if(isOpenCorrect)
        {
            QDataStream input(&txtFile);
            qDebug() << "Stream is created";
            input >> cats;
        }

        txtFile.close();
    }
    qDebug() << cats["Волгоград"].name << cats["Волгоград"].weigth();
    mBox.exec();
}

