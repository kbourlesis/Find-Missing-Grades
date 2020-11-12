#include <iostream>
#include <fstream>
#include <map>
#include <string.h>

#include "Course.h"
#include "Student.h"
#include "OpenFileException.h"
#include "comparator.h"

using namespace std;

void openFiles(char const **, fstream *);
void appendTerminalChar(char *);
void getCourses(fstream &, fstream &, ofstream &, char *, char*, map<char* ,Course*, cmpStr> &);
void getStudents(fstream &, ofstream &, char *, char*, map<char* , Student*, cmpStr> &);
void getGrades(fstream &, ofstream &, char *, char*, map<char* , Course*, cmpStr> &, map<char* , Student*, cmpStr> &, map<char*, map<char*, float, cmpStr>, cmpStr> &);
int checkForErrors(char *);
int checkGradeValue(float);
void replaceComma(char *);
void replaceCarriageReturn(char *);
bool errorReport(ofstream &, const char *, int, char *&, int, int, char* = NULL);
void findMissingGrades(map<char*, Course*, cmpStr> &, map<char*, Student*, cmpStr> &, map<char*, map<char*, float, cmpStr>, cmpStr> &);
void mainReport(ofstream &, Student*, Course*, float);

int main(int argc, char const *argv[]) {

  if (argc != 5) {
    cout << "\nUsage: ./main \"courses.csv\" \"matches.csv\" \"students.csv\" \"grades.csv\"\n" <<endl;
    exit(0);
  }

  ofstream errorReport_FileDesc;
  char *tmpReportingFile = " ", csvLine[256];

  fstream fileDesc_array[4];

  try{
    openFiles(argv, fileDesc_array);
  }catch(const OpenFileException &e){
      cout << e.getMsg() <<endl<<endl;
      exit(0);
  }

  map<char*, Course*, cmpStr> Courses;
  getCourses(fileDesc_array[0], fileDesc_array[1], errorReport_FileDesc, tmpReportingFile, csvLine, Courses);

  map<char*, Student*, cmpStr> Students;
  getStudents(fileDesc_array[2], errorReport_FileDesc, tmpReportingFile, csvLine, Students);

  map<char*, map<char*, float, cmpStr>, cmpStr> studentsGrades;
  getGrades(fileDesc_array[3], errorReport_FileDesc, tmpReportingFile, csvLine, Courses, Students, studentsGrades);

  findMissingGrades(Courses, Students, studentsGrades);

  for(auto iterator = Students.begin(); iterator != Students.end(); iterator++){

        delete iterator->first;
        delete iterator->second;
        Students.erase(iterator);

  }

  for(auto iterator = Courses.begin(); iterator != Courses.end(); iterator++){

        delete iterator->first;
        delete iterator->second;
        Courses.erase(iterator);

  }

  for (auto iterator = studentsGrades.begin(); iterator != studentsGrades.end(); iterator++){
    delete[] iterator->first;
    map<char*, float, cmpStr> &internalMap = iterator->second;
    for (auto internalIterator = internalMap.begin(); internalIterator != internalMap.end(); internalIterator++){
      delete[] internalIterator->first;
      internalMap.erase(internalIterator);
    }
    studentsGrades.erase(iterator);
  }

  if (errorReport_FileDesc.is_open())
    errorReport_FileDesc.close();

  for (int i = 0; i < 3; i++)
    fileDesc_array[i].close();

  return 0;
}


void openFiles(char const **fileNames, fstream *fileDescriptors){

  for (int i = 1; i < 5; i++){

    fileDescriptors[i-1].open(fileNames[i], ios::in);

    if (!fileDescriptors[i-1].is_open())
      throw OpenFileException("\nFailed to open ", fileNames[i]);

  }

}


void appendTerminalChar(char *strToken){

  if (strToken != NULL) {

    int strLength = strlen(strToken);

    char *tmpStr = new char[strLength+1];
    strcpy(tmpStr, strToken);
    tmpStr[strLength-1] = '\0';

    strToken = tmpStr;

  }

}


