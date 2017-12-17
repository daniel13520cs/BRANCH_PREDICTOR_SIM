//
//  main.cpp
//  branchPridictor_SIM
//
//  Created by chihchienhsiao on 12/16/17.
//  Copyright © 2017 chihchienhsiao. All rights reserved.
//

#include <iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <stdlib.h>     /* strtol */
#include <math.h>       /* pow */



using namespace std;
#define MemSize 65536
u_long test(bitset<32> in, int start, int end);

//branch instructions
class INSMem
{
public:
  bitset<32> Instruction;
  vector<unsigned int> IMem;
  vector<bool> res;
  
  INSMem()
  {
    IMem.resize(MemSize);
    res.resize(MemSize);
    ifstream imem;
    string line;
    int i=0;
    imem.open("trace.txt");
    if (imem.is_open())
    {
      while (getline(imem,line))
      {
        //convert branch instructions from hex string to int value
        string hexString = (line.substr(0,8)); //modified line
        char* pEnd;
        const char * hexNums = hexString.c_str();
        IMem[i] = (unsigned int) strtol (hexNums,&pEnd,16);
        //std:cout << IMem[i] << "\n";
        
        //the result of branch instructions (Taken or not Taken)
        res[i] = (line.substr(9) == "1");
        //cout << res[i] << "\n";
        i++;
      }
    }
    else cout<<"Unable to open file\n";
    imem.close();
    
  }
  
};

//2-bit correlating counter
/*class Counter(){
{
public:
  
private:
  vector<int > local;
  vector<int > global;
  
 }*/



class Counter{
public:
  int local;
  int global;
  int corelattingState;
  vector<vector<int>> predictState;
  
  int m = 12;
  int k = 2;
  Counter(){
    local = pow(2, m);
    global = pow(2, k);
    cout << "g = " << global;
    predictState.resize(local);
    //initialize size of counters
    for (int i = 0; i < local; ++i){
      predictState[i].resize(global);
    }
    //initialize state of counters to b11
    for (int i = 0; i < local; ++i){
     for (int j = 0; j < global; ++j){
       predictState[i][j] = 3;
     }
    }
    cout << "out of loop";
  }
  
  //return true if predict taken, false otherwise
  bool predict(int state){
    if(state == 0 || state == 1){
      //cout<<  "N" << "\n";
      return false;
    }
    if(state == 2 || state == 3){
      //cout<< "T" << "\n";
      return true;
    }
    return (state >= 2);
  }
  
private:

};

int main(int argc, const char * argv[]) {
  // insert code here...
  std::cout << "Hello, World!\n";
  INSMem myInsMem;
  Counter myCounter;
  int m = 12;
  int k = 2;
  
  int i = 0;
  int lsb = pow(2,m);
  int col = myCounter.corelattingState;
  while( myInsMem.IMem[i] != 0 ){
    int rowIdx = myInsMem.IMem[i] % lsb;
    int colIdx = myCounter.corelattingState;
    //predict correctly if pre res == branch operation res
    if(myCounter.predict(myCounter.predictState[rowIdx][colIdx]) == myInsMem.res[i]){
      cout << "predict correctly\n";
    } else {
      cout << "wrong\n";
    }
    i++;
  }
  
  
  
  
  return 0;
}

u_long test(bitset<32> in, int start, int end)
{
  u_long res = 0;
  for (int i=start; i>=end; i--){
    res = (res << 1) + in.test(i);
  }
  return res;
}


