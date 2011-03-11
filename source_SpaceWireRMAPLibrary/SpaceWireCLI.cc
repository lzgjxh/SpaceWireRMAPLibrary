//spCLI.cpp
// 2007-02-03 Hirokazu Odaka
// 2007-06-11 Hirokazu Odaka addToken
// 2007-07-11 Hirokazu Odaka

/* Renamed from spCLI.cpp */

//#include "spCLI.h"

#include "SpaceWireCLI.hh"
#include <cstdlib>

#include <string.h>

SpaceWireCLI::SpaceWireCLI()
{
}


bool SpaceWireCLI::ask(std::istream& ist, std::string& str, std::ostream& ost, const char* question)
{
  ost << question;
  ost << "[ " << str << " ] ";
  if (qtoken.empty()) {
    getline(ist, line);
    if (line.empty()) return false;
    
    unsigned int i = 0;
    unsigned int token_begin, token_end;
    
    while (i < line.size()) {
      std::string str_token;
      while (line[i]==' ' || line[i]=='\t') {
	i++;
	if (i>=line.size()) return false;
      }
      token_begin = i;
      while (line[i]!=' ' && line[i]!='\t' && i<line.size()) {
	i++;
      }
      token_end = i;
      str_token = line.substr(token_begin, (token_end-token_begin));
      qtoken.push(str_token);
    }
  }
  
  str = qtoken.front();
  qtoken.pop();
  return true;
}


bool SpaceWireCLI::ask_cstr(std::istream& ist, char* cstr, unsigned int size, std::ostream& ost, const char* question)
{
  std::string str = cstr;
  if ( ask(ist, str, ost, question) ) {
    if (str.length() < size) {
      strcpy(cstr, str.c_str());
    }
    else {
      strncpy(cstr, str.c_str(), size-1);
      cstr[size-1] = '\0';
    }

    return true;
  }
  return false;
}


bool SpaceWireCLI::ask_int(std::istream& ist, int& num, std::ostream& ost, const char* question) 
{
  char cstr[80];
  sprintf(cstr, "%d", num);
  std::string str = cstr;
  if ( ask(ist, str, ost, question) ) {
    num = atoi(str.c_str());
    return true;
  }
  return false;
}


bool SpaceWireCLI::ask_hex(std::istream& ist, unsigned int& num, std::ostream& ost, const char* question) 
{
  char cstr[80];
  sprintf(cstr, "%x", num);
  std::string str = cstr;

  if ( ask(ist, str, ost, question) ) {
    unsigned int nDigit = str.length();
    unsigned int tmp_num = 0;
    unsigned int digit = 1;
    try {
      for (unsigned int i = 1; i <= nDigit; i++) {
	tmp_num += digit * charToHex(str[nDigit-i]);
	digit *= 16;
      }
    }
    catch (char* errstr) {
      std::cout << errstr << std::endl;
      return false;
    } 
    num = tmp_num;
    return true;
  }
  return false;
}


bool SpaceWireCLI::ask_double(std::istream& ist, double& num, std::ostream& ost, const char* question) 
{
 char cstr[80];
  sprintf(cstr, "%f", num);
  std::string str = cstr;
  if ( ask(ist, str, ost, question) ) {
    num = atof(str.c_str());
    return true;
  }
  return false;
}


void SpaceWireCLI::ask2(std::istream& ist, std::string& str, std::ostream& ost, const char* question)
{
start:
  ost << question;
  while (qtoken.empty()) {
    getline(ist, line);
    if (line.empty()) goto start;
    
    unsigned int i = 0;
    unsigned int token_begin, token_end;
    
    while (i < line.size()) {
      std::string str_token;
      while (line[i]==' ' || line[i]=='\t') {
	i++;
	if (i>=line.size()) goto start;
      }
      token_begin = i;
      while (line[i]!=' ' && line[i]!='\t' && i<line.size()) {
	i++;
      }
      token_end = i;
      str_token = line.substr(token_begin, (token_end-token_begin));
      qtoken.push(str_token);
    }
  }
  
  str = qtoken.front();
  qtoken.pop();
}


void SpaceWireCLI::ask2_cstr(std::istream& ist, char* cstr, unsigned int size, std::ostream& ost, const char* question)
{
  std::string str = cstr;
  ask2(ist, str, ost, question);
  if (str.length() < size) {
    strcpy(cstr, str.c_str());
  }
  else {
    strncpy(cstr, str.c_str(), size-1);
    cstr[size-1] = '\0';
  }
}


void SpaceWireCLI::ask2_int(std::istream& ist, int& num, std::ostream& ost, const char* question) 
{
  std::string str;
  ask2(ist, str, ost, question);
  num = atoi(str.c_str());
}


void SpaceWireCLI::ask2_hex(std::istream& ist, unsigned int& num, std::ostream& ost, const char* question) 
{
  std::string str;
  unsigned int tmp_num;
  ask2(ist, str, ost, question);
  unsigned int nDigit = str.length();
  tmp_num = 0;
  unsigned int digit = 1;
  try {
    for (unsigned int i = 1; i <= nDigit; i++) {
      tmp_num += digit * charToHex(str[nDigit-i]);
      digit *= 16;
    }
  }
  catch (char* errstr) {
    std::cout << errstr << std::endl; 
  }

  num = tmp_num;
}


void SpaceWireCLI::ask2_double(std::istream& ist, double& num, std::ostream& ost, const char* question) 
{
  std::string str;
  ask2(ist, str, ost, question);
  num = atof(str.c_str());
}


void SpaceWireCLI::addToken(const std::string& str) 
{
  qtoken.push(str);
}


void SpaceWireCLI::addToken(const char* cstr) 
{
  qtoken.push((std::string)cstr);
}



void SpaceWireCLI::clear() 
{
  while(!qtoken.empty()) {
    qtoken.pop();
  }
}


void SpaceWireCLI::load(std::ifstream& fin)
{
  std::string str;
  while (!fin.eof()) {
    fin >> str;
    addToken(str);
  }
}


unsigned int SpaceWireCLI::charToHex(char c) throw (char*)
{
  unsigned int n = 0;
  
  switch (c) {
  case '0':
    n = 0;
    break;
  case '1':
    n = 1;
    break;
  case '2':
    n = 2;
    break;
  case '3':
    n = 3;
    break;
  case '4':
    n = 4;
    break;
  case '5':
    n = 5;
    break;
  case '6':
    n = 6;
    break;
  case '7':
    n = 7;
    break;
  case '8':
    n = 8;
    break;
  case '9':
    n = 9;
    break;
  case 'a':
  case 'A':
    n = 10;
    break;
  case 'b':
  case 'B':
    n = 11;
    break;
  case 'c':
  case 'C':
    n = 12;
    break;
  case 'd':
  case 'D':
    n = 13;
    break;
  case 'e':
  case 'E':
    n = 14;
    break;
  case 'f':
  case 'F':
    n = 15;
    break;
  default:
    throw "Not 0-f";
  }
  return n;
}