void getCourses(fstream &coursesFileDesc, fstream &matchesFileDesc, ofstream &errorReport_FileDesc, char *tmpReportingFile, char *csvLine, map<char* ,Course*, cmpStr> &Courses){

  char *title, *code_Token;
  const char *reportingFileName = "Courses Report\0";
  int colNum, rowNum = 1, error_emptyLine = 4, error_Type;
  bool approveTitle, approveCode;

  coursesFileDesc.getline(csvLine, 256);

  while (coursesFileDesc.getline(csvLine, 256)) {

    colNum = 1;
    rowNum++;

    if ( (strcmp(csvLine,"\0") == 0) ||  (strcmp(csvLine,"\r") == 0) || (strcmp(csvLine,"\n") == 0) ) {
      errorReport(errorReport_FileDesc, reportingFileName, error_emptyLine, tmpReportingFile, rowNum, colNum);
      continue;
    }

    replaceCarriageReturn(csvLine);

    approveTitle = true;
    approveCode = true;

    code_Token = strtok(csvLine,";");
    appendTerminalChar(code_Token);
    error_Type = checkForErrors(code_Token);

    if (error_Type != 0)
      approveCode = errorReport(errorReport_FileDesc, reportingFileName, error_Type, tmpReportingFile, rowNum, colNum);

    colNum++;

    title = strtok(NULL,";");
    appendTerminalChar(title);
    error_Type = checkForErrors(title);

    if (error_Type != 0)
      approveTitle = errorReport(errorReport_FileDesc, reportingFileName, error_Type, tmpReportingFile, rowNum, colNum);

    if (approveTitle && approveCode) {
      Course *course = new Course(title,code_Token);

      char *code = new char[strlen(code_Token) + 1];
      strcpy(code, code_Token);

      Courses[code] = course;
    }

  }

  char *oldCurriculum, *newCurriculum;
  bool approveOldCurriculum, approveNewCurriculum;
  const char *reportingFile = "Matches Report\0";

  rowNum = 0;

  while (matchesFileDesc.getline(csvLine, 256)) {

    colNum = 1;
    rowNum++;

    if ( (strcmp(csvLine,"\0") == 0) ||  (strcmp(csvLine,"\r") == 0) || (strcmp(csvLine,"\n") == 0) ) {
      errorReport(errorReport_FileDesc, reportingFile, error_emptyLine, tmpReportingFile, rowNum, colNum);
      continue;
    }

    replaceCarriageReturn(csvLine);

    approveOldCurriculum = true;
    approveNewCurriculum = true;

    newCurriculum = strtok(csvLine,";");
    appendTerminalChar(newCurriculum);
    error_Type = checkForErrors(newCurriculum);

    if (error_Type != 0)
      approveNewCurriculum = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);

    colNum++;

    oldCurriculum = strtok(NULL,";");
    appendTerminalChar(oldCurriculum);
    error_Type = checkForErrors(oldCurriculum);

    if (error_Type != 0)
      approveOldCurriculum = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);

    if (approveOldCurriculum && approveNewCurriculum) {

      if ( (Courses.find(newCurriculum) != Courses.end()) && (Courses.find(oldCurriculum) != Courses.end()) )
        Courses[oldCurriculum]->setMatchedCourse(Courses[newCurriculum]);
        //cout << Courses[oldCurriculum]->getCode() << " -> " << Courses[oldCurriculum]->getMatchedCourse()->getCode() <<endl;
        //Courses[newCurriculum]->setMatchedCourse(Courses[oldCurriculum]);
        //cout << Courses[newCurriculum]->getCode() << " -> " << Courses[newCurriculum]->getMatchedCourse()->getCode() <<endl;
    }
  }
  //Mathimata poy den exoyn antistoixisi
  // for(auto &it: Courses){
  //   if (it.second->getMatchedCourse() == NULL) {
  //     cout << it.second->getCode() << " " << it.second->getTitle() <<endl;
  //   }
  // }
}


void getStudents(fstream &studentsFileDesc, ofstream &errorReport_FileDesc, char *tmpReportingFile, char *csvLine, map<char* , Student*, cmpStr> &Students){

  char *AM_Token, *Name, *LName;
  const char *reportingFile = "Students Report\0";
  int colNum, rowNum = 1, error_Type, error_emptyLine = 4;
  bool approveAM, approveName, approveLName;

  studentsFileDesc.getline(csvLine, 256);

  while (studentsFileDesc.getline(csvLine, 256)) {

    colNum = 1;
    rowNum++;

    if ( (strcmp(csvLine,"\0") == 0) ||  (strcmp(csvLine,"\r") == 0) || (strcmp(csvLine,"\n") == 0) ) {
      errorReport(errorReport_FileDesc, reportingFile, error_emptyLine, tmpReportingFile, rowNum, colNum);
      continue;
    }

    replaceCarriageReturn(csvLine);

    approveAM = true;
    approveName = true;
    approveLName = true;

    AM_Token = strtok(csvLine,";");
    appendTerminalChar(AM_Token);
    error_Type = checkForErrors(AM_Token);

    if (error_Type != 0)
      approveAM = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);

    LName = strtok(NULL,";");
    appendTerminalChar(LName);
    error_Type = checkForErrors(Name);

    if (error_Type != 0)
      approveName = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);

    Name = strtok(NULL,";");
    appendTerminalChar(Name);
    error_Type = checkForErrors(LName);

    if (error_Type != 0)
      approveLName = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);

    if (approveAM && approveName && approveLName) {

      Student *student = new Student(AM_Token,Name,LName);

      char* am = new char[strlen(AM_Token) + 1];
      strcpy(am, AM_Token);

      Students[am] = student;
    }

  }

}


