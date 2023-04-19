#include "function_state.h"
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <cstring>

int main ( int argc, char *argv[]){
  
  uint32_t size_32b = 0;
  float size_gb = 0.01;
 
  std::cout << "Reading: " << argc << " arggument" << std::endl;
  if ( argc != 3) {
    std::cout << " need 2 argument ( config file, code base mode) " << std::endl;
    exit(0);
  }
  std::string configFile  = std::string(argv[1]);
  uint32_t c_mode = atoi( argv[2]);
  size_32b = (uint32_t) ( (size_gb * 1e9 )/ 4);
  std::cout  << "input Size: : " << size_gb<<"GB or " << size_32b<< " element of 32bit"  <<std::endl;
  std::cout << "Config File: "<< configFile <<std::endl; 
  std::cout <<" Setting up argument" << std::endl;
  ARGUMENT  * argm = new ARGUMENT  ( configFile, size_32b);
  std::cerr << " Input Size : " << size_gb <<" GB" <<std::endl; 
  TP * processor  = new TP( argm->config);
  printf("LOAD FST TO CPU\n");
  processor->loadFST_CPU();
  CODE_BASE_FST * CBF = new CODE_BASE_FST( processor, c_mode);
  //CBF->export_to_intermediate_form();
  std::cout <<"Generated Code Mode : " << CBF->c_mode_s<< std::endl;
  std::cout <<"Generate Code-based Kernel at " << CBF->baseline_file << std::endl;
  if ( c_mode ==0){
    CBF->baseline_gen();
  }
  else if (c_mode ==1){
    CBF->calculate_merge_function();
    CBF->connect_merge_function();
    CBF->to_DOT(processor->config->dotFile + "merge.dot");
    CBF->merge_function_gen();
    CBF->print_mergeFunction_info();
    CBF->write_report();
  }
  else if (c_mode ==2){
    CBF->calculate_merge_function();
    CBF->connect_merge_function();
    CBF->to_DOT(processor->config->dotFile + "merge.dot");
    CBF->cpu_function_gen();
//    CBF->print_mergeFunction_info();
    CBF->write_report();
  }
  std::cout <<" Application Finishes" << std::endl;
//return ;
}
