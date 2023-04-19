//#include  "processor.h"
#include "gpu_fst_engine.h"
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
  if ( argc != 6) {
    std::cout << " need 6 argument ( config file, number_of_block, number_of_threads, size in GB( accepting floating point), execution mode ), only get " << argc<< " argument"<< std::endl;
 	std::cout << " Mode 0: Function Call" << std::endl;
 	std::cout << " Mode 1: Inline" << std::endl;
  exit(0);
  }
  std::string configFile  = std::string(argv[1]);
  mode = atoi( argv[5]);
  size_gb = atof( argv[4]);
  size_32b = (uint32_t) ( (size_gb * 1e9 )/ 4);
  threads = atoi( argv[3]);
  blocks = atoi( argv[2]);
  std::cout  << "block: : " << blocks<<std::endl;
  std::cout  << "thread: : " << threads<<std::endl;
  std::cout  << "input Size: : " << size_gb<<"GB or " << size_32b<< " element of 32bit"  <<std::endl;
  std::cout << "Config File: "<< configFile <<std::endl; 
  std::cout <<" Setting up argument" << std::endl;
  ARGUMENT  * argm = new ARGUMENT  ( configFile, size_32b);
  std::cerr << " Input Size : " << size_gb <<" GB" <<std::endl; 
  FSTGPU * FST_ENGINE = (FSTGPU *)malloc(sizeof( FSTGPU));
  TP * processor  = new TP( argm->config);
  printf("LOAD FST TO CPU\n");
  processor->loadFST_CPU();
  processor->IO_allocation(argm->benchmark_int);
  FST_ENGINE->IO_setup( processor, blocks, threads);
  FST_ENGINE->IO_partition (processor,  argm->benchmark_int);
  if ( mode == 0){
    std::cout <<"Baseline Codebase Function" << std::endl;
    switch(processor->config->benchmark){
      case RLE_ENCODE_OPT:
        FST_ENGINE->rle_baseline_host();
        break;
      case RLE_DECODE_OPT:
        FST_ENGINE->rld_baseline_host();
        break;
      case BP_DECODE:
        FST_ENGINE->bpd_baseline_host();
        break;
      case BP_ENCODE:
        FST_ENGINE->bpe_baseline_host();
        break;
      case GV_DECODE:
        FST_ENGINE->gvd_baseline_host();
        break;
      case GV_ENCODE:
        FST_ENGINE->gve_baseline_host();
        break;
      case COO_CSR:
        FST_ENGINE->coo_csr_baseline_host();
        break;
      case DENSE_CSR_PTR:
        FST_ENGINE->dense_csr_baseline_host();
        break;
      case CSV_ENC_DET:
        FST_ENGINE->csv_enc_baseline_host();
        break;
      case HISTOGRAM:
        FST_ENGINE->hist_baseline_host();
        break;
      case CSV_ENC_INDET:
        FST_ENGINE->csv_raw_baseline_host();
        break;
    }
    std::cout <<"Finish Baseline Codebase Function" << std::endl;
  }
  else if ( mode == 1){
    std::cout <<"Merge Function" << std::endl;
    switch(processor->config->benchmark){
      case RLE_ENCODE_OPT:
        FST_ENGINE->rle_function_group_host();
        break;
      case RLE_DECODE_OPT:
        FST_ENGINE->rld_function_group_host();
        break;
      case BP_DECODE:
        FST_ENGINE->bpd_function_group_host();
        break;
      case BP_ENCODE:
        FST_ENGINE->bpe_function_group_host();
        break;
      case GV_DECODE:
        FST_ENGINE->gvd_function_group_host();
        break;
      case GV_ENCODE:
        FST_ENGINE->gve_function_group_host();
        break;
      case COO_CSR:
        FST_ENGINE->coo_csr_function_group_host();
        break;
      case DENSE_CSR_PTR:
        FST_ENGINE->dense_csr_function_group_host();
        break;
      case CSV_ENC_DET:
        FST_ENGINE->csv_enc_function_group_host();
        break;
      case HISTOGRAM:
        FST_ENGINE->hist_function_group_host();
        break;
      case CSV_ENC_INDET:
        FST_ENGINE->csv_raw_function_group_host();
        break;
    }
    std::cout <<"Merge Function Finished" << std::endl;
  }
  std::cout <<" Application Finishes" << std::endl;
//return ;
}
