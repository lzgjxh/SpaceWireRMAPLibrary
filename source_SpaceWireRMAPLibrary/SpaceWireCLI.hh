//spCLI.h
// 2007-02-03 Hirokazu Odaka
// 2007-06-11 Hirokazu Odaka addToken
// 2007-07-11 Hirokazu Odaka

/* Renamed from spCLI.h */

#ifndef spCLI_H
#define spCLI_H

#include <iostream>
#include <fstream>
#include <queue>
#include <string>


class SpaceWireCLI
{
private:
  std::queue<std::string> qtoken;
  std::string line;

public:
  SpaceWireCLI();
  bool ask(std::istream& ist, std::string& str, std::ostream& ost, const char* question);
  bool ask_cstr(std::istream& ist, char* cstr, unsigned int size, std::ostream& ost, const char* question);
  bool ask_int(std::istream& ist, int& num, std::ostream& ost, const char* question);
  bool ask_hex(std::istream& ist, unsigned int& num, std::ostream& ost, const char* question);
  bool ask_double(std::istream& ist, double& num, std::ostream& ost, const char* question);

  void ask2(std::istream& ist, std::string& str, std::ostream& ost, const char* question);
  void ask2_cstr(std::istream& ist, char* cstr, unsigned int size, std::ostream& ost, const char* question);
  void ask2_int(std::istream& ist, int& num, std::ostream& ost, const char* question);
  void ask2_hex(std::istream& ist, unsigned int& num, std::ostream& ost, const char* question);
  void ask2_double(std::istream& ist, double& num, std::ostream& ost, const char* question);

  void addToken(const std::string& str);
  void addToken(const char* cstr);

  void load(std::ifstream& fin);
  
  void clear();

 private: 
  unsigned int charToHex(char c) throw (char*);
};

#endif//spCLI_H
