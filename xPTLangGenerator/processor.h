#pragma once
#include <fstream>
#include <assert.h>
#include <iostream>
#include <map>
#include "npdt.h"
//#include "helper.h"
//#include "monitor.h"
#include  "argument.h"
#define DEBUG 1
#undef DEBUG
#define CPU_DEBUG 1
#undef CPU_DEBUG
#define CPU_STACK_DEPTH 16
enum MODE { NO_MEASURE  = 0,
            PAPI        = 1,
            TIME        = 2
          };
class stack {
  public:
  uint32_t * base;
  uint32_t * top;
  uint32_t size  = 0;
};

class TP {   
public:
  std::map<std::string, uint32_t> opcode;
  /* Full transducer information  */
  helper * config;
  double start_time, end_time;
  
  uint32_t stateCount;
  uint32_t transitionCount;
  uint32_t varCount;
  uint32_t totalVar;
  uint32_t totalStack;
  uint32_t inputCount;
  uint32_t outputCount;

  /* stack and memory info  */  
  uint32_t * stackList;  
  bool * stackMap;
  uint32_t * IdMap;
  NPDT * stateList;
  Transition * transitionList; 


  /* input and output Tape  */ 
  std::ifstream *  TextInput;
  std::ofstream *  TextOutput;
  uint32_t ** input;
  uint32_t * input_length;
  uint32_t * output;
  uint32_t * output_length;
 
  uint32_t threadCount; 
  uint32_t instances; /*CPU thread */
//context Information
  // how many symbol each thread have to process
  uint32_t ** partition_input_length_cpu;
  // where each thread process from
  uint32_t ** partition_input_base_cpu;
  uint32_t ** output_cpu;



  TP (helper * cf);
  void IO_allocation ( int test);
  void IO_partition ( uint32_t thread, int test);
  void IO_partition_rle ();
  void IO_allocation_1to1 ();
  void IO_allocation_coo_csr_dense_csr ();
  void opt_optID ();
  void loadFST_CPU();
  void RL_ENCODING();
  void RL_DECODING();
  void BP_ENCODING();
  void CSV_PARSING();
  void COOCSR();

  /* cpu function */
  void rle_cpu_host();
  void rld_cpu_host();
  void bpe_cpu_host();
  void bpd_cpu_host();
  void gve_cpu_host();
  void gvd_cpu_host();
  void coo_csr_cpu_host();
  void dense_csr_cpu_host();
  void hist_cpu_host();
  void csv_enc_cpu_host();
  void csv_raw_cpu_host();


  /* state function */
  void readInput(); 
  void OutputFetch(); 
  void OutputDump(); 

  /* setting up     */
  void Map_ID( uint64_t bitmap);
  void loadRegex ( std::string filename);
  void readState ( std::string filename, int verbose = 0);
  void readTransition ( std::string filename, int verbose = 0);
  void reorderTransition();
  void to_DOT ( std::string destination);
  std::string action_convert ( uint32_t opcode, uint32_t src1, uint32_t src2, uint32_t dst, bool log = false);
  std::string transition_convert ( uint32_t alphabet, bool log = false);
  void printTransitionTable();
  void printStateTable();
  void printTape ( std::string destination, uint32_t * start, uint32_t * end);
  std::string compare_code_convert ( uint32_t code);


  /* helper function  */
  void getTape ( uint32_t * start, uint32_t * end);
  void printCurrentInput( int threadID);
  void printOutput();
  void printMemInfo();
  void print_partition_info();
};

