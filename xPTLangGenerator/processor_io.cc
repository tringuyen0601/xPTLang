#include "processor.h"

void TP::IO_allocation (int test){
  std::cout << "=========================================="<< std::endl;
  switch(test){
    case RLE_ENCODE:
    case RLE_ENCODE_NO_STACK:
    case RLE_ENCODE_OPT:
      std::cout << " Allocate and Partition Run Length Encoding" <<std::endl;
      IO_allocation_1to1();
      break;
    case RLE_DECODE:
    case RLE_DECODE_OPT:
      std::cout << " Allocate and Partition Run Length Decoding" <<std::endl;
      IO_allocation_1to1();
      break;
    case BP_ENCODE:
    case BP_ENCODE_NO_STACK:
      std::cout << " Allocate and Partition Bit Packing Encoding" <<std::endl;
      IO_allocation_1to1();
      break;
    case BP_DECODE:
    case BP_DECODE_NO_STACK:
      std::cout << " Allocate and Partition Bit Packing Decoding" <<std::endl;
      IO_allocation_1to1();
      break;
    case GV_ENCODE:
      std::cout << " Allocate and Partition Variable Length Encoding" <<std::endl;
      IO_allocation_1to1();
      break;
    case GV_DECODE:
      std::cout << " Allocate and Partition Variable Length Decoding" <<std::endl;
      IO_allocation_1to1();
      break;
    case DENSE_DOK:
      break;
    case DOK_LIL:
      break;
    case  LIL_COO:
      break;
    case  COO_CSR:
      std::cout << " Allocate and Partition COO-CSR" <<std::endl;
      IO_allocation_coo_csr_dense_csr ();
      break;
    case DENSE_CSR:
      std::cout << " Allocate and Partition Dense-CSR" <<std::endl;
      IO_allocation_coo_csr_dense_csr ();
      break;
    case DENSE_CSR_PTR:
      std::cout << " Allocate and Partition Dense-CSR" <<std::endl;
      IO_allocation_coo_csr_dense_csr ();
      break;
    case CSR_DENSE:
      break;
    case CSV_ENC_DET:
    case CSV_ENC_INDET:
      std::cout << " Allocate and Partition CSV Encoded" <<std::endl;
      IO_allocation_1to1();
      break;
    case HISTOGRAM:
      std::cout << " Allocate and Partition HISTOGRAM" <<std::endl;
      IO_allocation_1to1();
      break;
    default:
      break;
  }
  std::cout << "=========================================="<< std::endl;
}

void TP::IO_allocation_1to1(){
  int j = 0;
  FILE * inputFile = fopen( config->inputFile[j].c_str(), "r");
  fseek( inputFile, 0, SEEK_END);
  uint32_t filesize  = ftell ( inputFile)/4;
  uint32_t duplicate = config->size_32b / filesize;
  if ( config->size_32b ==0) {
    std::cout << " Enter size 0, keep original size" << std::endl;
    duplicate = 1;
  }
  if ( duplicate ==0) duplicate = 1;
  input_length[j] = duplicate * filesize;
  printf("Input %d has %d element\n", j,filesize );
  printf("duplicate %d times to be %d bytes \n", duplicate,input_length[j]*4 );

  uint32_t *  filecontent  = (uint32_t *) malloc ( (filesize) * sizeof( uint32_t ));
  fseek( inputFile, 0, SEEK_SET);
  fread ( filecontent, filesize,4, inputFile);
  fclose(inputFile);

  printf("finished reading file\n");
  input[j] = (uint32_t *) malloc ( (input_length[j]+1) * sizeof( uint32_t ));
  memset( input[j], 0,  (input_length[j] + 1) * sizeof(uint32_t));
  printf("finished setting up\n");
  for ( uint32_t  k = 0; k < duplicate; k++){
  memcpy( &input[j][k * filesize], filecontent, filesize* sizeof(uint32_t));
  }
}

void TP::IO_allocation_coo_csr_dense_csr (){
  FILE * inputFile = fopen( config->inputFile[0].c_str(), "r");
  
  // metadata  
  fseek( inputFile, 0, SEEK_END);
  printf("reading meta data: %s\n",  config->inputFile[0].c_str());
  input_length[0] = ftell ( inputFile)/4;
  printf("Meta data has %u element\n", input_length[0] );
  input[0] = (uint32_t *) malloc ( (input_length[0]+1) * sizeof( uint32_t ));
  fseek( inputFile, 0, SEEK_SET);    fread ( input[0], input_length[0],4, inputFile);
  fclose(inputFile);

  // data
  inputFile = fopen( config->inputFile[1].c_str(), "r");
  fseek( inputFile, 0, SEEK_END);
  printf("Reading Row index | data value: %s\n",  config->inputFile[1].c_str());
  uint32_t filesize  = ftell ( inputFile)/4;
  uint32_t duplicate = config->size_32b / filesize;
  if ( config->size_32b ==0) {
    std::cout << " Enter size 0, keep original size" << std::endl;
    duplicate = 1;
  }
  if ( duplicate ==0) duplicate = 1;
  input_length[1] = duplicate * filesize;
  printf("Row has %d element\n",filesize );
  printf("duplicate %d times to be %d bytes \n", duplicate,input_length[1]*4 );

  uint32_t *  filecontent  = (uint32_t *) malloc ( (filesize) * sizeof( uint32_t ));
  fseek( inputFile, 0, SEEK_SET);
  fread ( filecontent, filesize,4, inputFile);    fclose(inputFile);

  printf("finished reading row\n");
  input[1] = (uint32_t *) malloc ( (input_length[1]+1) * sizeof( uint32_t ));
  memset( input[1], 0,  (input_length[1] + 1) * sizeof(uint32_t));

  printf("finished setting up row\n");    
  for ( uint32_t k = 0; k < duplicate; k++){
    memcpy( &input[1][k * filesize], filecontent, filesize* sizeof(uint32_t));
  }
#ifdef PRINT_IO
    printf("Print input 0 of %u element\n", input_length[0]);
    uint32_t printsize = input_length[0];    
    if (printsize > 10) printsize = 10;
    for ( uint32_t k  = 0; k < printsize; k++){
      printf("%u; ", input[0][k]);
    }
      printf("\n");
    printf("Print input 1 of %u element\n", input_length[1]);
    printsize = input_length[1];
    if (printsize > 10) printsize = 10;
    for ( uint32_t k  = 0; k < printsize; k++){
      printf("%u; ", input[1][k]);
    }
      printf("\n");
#endif
    printf("Finishing input \n");
    printf("Initialize %d output\n", outputCount);


}

