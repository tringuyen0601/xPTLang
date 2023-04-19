#include  "processor.h"
//#include "gpu_fst_engine.h"
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <cstring>

int main ( int argc, char *argv[]){

  uint32_t threads= 0;
  uint32_t blocks = 0;
  uint32_t size_32b = 0;
  float size_gb = 0;
  uint32_t mode = 0;
 
  std::cout << "Reading: " << argc << " arggument" << std::endl;
  if ( argc != 4) {
    std::cout << " need 4 argument ( config file, number_of_thread, size in GB( accepting floating point)), only get " << argc<< " argument"<< std::endl;
  exit(0);
  }
  std::string configFile  = std::string(argv[1]);
  size_gb = atof( argv[3]);
  size_32b = (uint32_t) ( (size_gb * 1e9 )/ 4);
  threads = atoi( argv[2]);
  std::cout  << "thread: : " << threads<<std::endl;
  std::cout  << "input Size: : " << size_gb<<"GB or " << size_32b<< " element of 32bit"  <<std::endl;
  std::cout << "Config File: "<< configFile <<std::endl; 
  std::cout <<" Setting up argument" << std::endl;
  ARGUMENT  * argm = new ARGUMENT  ( configFile, size_32b);
  std::cerr << " Input Size : " << size_gb <<" GB" <<std::endl; 
  TP * processor  = new TP( argm->config);
  printf("LOAD FST TO CPU\n");
  processor->loadFST_CPU();
  processor->IO_allocation(argm->benchmark_int);
  processor->IO_partition(threads, argm->benchmark_int);


  std::cout <<"================================" << std::endl;
  std::cout <<"START CPU EXECUTION" << std::endl;
  switch(processor->config->benchmark){
    case RLE_ENCODE_OPT:
      processor->rle_cpu_host();
      break;
    case RLE_DECODE_OPT:
      processor->rld_cpu_host();
      break;
    case BP_DECODE:
      processor->bpd_cpu_host();
      break;
    case BP_ENCODE:
      processor->bpe_cpu_host();
      break;
    case GV_DECODE:
      processor->gvd_cpu_host();
      break;
    case GV_ENCODE:
      processor->gve_cpu_host();
      break;
    case COO_CSR:
      processor->coo_csr_cpu_host();
      break;
    case DENSE_CSR_PTR:
      processor->dense_csr_cpu_host();
      break;
    case CSV_ENC_DET:
      processor->csv_enc_cpu_host();
      break;
    case HISTOGRAM:
      processor->hist_cpu_host();
      break;
    case CSV_ENC_INDET:
      processor->csv_raw_cpu_host();
      break;
  }

  std::cout <<"CPU Function Finished" << std::endl;
  std::cout <<" Application Finishes" << std::endl;
//return ;
}
