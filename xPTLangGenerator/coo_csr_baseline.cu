#include "gpu_fst_engine.h"
typedef struct Return_message{
	uint32_t var_0;
	uint32_t var_1;
	uint32_t nextState;
} Return_message;

__global__
void coo_csr_baseline_global( uint32_t ** input_base, uint32_t ** input_length, uint32_t ** input,
						uint32_t **output_base, uint32_t ** output, uint32_t * test_output){
	uint32_t globalID = blockIdx.x * blockDim.x + threadIdx.x;
	uint32_t startStack_shared = threadIdx.x * 0;
	uint32_t base_0 = input_base[globalID][0];
	uint32_t length_0 = input_length[globalID][0];
	uint32_t processed_0 = 0;
	uint32_t base_1 = input_base[globalID][1];
	uint32_t length_1 = input_length[globalID][1];
	uint32_t processed_1 = 0;
	uint32_t currentState = 0;
	uint32_t var_0 =  0;
	uint32_t var_1 =  0;
	uint32_t var_2 =  0;
	uint32_t var_3 =  0;
	uint32_t var_4 =  0;
	uint32_t printed = 0;
	bool done = false;
	uint32_t cycle =0;
	Return_message message;
#ifdef DEBUG
	while ( cycle < 20){
		printf("=============%d=============\n", cycle);
		printf("Current State : %d\n", currentState);
		printf("Var[0]: %d\n", var_0);
		printf("Var[1]: %d\n", var_1);
		printf("Var[2]: %d\n", var_2);
		printf("Var[3]: %d\n", var_3);
		printf("Var[4]: %d\n", var_4);
		printf("Processed Input 0: %d/%d\n", processed_0, length_0);
		printf("Current Input: ");
		for ( uint32_t i =0 ; i < 20; i++){
			printf("%d ", input[0][base_0+i]);
}
		printf("\n");
		printf("Processed Input 1: %d/%d\n", processed_1, length_1);
		printf("Current Input: ");
		for ( uint32_t i =0 ; i < 20; i++){
			printf("%d ", input[1][base_1+i]);
}
		printf("\n");
		printf("Current Output: ");
		for ( uint32_t i =0 ; i < printed; i++){
			printf("%d ", test_output[i]);
		}
		printf("\n");
		cycle++;
#else
	while (  (!done) && ( currentState != 22)){
#endif
		if(currentState == 0){
				var_1=input[0][base_0 + processed_0];
			currentState = 1;
			processed_0++;
		}
		else if (currentState ==1){
#ifdef DEBUG
			test_output[printed] = var_1;
			printed++;
#else 
			output[globalID][0] = var_1;
#endif
			currentState = 2;
		}
		else if (currentState ==2){
			var_1 = var_1 & 0;
			currentState = 3;
		}
		else if (currentState ==3){
				var_0=input[0][base_0 + processed_0];
			currentState = 4;
			processed_0++;
		}
		else if (currentState ==4){
#ifdef DEBUG
			test_output[printed] = var_0;
			printed++;
#else 
			output[globalID][0] = var_0;
#endif
			currentState = 5;
		}
		else if (currentState ==5){
				var_3=input[0][base_0 + processed_0];
			currentState = 6;
			processed_0++;
		}
		else if (currentState ==6){
#ifdef DEBUG
			test_output[printed] = var_3;
			printed++;
#else 
			output[globalID][0] = var_3;
#endif
			currentState = 7;
		}
		else if (currentState ==7){
			var_2 = var_2 & 0;
			currentState = 8;
		}
		else if (currentState ==8){
#ifdef DEBUG
			test_output[printed] = var_1;
			printed++;
#else 
			output[globalID][0] = var_1;
#endif
			currentState = 9;
		}
		else if (currentState ==9){
				var_4=input[1][base_1 + processed_1];
			currentState = 10;
			processed_1++;
		}
		else if (currentState ==10){
			if((var_2 == var_4)){
				currentState = 11;
			}
			else if((var_2!= var_4)){
				currentState = 16;
			}
		}
		else if (currentState ==11){
			var_1 = var_1 + 1;
			currentState = 12;
		}
		else if (currentState ==12){
			if((var_1 == var_3)){
				currentState = 13;
			}
			else if((var_1!= var_3)){
				currentState = 9;
			}
		}
		else if (currentState ==13){
#ifdef DEBUG
			test_output[printed] = var_1;
			printed++;
#else 
			output[globalID][0] = var_1;
#endif
			currentState = 14;
		}
		else if (currentState ==14){
			var_1 = var_1 & 0;
			currentState = 15;
		}
		else if (currentState ==15){
			var_2 = var_2 & 0;
			currentState = 9;
		}
		else if (currentState ==16){
#ifdef DEBUG
			test_output[printed] = var_1;
			printed++;
#else 
			output[globalID][0] = var_1;
#endif
			currentState = 17;
		}
		else if (currentState ==17){
				var_4=input[1][base_1 + processed_1];
			currentState = 18;
			processed_1++;
		}
		else if (currentState ==18){
			if((var_2!= var_4)){
				currentState = 19;
			}
			else if((var_2 == var_4)){
				currentState = 21;
			}
		}
		else if (currentState ==19){
			var_2 = var_2 + 1;
			currentState = 20;
		}
		else if (currentState ==20){
				var_4=input[1][base_1 + processed_1];
			currentState = 18;
			processed_1++;
		}
		else if (currentState ==21){
			var_1 = var_1 + 1;
			currentState = 9;
		}
		else {
			currentState = 22;
		}
		if ((processed_0 >= length_0)&& (processed_1 >= length_1))
			done = true;
	}
}
void FSTGPU::coo_csr_baseline_host(){
	printf("Begin coo_csr_baseline_host \n ");
	printf(" Number of block: %d \n", number_of_block);
	printf(" Number of thread per block: %d\n", number_of_thread);
	cudaEvent_t start_execution, stop_execution;
	cudaEventCreate( &start_execution);
	cudaEventCreate( &stop_execution);
	cudaEventRecord( start_execution, 0);
	coo_csr_baseline_global<<< number_of_block,number_of_thread>>> ( partition_input_base, partition_input_length, input,  partition_output_base,  simulated_output, printed_output);
	cudaEventRecord( stop_execution,0);
	cudaEventSynchronize(stop_execution);
	float execution_time_ms = 0;
	cudaEventElapsedTime(&execution_time_ms, start_execution, stop_execution);
	printf("Execution  Finished\n");
	cudaError_t err = cudaGetLastError();
	if ( err != cudaSuccess )
		printf("CUDA Error: %s\n", cudaGetErrorString(err));
	fprintf(stderr,"Execution Time %f ms\n", execution_time_ms);
}
