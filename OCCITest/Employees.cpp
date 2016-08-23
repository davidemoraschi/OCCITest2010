/* 
* A simple OCCI test application 
* This file contains the Employees class implementation 
*/ 

#include "Employees.h" 

using namespace std; 
using namespace oracle::occi; 

int main (void) 
{ 
  /* 
   * create an instance of the Employees class, 
   * invoke the List member, delete the instance, 
   * and prompt to continue... 
   */ 

  Employees *pEmployees = new Employees(); 

  pEmployees->List(); 

  delete pEmployees; 

  cout << "ENTER to continue..."; 

  cin.get(); 

  return 0; 
} 
Employees::Employees() 
{ 
  /* 
   * connect to the test database as the HR 
   * sample user and use the EZCONNECT method 
   * of specifying the connect string. Be sure 
   * to adjust for your environment! The format 
   * of the string is host:port/service_name 
   */ 

  user = "hr"; 
  passwd = "hr"; 
  db = "oel01:1521/OEL11GR1.SAND"; 

  env = Environment::createEnvironment(Environment::DEFAULT); 

  try 
  { 
    con = env->createConnection(user, passwd, db); 
  } 
  catch (SQLException& ex) 
  { 
    cout << ex.getMessage(); 

    exit(EXIT_FAILURE); 
  } 
} 

Employees::~Employees() 
{ 
  env->terminateConnection (con); 

  Environment::terminateEnvironment (env); 
} 

void Employees::List() 
{ 
  /* 
   * simple test method to select data from 
   * the employees table and display the results 
   */ 

  Statement *stmt = NULL; 
  ResultSet *rs = NULL; 
  string sql = "select employee_id, first_name, last_name from employees order by last_name, first_name"; 

  try 
  { 
    stmt = con->createStatement(sql); 
  } 
  catch (SQLException& ex) 
  { 
    cout << ex.getMessage(); 
  } 

  if (stmt) 
  { 
    try 
    { 
      stmt->setPrefetchRowCount(32); 

      rs = stmt->executeQuery(); 
    } 
    catch (SQLException& ex) 
    { 
      cout << ex.getMessage(); 
    } 

    if (rs) 
    { 
      cout << endl << setw(8) << left << "ID" 
           << setw(22) << left << "FIRST NAME" 
           << setw(27) << left << "LAST NAME" 
           << endl; 
      cout << setw(8) << left << "======" 
           << setw(22) << left << "====================" 
           << setw(27) << left << "=========================" 
           << endl; 

      while (rs->next()) { 
        cout << setw(8) << left << rs->getString(1) 
             << setw(22) << left << (rs->isNull(2) ? "n/a" : rs->getString(2)) 
             << setw(27) << left << rs->getString(3) 
             << endl; 
      } 

      cout << endl; 

      stmt->closeResultSet(rs); 
    } 

    con->terminateStatement(stmt); 
  } 
}