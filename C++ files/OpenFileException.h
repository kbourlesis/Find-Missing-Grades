#include <iostream>

#ifndef OpenFileException_H
#define OpenFileException_H

using namespace std;

class OpenFileException {

  char* msg;

public:

  //Constructor
  OpenFileException(char* msg, const char* fileName);

  //Destructor
  ~OpenFileException();

  //Getter
  char* getMsg() const;

  //Setter
  void setMsg(char *);

};

#endif
