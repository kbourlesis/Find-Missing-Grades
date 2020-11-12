#include "OpenFileException.h"
#include <string.h>


using namespace std;

//Constructor
OpenFileException::OpenFileException(char* msg, const char* fileName){

  if (msg != NULL && fileName != NULL) {
    this->msg = new char[strlen(msg) + strlen(fileName) + 3];
    strcpy(this->msg, msg);
    strcat(this->msg, "\"");
    strcat(this->msg, fileName);
    strcat(this->msg, "\"");
  }

}

//Destructor
OpenFileException::~OpenFileException(){

  if (msg != NULL)
    delete[] msg;

}

//Getter
char* OpenFileException::getMsg() const {
  return msg;
}

//Setter
void OpenFileException::setMsg(char *newMsg){

  if (msg != NULL)
    delete[] msg;

  int length = strlen(newMsg);
  msg = new char[length];

  strcpy(msg,newMsg);

}
