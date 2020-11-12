#include <iostream>

#ifndef CoursesHeader_H
#define CoursesHeader_H

using namespace std;

class Course{

  char *Title, *Code;
  Course *matchingCourse;

public:

  //Constructor
  Course(const char * = NULL, const char * = NULL, Course * = NULL);

  //Destructor
  ~Course();

  //Getters
  char* getTitle()const;
  char* getCode()const;
  Course* getMatchedCourse()const;

  //Setters
  void setTitle(char *);
  void setCode(char *);
  void setMatchedCourse(Course *);

};

#endif
