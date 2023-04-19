#include "processor.h"
#define MIN_LENGTH 10

void TP::IO_partition (uint32_t  thread, int test){
  instances = thread;
  std::cout << "=========================================="<< std::endl;
  switch(test){
    case RLE_ENCODE:
    case RLE_ENCODE_NO_STACK:
    case RLE_ENCODE_OPT:
      std::cout << " Partition Run Length Encoding" <<std::endl;
      RL_ENCODING();
      break;
    case RLE_DECODE:
    case RLE_DECODE_OPT:
      std::cout << " Partition Run Length Decoding" <<std::endl;
      RL_DECODING();
      break;
    case BP_ENCODE:
    case BP_ENCODE_NO_STACK:
      std::cout << " Partition Bit Packing Encoding" <<std::endl;
      BP_ENCODING();
      break;
    case BP_DECODE:
    case BP_DECODE_NO_STACK:
      RL_ENCODING();
      std::cout << " Partition Bit Packing Decoding" <<std::endl;
      break;
    case GV_ENCODE:
      RL_ENCODING();
      std::cout << " Partition Variable Length Encoding" <<std::endl;
      break;
    case GV_DECODE:
      RL_ENCODING();
      std::cout << " Partition Variable Length Decoding" <<std::endl;
      break;
    case DENSE_DOK:
      break;
    case DOK_LIL:
      break;
    case  LIL_COO:
      break;
    case  COO_CSR:
      std::cout << " Partition COO-CSR" <<std::endl;
      COOCSR();
      break;
    case DENSE_CSR:
      std::cout << " Partition Dense-CSR" <<std::endl;
      break;
    case DENSE_CSR_PTR:
      std::cout << " Partition Dense-CSR" <<std::endl;
      COOCSR();
      break;
    case CSR_DENSE:
      break;
    case CSV_ENC_DET:
    case CSV_ENC_INDET:
      std::cout << " Partition CSV Encoded" <<std::endl;
      CSV_PARSING();
      break;
    case HISTOGRAM:
      std::cout << " Partition HISTOGRAM" <<std::endl;
      RL_ENCODING();
      break;
    default:
      break;
  }
  print_partition_info();
  std::cout << "=========================================="<< std::endl;
}

// RLE ENCODING
void TP::RL_ENCODING(){
  std::cout << "==================================== "<< std::endl;
  std::cout << " START PARTITION INPUT FOR RLE"<< std::endl;
  std::cout << " Input has  " << input_length[0] <<" symbols"<< std::endl;
  uint32_t input_per_thread = input_length[0] / instances;

  if ( input_per_thread < MIN_LENGTH) input_per_thread = MIN_LENGTH;
  if ( input_length[0]  < MIN_LENGTH) input_per_thread = input_length[0];

  std::cout << " each thread process " << input_per_thread <<" symbols"<< std::endl;
  uint32_t max_thread = input_length[0] / input_per_thread;
  uint32_t left_over  = input_length[0] % input_per_thread;
  std::cout << " max thread " << max_thread <<" threads"<< std::endl;
  std::cout << " left over " << left_over <<" symbols"<< std::endl;

  partition_input_length_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  partition_input_base_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  output_cpu = (uint32_t **) malloc ( instances * sizeof ( uint32_t*));

  for ( uint32_t i = 0; i < instances; i++){
    partition_input_length_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    partition_input_base_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    output_cpu[i] = (uint32_t *) malloc ( outputCount * sizeof ( uint32_t));
    for (uint32_t j = 0; j < inputCount; j++){
      //std::cout << "thread : " << i << ", input: " << j << std::endl;
      if ( i < max_thread){
        partition_input_length_cpu[i][j] = input_per_thread;
      }
      else if ( i == max_thread){
        partition_input_length_cpu[i][j] = left_over;
      }
      else {
        partition_input_length_cpu[i][j] =0;
      }
      partition_input_base_cpu[i][j] = i * input_per_thread;
    }
  }
  std::cout << "==================================== "<< std::endl;
}

