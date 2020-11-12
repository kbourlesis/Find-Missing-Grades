#include <iostream>

#ifndef StudentsHeader_H
#define StudentsHeader_H

using namespace std;

class Student{

  char *am, *Name, *LName;

public:

  //Constructor
  Student(char *am = NULL, char *Name = NULL, char *LName = NULL);

  //Destructor
  ~Student();

  //Getters
  char* getAM()const;
  char* getName()const;
  char* getLName()const;

  //Setters
  void setAM(char *);
  void setName(char *);
  void setLName(char *);

};

#endif
