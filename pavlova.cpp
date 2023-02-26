#include <numizmat.h>

Numizmat::Numizmat()
{
}

Numizmat::Numizmat(QString name,bool rarity,int count,QString origin,QString era, int year, int century,
                   QString nominal, QString features)
{
    this->name = name;
    this->rarity = rarity;
    this->era = era;
    this->nominal = nominal;
    this->origin = origin;
    this->count = count;
    this->features = features;
}

void Numizmat::setName(const QString &name){
    this->name = name;
}
void Numizmat::setNominal(const QString &nominal){
    this->nominal = nominal;
}
void Numizmat:: setEra(const QString &era){
    this->era = era;
}
void Numizmat::setYear(const int &year){
   this->year = year;
}
void Numizmat::setCentury(const int &century){
   this->century = century;
}
void Numizmat::setRarity(bool rarity){
    this->rarity = rarity;
}
void Numizmat::setCount(const int &count){
    this->count = count;
}
void Numizmat::setOrigin(const QString &origin){
    this->origin = origin;
}
void Numizmat::setFeatures(const QString &features){
    this->features = features;
}




QString Numizmat:: getName() const{
    return this->name;
}
QString Numizmat:: getNominal() const{
    return  this->nominal;
}
QString Numizmat:: getEra() const{
    return this->era;
}
int Numizmat:: getYear() const{
    return  this->year;
}
int Numizmat:: getCentury() const{
    return  this->century;
}
bool Numizmat:: getRarity() const{
    return this->rarity;
}
int Numizmat:: getCount() const{
    return this->count;
}
QString Numizmat:: getOrigin() const{
    return this->origin;
}
QString Numizmat::getFeatures() const{
    return this->features;
}


bool Numizmat:: operator ==(Numizmat &other){
    bool res = 1;

    if (this->name!=other.name) res = 0;
    if (this->nominal!=other.nominal) res = 0;
    if (this->era!= other.era) res = 0;
    if (this->year != other.year) res = 0;
    if (this->century!=other.century) res = 0;
    if (this->rarity!= other.rarity) res = 0;
    if (this->count!=other.count) res = 0;
    if (this->origin!=other.origin) res = 0;
    if (this->features!=other.features) res = 0;

    return  res;

}

bool Numizmat:: operator !=(Numizmat &other){

    return !(this==&other);
}

bool Numizmat:: operator <(Numizmat &other){

    if(this->rarity!=other.rarity) return this->rarity < other.rarity;

    if(this->year!=other.year) return this->year < other.year;

    return this->name < other.name;
}

bool Numizmat:: operator >=(Numizmat &other){

    return !(this < &other);
}

bool Numizmat:: operator >(Numizmat &other){


    if(this->rarity!=other.rarity) return this->rarity > other.rarity;

    if(this->year!=other.year) return this->year > other.year;

    return this->name > other.name;
}

bool Numizmat:: operator <=(Numizmat &other){

    return ! (this > &other);
}

void Numizmat:: operator=(Numizmat &other){
    this->name = other.name;
    this->nominal = other.nominal;
    this->era = other.era;
    this->year = other.year;
    this->century = other.century;
    this->rarity = other.rarity;
    this->count = other.count;
    this->origin = other.origin;
    this->features = other.features;
}