void TP::RL_DECODING(){
  std::cout << "==================================== "<< std::endl;
  std::cout << " START PARTITION INPUT FOR BP"<< std::endl;
  std::cout << " Input has  " << input_length[0] <<" symbols"<< std::endl;
  uint32_t input_per_thread = input_length[0] / instances;

  if ( input_per_thread < MIN_LENGTH) input_per_thread = MIN_LENGTH;
  if ((input_per_thread %2) != 0) input_per_thread = input_per_thread + (4 -(input_per_thread %4));
  if ( input_length[0]  < MIN_LENGTH) input_per_thread = input_length[0];

  std::cout << " each thread process " << input_per_thread <<" symbols"<< std::endl;
  uint32_t max_thread = input_length[0] / input_per_thread;
  uint32_t left_over  = input_length[0] % input_per_thread;
  left_over =0;
  std::cout << " max thread " << max_thread <<" threads"<< std::endl;
  std::cout << " left over " << left_over <<" symbols"<< std::endl;

  partition_input_length_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  partition_input_base_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  output_cpu = (uint32_t **) malloc ( instances * sizeof ( uint32_t*));

  for ( uint32_t i = 0; i < instances; i++){
    partition_input_length_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    partition_input_base_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    output_cpu[i] = (uint32_t *) malloc ( outputCount * sizeof ( uint32_t));
    for (uint32_t j = 0; j < inputCount; j++){
      //std::cout << "thread : " << i << ", input: " << j << std::endl;
      if ( i < max_thread){
        partition_input_length_cpu[i][j] = input_per_thread;
      }
      else if ( i == max_thread){
        partition_input_length_cpu[i][j] = left_over;
      }
      else {
        partition_input_length_cpu[i][j] =0;
      }
      partition_input_base_cpu[i][j] = i * input_per_thread;
    }
  }
  std::cout << "==================================== "<< std::endl;
}
// BP ENCODING
void TP::BP_ENCODING(){
  std::cout << "==================================== "<< std::endl;
  std::cout << " START PARTITION INPUT FOR BP"<< std::endl;
  std::cout << " Input has  " << input_length[0] <<" symbols"<< std::endl;
  uint32_t input_per_thread = input_length[0] / instances;

  if ( input_per_thread < MIN_LENGTH) input_per_thread = MIN_LENGTH;
  if ((input_per_thread %4) != 0) input_per_thread = input_per_thread + (4 -(input_per_thread %4));
  if ( input_length[0]  < MIN_LENGTH) input_per_thread = input_length[0];

  std::cout << " each thread process " << input_per_thread <<" symbols"<< std::endl;
  uint32_t max_thread = input_length[0] / input_per_thread;
  uint32_t left_over  = input_length[0] % input_per_thread;
  left_over =0;
  std::cout << " max thread " << max_thread <<" threads"<< std::endl;
  std::cout << " left over " << left_over <<" symbols"<< std::endl;

  partition_input_length_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  partition_input_base_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  output_cpu = (uint32_t **) malloc ( instances * sizeof ( uint32_t*));

  for ( uint32_t i = 0; i < instances; i++){
    partition_input_length_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    partition_input_base_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    output_cpu[i] = (uint32_t *) malloc ( outputCount * sizeof ( uint32_t));
    for (uint32_t j = 0; j < inputCount; j++){
      //std::cout << "thread : " << i << ", input: " << j << std::endl;
      if ( i < max_thread){
        partition_input_length_cpu[i][j] = input_per_thread;
      }
      else if ( i == max_thread){
        partition_input_length_cpu[i][j] = left_over;
      }
      else {
        partition_input_length_cpu[i][j] =0;
      }
      partition_input_base_cpu[i][j] = i * input_per_thread;
    }
  }
  std::cout << "==================================== "<< std::endl;
}
void TP::print_partition_info(){

  for ( uint32_t i = 0; i < instances; i++){
    std::cout << "CPU Thread : " << i << std::endl;
    for (uint32_t j = 0; j < inputCount; j++){
      std::cout <<"\tInput: "<< j << ", "<< partition_input_base_cpu[i][j];
      std::cout <<"--"<< partition_input_length_cpu[i][j] << std::endl;
    }
  }
}