void getGrades(fstream &gradesFileDesc, ofstream &errorReport_FileDesc, char *tmpReportingFile, char *csvLine, map<char* , Course*, cmpStr> &Courses, map<char* , Student*, cmpStr> &Students, map<char*, map<char*, float, cmpStr>, cmpStr> &studentsGrades){

  char *AM_Token, *courseCode_Token, *grade;
  const char *reportingFile = "Grades Report\0";
  int colNum, rowNum = 1, error_emptyLine = 4, error_Type;
  float gradeValue;
  bool approveAM, approveCourseCode, approveGrade;

  gradesFileDesc.getline(csvLine, 256);

  while (gradesFileDesc.getline(csvLine, 256)) {

    colNum = 1;
    rowNum++;

    if ( (strcmp(csvLine,"\0") == 0) ||  (strcmp(csvLine,"\r") == 0) || (strcmp(csvLine,"\n") == 0) ) {
      errorReport(errorReport_FileDesc, reportingFile, error_emptyLine, tmpReportingFile, rowNum, colNum);
      continue;
    }

    replaceCarriageReturn(csvLine);

    approveAM = true;
    approveCourseCode = true;
    approveGrade = true;

    AM_Token = strtok(csvLine,";");
    appendTerminalChar(AM_Token);
    error_Type = checkForErrors(AM_Token);

    if (error_Type != 0)
      approveAM = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);
    else{
      if(Students.find(AM_Token) == Students.end()){
        error_Type = 6;
        approveAM = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum, AM_Token);
      }
    }

    colNum++;

    courseCode_Token = strtok(NULL,";");
    appendTerminalChar(courseCode_Token);
    error_Type = checkForErrors(courseCode_Token);

    if (error_Type != 0)
      approveCourseCode = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);
    else{
      if(Courses.find(courseCode_Token) == Courses.end()){
        error_Type = 5;
        approveAM = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum, courseCode_Token);
      }
    }

    colNum++;

    grade = strtok(NULL,";");
    appendTerminalChar(grade);
    error_Type = checkForErrors(grade);

    if (error_Type != 0)
      approveGrade = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);
    else{

      replaceComma(grade);

      gradeValue = atof(grade);
      error_Type = checkGradeValue(gradeValue);

      if (error_Type != 0)
        approveGrade = errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum);
    }

    if (approveAM && approveCourseCode && approveGrade) {

      char *am = new char[strlen(AM_Token)+1];
      strcpy(am,AM_Token);

      char *courseCode = new char[strlen(courseCode_Token)+1];
      strcpy(courseCode,courseCode_Token);

      if (studentsGrades[am].find(courseCode) != studentsGrades[am].end()) {

        if ( ((studentsGrades[am][courseCode] >= 5) && (gradeValue >= 5)) && (studentsGrades[am][courseCode] != gradeValue) ) {
          error_Type = 7;
          errorReport(errorReport_FileDesc, reportingFile, error_Type, tmpReportingFile, rowNum, colNum, am);
        }

        if (studentsGrades[am][courseCode] < gradeValue)
          studentsGrades[am][courseCode] = gradeValue;

      }else
        studentsGrades[am][courseCode] = gradeValue;

    }

  }

}


int checkForErrors(char *strToken){

  if ( (strToken == NULL) || (strcmp(strToken," ") == 0) || (strcmp(strToken,"\r") == 0) )
    return 1;

  int tokenLength = strlen(strToken);
  if ( (strToken[0] == ' ') || (tokenLength > 1 && strToken[tokenLength-1] == ' '))
    return 2;

  return 0;

}


int checkGradeValue(float grade){

  if ((grade < 0) || (grade > 10))
    return 3;

  return 0;

}


