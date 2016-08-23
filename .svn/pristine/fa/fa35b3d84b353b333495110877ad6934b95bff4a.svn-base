/* 
* A simple OCCI test application 
* This file contains the Employees class declaration 
*/ 

#include <occi.h> 
#include <iostream> 
#include <iomanip> 

using namespace oracle::occi; 
using namespace std; 

class Employees { 
public: 
  Employees(); 
  virtual ~Employees(); 

  void List(); 

private: 
  Environment *env; 
  Connection  *con; 

  string user; 
  string passwd; 
  string db; 
};
