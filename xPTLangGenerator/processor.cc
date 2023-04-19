#include "processor.h"
#include "npdt.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <sys/time.h>

inline double gettime_ms(){
  timespec t;
  clock_gettime(CLOCK_MONOTONIC_RAW, &t);
  return (t.tv_sec + t.tv_nsec*1e-9)*1000;
}
TP::TP( helper * cf ){
  config = cf;
  stateCount          = config->stateno; 
  transitionCount     = config->transno;
  varCount          = config->varno; 
  inputCount          = config->inputno; 
  outputCount         = config->outputno;
  stateList           = (NPDT *)        malloc (stateCount      * sizeof(NPDT));
  transitionList      = (Transition *)  malloc (transitionCount * sizeof(Transition));
  stackMap            = (bool *)        malloc (varCount      * sizeof(bool));
  IdMap               = (uint32_t *)    malloc (varCount      * sizeof(uint32_t));
  Map_ID ( config->bitmap);

/*  set initial state to activated, the rest to not */
  stateList[0].accepting = 0; 
  for (uint32_t i = 1; i < stateCount; i++){
    stateList[i].accepting = 0; 
    stateList[i].epsilonState = false; 
  }
/* initialize I/O stream  */
  input  = (uint32_t **) malloc ( inputCount * sizeof( uint32_t *));

  input_length    = (uint32_t *) malloc ( inputCount * sizeof( uint32_t ));
  output_length   = (uint32_t *) malloc ( outputCount * sizeof( uint32_t ));

  printf("INITIALIZE IO on CPU\n");

#ifdef DEBUG
    output  = (uint32_t *) malloc ( threadCount * sizeof( uint32_t ));
#else
    output  = (uint32_t *) malloc ( threadCount * sizeof( uint32_t ));
#endif
  opt_optID();

}

void TP::opt_optID ( ){
  opcode["EPSILON"] = 0;
  opcode["ADD"]     = 1;
  opcode["ADDI"]    = 2;
  opcode["SUB"]     = 3;
  opcode["SUBI"]    = 4;
  opcode["MUL"]     = 5;
  opcode["MULI"]    = 6;
  opcode["DIV"]     = 7;
  opcode["DIVI"]    = 8; 
  opcode["SET"]     = 9; 
  opcode["POP"]     = 10; 
  opcode["LSHIFT"]  = 11; 
  opcode["LSHIFTI"] = 12; 
  opcode["RSHIFT"]  = 13; 
  opcode["RSHIFTI"] = 14; 
  opcode["OR"]      = 15; 
  opcode["ORI"]     = 16; 
  opcode["AND"]     = 17; 
  opcode["ANDI"]    = 18; 
}

void TP::Map_ID( uint64_t bitmap){
  uint64_t mask = 1; 
  std::cout << "==================MAP ID=========================" << std::endl;

  std::cout << "Stack Map: ";
  for ( uint64_t i = 0 ; i < varCount; i ++){
    stackMap[i] = bitmap & ( mask << i);
    std::cout << stackMap[i] << ",";
  } 
  std::cout << std::endl;

  uint32_t currentVar = 0;
  totalVar = 0;
  uint32_t currentStack = 0;
  totalStack = 0;

  std::cout << "ID  Map: ";
  for ( uint32_t i =0; i < varCount; i++){
    if ( stackMap[i] == 0){
      IdMap[i] = currentVar;
      currentVar++;
    }
    else {
      IdMap[i] = currentStack;
      currentStack++;
    }
    std::cout << IdMap[i] << ",";
  }
  std::cout << std::endl;
  totalVar = currentVar;
  totalStack = currentStack;
  std::cout << "Number of Variable: "<< totalVar << std::endl;
  std::cout << "Number of Stack: "<< totalStack << std::endl;
  std::cout << "==================================================="<<std::endl;
}