void TP::CSV_PARSING(){
  std::cout << "==================================== "<< std::endl;
  std::cout << " START PARTITION INPUT FOR RLE"<< std::endl;
  std::cout << " Input has  " << input_length[0] <<" symbols"<< std::endl;
  uint32_t input_per_thread = input_length[0] / instances;

  if ( input_per_thread < MIN_LENGTH) input_per_thread = MIN_LENGTH;
  if ( input_length[0]  < MIN_LENGTH) input_per_thread = input_length[0];

  std::cout << " each thread process " << input_per_thread <<" symbols"<< std::endl;
  uint32_t max_thread = input_length[0] / input_per_thread;
  uint32_t left_over  = input_length[0] % input_per_thread;
  std::cout << " max thread " << max_thread <<" threads"<< std::endl;
  std::cout << " left over " << left_over <<" symbols"<< std::endl;

  partition_input_length_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  partition_input_base_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  output_cpu = (uint32_t **) malloc ( instances * sizeof ( uint32_t*));

  for ( uint32_t i = 0; i < instances; i++){
    partition_input_length_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    partition_input_base_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    output_cpu[i] = (uint32_t *) malloc ( outputCount * sizeof ( uint32_t));
    for (uint32_t j = 0; j < inputCount; j++){
      //std::cout << "thread : " << i << ", input: " << j << std::endl;
      if ( i < max_thread){
        partition_input_length_cpu[i][j] = input_per_thread;
      }
      else if ( i == max_thread){
        partition_input_length_cpu[i][j] = left_over;
      }
      else {
        partition_input_length_cpu[i][j] =0;
      }
      partition_input_base_cpu[i][j] = 0;
    }
  }
  std::cout << "==================================== "<< std::endl;
}


void TP::COOCSR(){
  std::cout << "==================================== "<< std::endl;
  std::cout << " START PARTITION INPUT FOR MATRIX"<< std::endl;
  std::cout << " Input has  " << input_length[0] <<" symbols"<< std::endl;
  uint32_t number_of_data = input[0][2];
  uint32_t number_of_row = input[0][0];
  uint32_t number_of_col = input[0][1];
  std::cout << " Number of data: " << number_of_data <<std::endl;
  std::cout << " Number of row: " << number_of_row <<std::endl;
  std::cout << " Number of col: " << number_of_col <<std::endl;


  uint32_t input_per_thread = input_length[1] / instances;

  if ( input_per_thread < MIN_LENGTH) input_per_thread = MIN_LENGTH;
  if ( input_length[1]  < MIN_LENGTH) input_per_thread = input_length[1];

  std::cout << " each thread process " << input_per_thread <<" symbols"<< std::endl;
  uint32_t max_thread = input_length[1] / input_per_thread;
  uint32_t left_over  = input_length[1] % input_per_thread;
  left_over = 0;
  std::cout << " max thread " << max_thread <<" threads"<< std::endl;
  std::cout << " left over " << left_over <<" symbols"<< std::endl;

  partition_input_length_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  partition_input_base_cpu = (uint32_t **) malloc ( instances * sizeof(uint32_t *));
  output_cpu = (uint32_t **) malloc ( instances * sizeof ( uint32_t*));

  for ( uint32_t i = 0; i < instances; i++){
    partition_input_length_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    partition_input_base_cpu[i] = (uint32_t *) malloc ( inputCount * sizeof(uint32_t));
    output_cpu[i] = (uint32_t *) malloc ( outputCount * sizeof ( uint32_t));

    for (uint32_t j = 0; j < inputCount; j++){
      //std::cout << "thread : " << i << ", input: " << j << std::endl;
      if ( j == 0){
        partition_input_length_cpu[i][j] =3;
        partition_input_base_cpu[i][j] = 0;
      }
      else{
        if ( i < max_thread){
          partition_input_length_cpu[i][j] = input_per_thread;
        }
        else if ( i == max_thread){
          partition_input_length_cpu[i][j] = left_over;
        }
        else {
          partition_input_length_cpu[i][j] =0;
        }
        partition_input_base_cpu[i][j] = 0;
      }
    }
  }
  std::cout << "==================================== "<< std::endl;
}















