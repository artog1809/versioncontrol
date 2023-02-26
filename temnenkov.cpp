#include <recipeentry.h>
#include <QDebug>
 RecipeEntry:: RecipeEntry(){

}

 RecipeEntry:: RecipeEntry(QString name, bool isDietary, int energy, QTime time, QString typeOfDish, QString ingridient, QString recipe){
     this->name  = name;
     this->isDietary = isDietary;
     this->energy = energy;
     this->time = time;
     this->typeOfDish = typeOfDish;
     this->ingridients = ingridient;
 }

 //-------------------SET--------------------------
 void RecipeEntry::setName(const QString &name){
     this->name = name;
 }
 void RecipeEntry::setTypeOfDish(const QString typeOfDish){
     this->typeOfDish = typeOfDish;
 }
 void RecipeEntry:: setDietary(bool isDietary){
     this->isDietary = isDietary;
 }
 void RecipeEntry::setTime(const QTime &time){
    this->time = time;
 }
 void RecipeEntry::setIngridients(const QString &ingridients){
     this->ingridients = ingridients;
 }
 void RecipeEntry::setRecipe(const QString &recipe){
     this->recipe = recipe;
 }
 void RecipeEntry::setEnergy(const int energy){
     this->energy = energy;
 }


//-------------------GET--------------------------

 QString RecipeEntry:: getName() const{
     return this->name;

 }

 QString RecipeEntry:: getTypeOfDish() const{
     return this->typeOfDish;
 }
 bool RecipeEntry:: getDietary() const{
     return  this->isDietary;
 }
 QTime RecipeEntry:: getTime() const{
     return this->time;
 }
 QString RecipeEntry:: getIngridients() const{
     return this->ingridients;
 }
 QString RecipeEntry:: getRecipe() const{
     return this->recipe;
 }
 int RecipeEntry:: getEnergy() const{
     return  this->energy;
 }


 //----------------OPERATOR-------------------------

 bool RecipeEntry:: operator ==(RecipeEntry &other){
     bool res = 1;

     if (this->name!=other.name) res = 0;
     if (this->typeOfDish!=other.typeOfDish) res = 0;
     if (this->isDietary!= other.isDietary) res = 0;
     if (this->time != other.time) res = 0;
     if (this->ingridients!=other.ingridients) res = 0;
     if (this->recipe!= other.recipe) res = 0;
     if (this->energy!=other.energy) res = 0;

     return  res;

 }

 bool RecipeEntry:: operator !=(RecipeEntry &other){

     return !(this==&other);
 }

 bool RecipeEntry:: operator <(RecipeEntry &other){

     if(this->typeOfDish!=other.typeOfDish) return this->typeOfDish < other.typeOfDish;

     return this->name < other.name;
 }

 bool RecipeEntry:: operator >=(RecipeEntry &other){

     return !(this < &other);
 }

 bool RecipeEntry:: operator >(RecipeEntry &other){

    // bool tmp1 = this->typeOfDish > other.typeOfDish, tmp2 = this->name > other.name;
     if(this->typeOfDish!=other.typeOfDish) return this->typeOfDish > other.typeOfDish;

     return this->name > other.name;
 }

 bool RecipeEntry:: operator <=(RecipeEntry &other){

     return ! (this > &other);
 }

void RecipeEntry:: operator=(RecipeEntry &other){
     this->name = other.name;
     this->isDietary = other.isDietary;
     this->ingridients = other.ingridients;
     this->recipe = other.recipe;
     this->energy = other.energy;
     this->typeOfDish = other.typeOfDish;
     this->time = other.time;
 }
