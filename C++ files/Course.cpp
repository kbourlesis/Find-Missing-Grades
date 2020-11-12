#include "Course.h"
#include <string.h>

using namespace std;

//Constructor
Course::Course(const char *title, const char *code, Course *matchedCourse):Title(NULL),Code(NULL){

  int length;

  if (title != NULL){
    length = strlen(title);
    Title = new char[length+1];
    strcpy(Title, title);
  }

  if (code != NULL){
    length = strlen(code);
    Code = new char[length+1];
    strcpy(Code, code);
  }

  matchingCourse = matchedCourse;

}

//Destructor
Course::~Course(){

  if (Title != NULL)
    delete[] Title;
  if (Code != NULL)
    delete[] Code;

}

//Getters
char* Course::getTitle()const{
  return Title;
}

char* Course::getCode()const{
  return Code;
}

Course* Course::getMatchedCourse()const{
  return matchingCourse;
}

//Setters
void Course::setTitle(char *newTitle){

  if (Title != NULL)
    delete[] Title;

  int length = strlen(newTitle);
  Title = new char[length+1];

  strcpy(Title, newTitle);

}

void Course::setCode(char *newCode){

  if (Code != NULL)
    delete[] Code;

  int length = strlen(newCode);
  Code = new char[length+1];

  strcpy(Code, newCode);

}

void Course::setMatchedCourse(Course *matchedCourse){

  matchingCourse = matchedCourse;

}
