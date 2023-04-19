#pragma once
#include "processor.h"
#define MIN_LENGTH 10
#define DEBUG_GPU 1
#undef DEBUG_GPU
#define STACK_DEPTH 16 
enum COMPARECODE {  ALPHABET      = 0,
                      NEGALPHABET = 1,
                      EPS         = 2,
                      VAR         = 3,
                      INPT        = 4,
                      PASSTHROUGH = 5,
                      NEGVAR      = 6,
                      NEGINPT     = 7,
                      ILL         = 8
                  };

class FSTGPU {
  public:
// IO stream
  
  uint32_t inputCount;
  uint32_t **input;
  uint32_t * input_length;
  uint32_t outputCount;
  uint32_t ** output;

// IO partition Information
  uint32_t number_of_block;
  uint32_t number_of_thread;
  uint32_t total_threads;

  // temp varaible to simulate writing 
  uint32_t ** simulated_output;
  uint32_t * printed_output;

  // how many symbol each thread have to process
  uint32_t ** partition_input_length;
  uint32_t ** partition_input_length_cpu;
  // where each thread process from
  uint32_t ** partition_input_base;
  uint32_t ** partition_input_base_cpu;
  // how many symbol each thread have processed
  uint32_t ** partition_input_current;
  uint32_t ** partition_input_current_cpu;
  // where each thread write to 
  uint32_t ** partition_output_base;
  uint32_t ** partition_output_base_cpu;
  // how many symbol have each thread written 
  uint32_t ** partition_output_current;
  uint32_t ** partition_output_current_cpu;
  // core function
  void IO_setup( TP * cpu_transducer, uint32_t blocks, uint32_t threads);
  void IO_partition ( TP* cpu_transducer, int test); 

  // code-based kernel
  void process_rle();
  void process_rle_inline();
  void rle_baseline_host();
  void rld_baseline_host();
  void bpe_baseline_host();
  void bpd_baseline_host();
  void gve_baseline_host();
  void gvd_baseline_host();
  void coo_csr_baseline_host();
  void dense_csr_baseline_host();
  void csv_enc_baseline_host();
  void hist_baseline_host();
  void csv_raw_baseline_host();

  // merge-function kernel
  void rld_function_group_host();
  void rle_function_group_host();
  void bpd_function_group_host();
  void bpe_function_group_host();
  void gvd_function_group_host();
  void gve_function_group_host();
  void coo_csr_function_group_host();
  void dense_csr_function_group_host();
  void csv_enc_function_group_host();
  void hist_function_group_host();
  void csv_raw_function_group_host();

  // parition scheme
  void partition (  TP * cpu_transducer, int test); 
  void RLE_ENCODING( TP * cpu_transducer);
  void CSV_PARSING( TP * cpu_transducer);
  void RLE_DECODING( TP * cpu_transducer);
  void BP_ENCODING( TP * cpu_transducer);
  void COOCSR( TP * cpu_transducer);

  // helper function  
  void save_thread_config( TP * cpu_transducer, std::string destination);
  void action_convert( uint32_t opcode, uint32_t src1, uint32_t src2, uint32_t dst);
  void transition_convert(uint32_t alphabet);
};
