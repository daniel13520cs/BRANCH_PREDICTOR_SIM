//
//  main.cpp
//  branchPridictor_SIM
//
//  Created by chihchienhsiao on 12/16/17.
//  Copyright © 2017 chihchienhsiao. All rights reserved.
//

#include <iostream>
#include <sstream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <stdlib.h>     /* strtol */
#include <math.h>       /* pow */

using namespace std;
#define MemSize 65536

//branch instructions
class INSMem
{
public:
  bitset<32> Instruction;
  vector<unsigned int> IMem;
  vector<bool> res;
  
  INSMem(const char* fileName)
  {
    IMem.resize(MemSize);
    res.resize(MemSize);
    ifstream imem;
    string line;
    int i=0;
    imem.open(fileName);
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

class Counter{
public:
  int local;
  int global;
  int corelattingState = 3;
  vector<vector<int> > predictState;
  int m;
  int k;
  Counter(const char* fileName){
      //get the value of m and k
      ifstream mk;
      string line;
      //mk.open("config_new.txt");
      mk.open(fileName);
      if (mk.is_open())
      {
        //get m
        mk >> m;
        //get k
        mk >> k;
      } else {
        cout << "Unable to open the file\n";
      }
    corelattingState = pow(2,k) - 1;
    
      local = pow(2, m);
    //cout << "local=" << local;
      global = pow(2, k);
      //cout << "g = " << global;
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
  Counter myCounter(argv[0]); // "config_new.txt"
  INSMem myInsMem(argv[1]); // "trace.txt"
  int m = myCounter.m;
  int k = myCounter.k;
  int i = 0;
  int lsb = pow(2,m);
  
  double numOfMiss = 0;
  
  unsigned int rowIdx = myInsMem.IMem[i] % lsb;
  unsigned int colIdx = myCounter.corelattingState;
  ofstream traceOut;
  //erase the last result
  traceOut.open("trace.txt.out",std::ofstream::out | std::ofstream::trunc);
  if (traceOut.is_open()){
    while( myInsMem.IMem[i] != 0 ){
      
      //miss rate
      if(myCounter.predict(myCounter.predictState[rowIdx][colIdx]) != myInsMem.res[i]){
        numOfMiss++;
      }
      
      //output my prediction result
      if(myCounter.predict(myCounter.predictState[rowIdx][colIdx])){
        traceOut<< "1"<<endl;
        //cout << "line "<<i+1<<" 1\n";
      } else {
        traceOut<< "0"<<endl;
        //cout << "line "<<i+1<<" 0\n";
      }
      
      //cout << rowIdx << "\t" << colIdx << "\t" << myCounter.predictState[rowIdx][colIdx] << endl;
      
      //update local counter states
      if(myInsMem.res[i] & (myCounter.predictState[rowIdx][colIdx] !=3)){
        //cout << "pass";
        if(myCounter.predictState[rowIdx][colIdx] == 1){
          myCounter.predictState[rowIdx][colIdx] +=2;
        } else {
          myCounter.predictState[rowIdx][colIdx] ++;
        }
      } else if( (!myInsMem.res[i]) & (myCounter.predictState[rowIdx][colIdx] !=0)){
        if(myCounter.predictState[rowIdx][colIdx] == 2){
          myCounter.predictState[rowIdx][colIdx] -=2;
        } else {
          myCounter.predictState[rowIdx][colIdx] --;
        }
      
      }
    
      //update global counter states
      if (k>0){
        if(myInsMem.res[i]){
          colIdx = (colIdx / 2) + ( pow(2,(k-1)) );
          
        } else {
          colIdx = colIdx / 2;
        }
      }
      //cout << "colIdx = " << colIdx << "\n";
      
      //update local counter states
      rowIdx = myInsMem.IMem[++i] % lsb;
      
    }
    
  } else {
    //cout<<"Unable to open file";
    traceOut.close();
  }
  
  //debug
  for (int i = 0; i < pow(2,m); ++i){
   for (int j = 0; j < pow(2,k); ++j){
     //cout << "predictStateResult = " << myCounter.predictState[i][j] <<"\n";
   }
  }
  /*
  cout << "numOfMiss" << numOfMiss;
  cout << "i" << i;
  cout << "miss rate =" << (numOfMiss /i);
  */
  //cout << "end";
  return 0;
}



