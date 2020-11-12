#include "Student.h"
#include <string.h>

using namespace std;

//Constructor
Student::Student(char *am, char *name, char *lname):am(NULL),Name(NULL),LName(NULL){

  int length;

  if (am != NULL) {
    length = strlen(am);
    this->am = new char[length];
    strcpy(this->am,am);
  }

  if (name != NULL) {
    length = strlen(name);
    Name = new char[length];
    strcpy(Name,name);
  }

  if (lname != NULL) {
    length = strlen(lname);
    LName = new char[length];
    strcpy(LName,lname);
  }

}

//Destructor
Student::~Student(){
  if (this->am != NULL)
    delete[] am;
  if (Name != NULL)
    delete[] Name;
  if (LName != NULL)
    delete[] LName;

}

//Getters
char* Student::getAM()const{
  return am;
}

char* Student::getName()const{
    return Name;
}

char* Student::getLName()const{
  return LName;
}

//Setters
void Student::setAM(char *am){

  if (this->am != NULL)
    delete[] this->am;

  int length = strlen(am);
  this->am = new char[length+1];

  strcpy(this->am,am);

}

void Student::setName(char *newName){

  if (Name != NULL)
    delete[] Name;

  int length = strlen(newName);
  Name = new char[length];

  strcpy(Name,newName);

}

void Student::setLName(char *newLName){

  if (LName != NULL)
    delete[] LName;

  int length = strlen(newLName);
  LName = new char[length];

  strcpy(LName,newLName);

}
