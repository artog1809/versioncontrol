#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->dateOfBirthdayEdit->setMaximumDate(QDate::currentDate());

    connect(ui->resultBtn, SIGNAL(clicked()), this, SLOT(outputResult()));
}

void MainWindow::outputResult()
{
    qDebug() << "result active";
    person.setName(ui->nameEdit->text());
    person.setDateOfBirthday(ui->dateOfBirthdayEdit->date());

    if(checkCorrectInput())
    {
        QString text = searchAge();
        ui->result->setText(text);
    }
    else
    {
        ui->result->setText(QString("error"));
    }

}

bool MainWindow::checkCorrectInput()
{
    if(ui->nameEdit->text() == "")
        return false;

    return true;
}

QString MainWindow::searchAge()
{
    int dayPerson = person.getDateOfBirthday().day();
    int monthPerson = person.getDateOfBirthday().month();
    int yearPerson = person.getDateOfBirthday().year();

    int year = 0;
    int month = 0;
    int day = 0;

    if (QDate::currentDate().year() > yearPerson)
    {
        year = QDate::currentDate().year() - yearPerson;
    }

    if (QDate::currentDate().month() > monthPerson)
    {
        month = QDate::currentDate().month() - monthPerson;
    }
    else if (QDate::currentDate().month() < monthPerson)
    {
        year--;
        int currentMonth = QDate::currentDate().month();
        currentMonth += 12;
        month = currentMonth - monthPerson;
    }

    if(QDate::currentDate().day() > dayPerson)
    {
        day = QDate::currentDate().day() - dayPerson;
    }
    else if (QDate::currentDate().day() < dayPerson)
    {
        month--;
        int currentDay = QDate::currentDate().day();
        currentDay += QDate::currentDate().daysInMonth();
        day = currentDay - dayPerson;
    }

    return person.getName() + ", " + QString::number(year) + "лет, " + QString::number(month) + "месяцев, " + QString::number(day) + "дней";
}

MainWindow::~MainWindow()
{
    delete ui;
}