bool errorReport(ofstream &errorReport_FileDesc, const char *reportingFile, int error_Type, char *&tmpReportingFile, int rowNum, int colNum, char *AM_courseCode_Token){

  if (!errorReport_FileDesc.is_open()) {

    errorReport_FileDesc.open("error report.txt", ios::out);

    if (!errorReport_FileDesc.is_open()) {
      cout << "couldn't open error file." <<endl;
      exit(0);
    }
  }

  if (strcmp(reportingFile, tmpReportingFile) != 0) {
    tmpReportingFile = (char *)reportingFile;

    int strLength = strlen(tmpReportingFile);
    int length = 70;

    for (int i = 0; i < length; i++) {

      if (i+1 == length) {
        errorReport_FileDesc << "*" <<endl;
      }else
        errorReport_FileDesc << "*";

    }

    for (int i = 0; i < (length - strLength - 2); i++) {
      if (i == 0)
        errorReport_FileDesc << "*";

      if ( i == (length - strLength - 2)/2)
        errorReport_FileDesc << tmpReportingFile << " ";
      else
        errorReport_FileDesc << " ";

      if (i+1 == (length - strLength - 2))
        errorReport_FileDesc << "*" <<endl;
    }

    for (int i = 0; i < length; i++) {

      if (i+1 == length) {
        errorReport_FileDesc << "*" <<endl;
      }else
        errorReport_FileDesc << "*";

    }
  }

  if (error_Type == 1)
    errorReport_FileDesc << "Empty field -> row: " << rowNum << "." <<endl;

  if (error_Type == 2)
    errorReport_FileDesc << "Space detected -> col: " << colNum << ", row: " << rowNum << "." <<endl;

  if (error_Type == 3)
    errorReport_FileDesc << "Invalid grade -> row: " << rowNum << "." <<endl;

  if (error_Type == 4)
    errorReport_FileDesc << "Empty line -> row: " << rowNum << "." <<endl;

  if (error_Type == 5)
    errorReport_FileDesc << "There is no course with code \"" << AM_courseCode_Token << "\" as in row " << rowNum << " of csv file." <<endl;

  if (error_Type == 6)
    errorReport_FileDesc << "There is no student with AM \"" << AM_courseCode_Token << "\" as in row " << rowNum << " of csv file." <<endl;

  if (error_Type == 7)
    errorReport_FileDesc << "Student \"" << AM_courseCode_Token << "\" has two promotable grades." <<endl;

  return false;

}


void replaceComma(char *grade){

  for (int i = 0; i < (int)strlen(grade); i++)
    if (grade[i] == ',')
      grade[i] = '.';

}


void replaceCarriageReturn(char *csvLine){

  int linelength = strlen(csvLine);

  if (csvLine != NULL)
    if (csvLine[linelength-1] == '\r' || csvLine[linelength-1] == '\n')
      csvLine[linelength-1] = '\0';

}

void findMissingGrades(map<char*, Course*, cmpStr> &Courses, map<char*, Student*, cmpStr> &Students, map<char*, map<char*, float, cmpStr>, cmpStr> &studentsGrades){

  char *am;
  ofstream mainReport_FileDesc;

  // for (auto it = studentsGrades.begin(); it != studentsGrades.end(); ++it) {
  //   cout << "AM: " << it->first <<endl;
  //   map<char*, float, cmpStr> &internal = it->second;
  //   for (auto it2 = internal.begin(); it2 != internal.end(); ++it2) {
  //     cout << "Course code: " << it2->first << " Grade: " << it2->second << endl;
  //   }
  //   //break;
  // }

  for (auto iterator = studentsGrades.begin(); iterator != studentsGrades.end(); iterator++) {
    //cout << "AM: " << iterator->first <<endl;
    am = iterator->first;
    map<char*, float, cmpStr> &internalMap = iterator->second;
    for (auto internalIterator = internalMap.begin(); internalIterator != internalMap.end(); internalIterator++) {
      //cout << "Course code: " << internalIterator->first << " Grade: " << internalIterator->second << endl;
      char *courseCode = internalIterator->first;
      float oldCourseGrade = internalIterator->second;
      Course *matchedCourse = Courses[courseCode]->getMatchedCourse();
      if(matchedCourse != NULL){
        //cout << "am: " << am << " " << "course code " << matchedCourse->getCode() <<endl;
        float newCourseGrade = studentsGrades[am][matchedCourse->getCode()];
        if (newCourseGrade != oldCourseGrade) {
          //cout << Students[am]->getAM() << " " << Courses[courseCode]->getMatchedCourse()->getCode() << ": " << newCourseGrade << ", " << Courses[courseCode]->getCode() << ": " << oldCourseGrade <<endl;
          mainReport(mainReport_FileDesc, Students[am], Courses[courseCode], oldCourseGrade);
        }
      }
    }

  }

  if (mainReport_FileDesc.is_open())
  mainReport_FileDesc.close();
  // for(auto &student: students){
  //   for(map<char*, float, cmpStr>::iterator it = student.second->getCoursesGrades().begin(); it != student.second->getCoursesGrades().end(); ++it){
  //     cout << it->first <<endl;
  //     // if (courses[it->first] == courses.end()) {
  //     //   cout <<"den uparxei"<<endl;
  //     // }
  //     Course *matchedCourse = courses[it->first]->getMatchedCourse();
  //     if ( matchedCourse != NULL) {
  //         float newCourseGrade = student.second->getCoursesGrades()[matchedCourse->getCode()];
  //         float oldCourseGrade = it->second;
  //         if (newCourseGrade != oldCourseGrade) {
  //           mainReport(mainReport_FileDesc, student.second, courses[it->first], oldCourseGrade);
  //         }
  //     }
  //   }
  // }

// map<char*, map<char*, double, cmpStr>,cmpStr> matching;
//
//   for(auto &student: students){
//     am = student.first;
//     for (int i = 0; i < (int)grades.size(); i++) {
//       if (strcmp(am,grades[i]->getAM()) == 0) {
//         //gradeList.push_back(grades[i]);
//         matching[am][grades[i]->getCourseCode()] = grades[i]->getGrade();
//       }
//     }
//     break;
//   }
//
//   for (auto it = matching.begin(); it != matching.end(); ++it) {
//     cout << "Student AM: " << it->first << endl;
//     map<char*, Grade*, cmpStr> &internal = it->second;
//     cout << "CourseCode: " << internal.first << " Grade: " << internal.second->getGrade() << endl;
//   }

// Grade g("dasf","adff","fgre");
// v.push_back(&g);
// vector<Grade*> *vptr;
// vptr = &v;
//
// cout << (*vptr)[0]->getAM() << " " << (*vptr)[0]->getCourseCode() << " " <<endl;

  // for(auto &student: students){
  //   am = student.first;
  //   vector<Grade*> gradeList;
  //   for (int i = 0; i < (int)grades.size(); i++) {
  //     if (strcmp(am,grades[i]->getAM()) == 0) {
  //       gradeList.push_back(grades[i]);
  //     }
  //   }
  //   student.second->setGradeList(&gradeList);
  //   //break;
  // }
  //
  // for(Grade *gr: students[am]->getGradeList()[0]){
  //   cout << gr->getCourseCode() << " " << gr->getGrade() <<endl;
  // }

  // for (int i = 0; i < (int)student[am].getGradeList().size(); i++) {
  //   cout << student[am].getGradeList()[i]->getAM() << " " << student[am].getGradeList()[i]->getGrade() <<endl;
  // }

  // for(Grade *grade: grades){
  //   courseCode = grade->getCourseCode();
  //   am = grade->getAM();
  //   // cout << "am: " << am << " " << "coursecode: " << courseCode <<endl;
  //     if (courses[courseCode]->getMatchedCourse() != NULL) {
  //       //cout << students[am]->getAM()<<endl;
  //        cout << courses[courseCode]->getCode() << " matched course: " << courses[courseCode]->getMatchedCourse()->getCode()<<endl<<endl;
  //     }
  //
  // }

  // map<char*, map<char*, Course*, comparator>, comparator> mymap;
  // for (auto it = mymap.begin(); it != mymap.end(); ++it) {
  //   cout << "code1: " << it->first; << endl;
  //   map<char*, Course*, comparator> &internal = it->second;
  //   cout << "code2: " << it->first << " title: " << it->second << endl;
  // }
}

void mainReport(ofstream &mainReport_FileDesc, Student *student, Course *oldCourse, float oldCourseGrade){

  if (!mainReport_FileDesc.is_open()) {

    mainReport_FileDesc.open("report.csv", ios::out);
    mainReport_FileDesc << "A.M." << ";" << "Last Name" << ";" << "First Name" << ";" << "Course Code Old Curric." << ";" << "Course Title Old Curric." << ";" << "Course Code New Curric." << ";" << "Course Title New Curric." << ";" << "Missing Grade" <<endl;

    if (!mainReport_FileDesc.is_open()) {
      cout << "couldn't open error file." <<endl;
      exit(0);
    }
  }

  mainReport_FileDesc << student->getAM() << ";" << student->getLName() << ";" << student->getName() << ";" << oldCourse->getCode() << ";" << oldCourse->getTitle() << ";" << oldCourse->getMatchedCourse()->getCode() << ";" << oldCourse->getMatchedCourse()->getTitle() << ";" << oldCourseGrade <<endl;

}
