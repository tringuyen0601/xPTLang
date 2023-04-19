#include "gpu_fst_engine.h"
#include "shared.h"
typedef struct Return_message{
	uint32_t var_0;
	uint32_t var_1;
	uint32_t nextState;
} Return_message;

__global__
void csv_raw_baseline_global( uint32_t ** input_base, uint32_t ** input_length, uint32_t ** input,
						uint32_t **output_base, uint32_t ** output, uint32_t * test_output){
	uint32_t globalID = blockIdx.x * blockDim.x + threadIdx.x;
	uint32_t startStack_shared = threadIdx.x * 1;
	uint32_t base_0 = input_base[globalID][0];
	uint32_t length_0 = input_length[globalID][0];
	uint32_t processed_0 = 0;
	uint32_t currentState = 0;
	uint32_t var_0 =  0;
	uint32_t var_1 =  0;
	uint32_t var_2 =  0;
	__shared__ uint32_t stack [1 * STACK_DEPTH * THREAD_PER_BLOCK];
	for (uint32_t i = 0; i < 1; i++){
		 stack[ (threadIdx.x *1 +i)* STACK_DEPTH ] = 0;
	}
	__syncthreads();
	uint32_t printed = 0;
	bool done = false;
	uint32_t cycle =0;
	Return_message message;
#ifdef DEBUG
	while ( cycle < 20){
		printf("=============%d=============\n", cycle);
		printf("Current State : %d\n", currentState);
		printf("Stack[ %d]:depth %d\n", startVar_shared + 0, var_0);
		for ( uint32_t v = 0; v < var_0; v++){
			printf("%d, ", stack[(startVar_shared +0) * STACK_DEPTH + v]);
		}
		printf("\n");
		printf("Var[1]: %d\n", var_1);
		printf("Var[2]: %d\n", var_2);
		printf("Processed Input 0: %d/%d\n", processed_0, length_0);
		printf("Current Input: ");
		for ( uint32_t i =0 ; i < 20; i++){
			printf("%d ", input[0][base_0+i]);
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
			if((var_1 == 0)){
				currentState = 1;
			}
			else if((var_1!=0)){
				currentState = 18;
			}
		}
		else if (currentState ==1){
				var_2=input[0][base_0 + processed_0];
			currentState = 2;
			processed_0++;
		}
		else if (currentState ==2){
			if((var_2!=59)){
				currentState = 3;
			}
			else if((var_2 == 59)){
				currentState = 4;
			}
		}
		else if (currentState ==3){
				var_2=input[0][base_0 + processed_0];
			currentState = 2;
			processed_0++;
		}
		else if (currentState ==4){
			var_1 = var_1 + 1;
			currentState = 5;
		}
		else if (currentState ==5){
				var_2=input[0][base_0 + processed_0];
			currentState = 6;
			processed_0++;
		}
		else if (currentState ==6){
			if((var_2!=59)){
				currentState = 7;
			}
			else if((var_2 == 59)){
				currentState = 8;
			}
		}
		else if (currentState ==7){
				var_2=input[0][base_0 + processed_0];
			currentState = 6;
			processed_0++;
		}
		else if (currentState ==8){
			var_1 = var_1 + 1;
			currentState = 9;
		}
		else if (currentState ==9){
				var_2=input[0][base_0 + processed_0];
			currentState = 10;
			processed_0++;
		}
		else if (currentState ==10){
			if((var_2 == 2)){
				currentState = 11;
			}
			else if((var_2!=2)){
				currentState = 13;
			}
		}
		else if (currentState ==11){
				for( uint32_t i = 0; i < var_0; i++){
#ifdef DEBUG
				test_output[printed] = stack[(threadIdx.x * 1 + 0) * STACK_DEPTH + i];
				printed++;
#else 
				output[globalID][0] = stack[(threadIdx.x * 1 + 0) * STACK_DEPTH + i];
#endif
				}
			currentState = 12;
		}
		else if (currentState ==12){
				var_0=1;
				stack[(threadIdx.x * 1 + 0) * STACK_DEPTH ]=0;
			currentState = 14;
		}
		else if (currentState ==13){
				var_0=1;
				stack[(threadIdx.x * 1 + 0) * STACK_DEPTH ]=0;
			currentState = 14;
		}
		else if (currentState ==14){
				var_2=input[0][base_0 + processed_0];
			currentState = 15;
			processed_0++;
		}
		else if (currentState ==15){
			if((var_2!=10)){
				currentState = 16;
			}
			else if((var_2 == 10)){
				currentState = 0;
			}
		}
		else if (currentState ==16){
				var_2=input[0][base_0 + processed_0];
			currentState = 17;
			processed_0++;
		}
		else if (currentState ==17){
			var_1 = var_1 & 0;
			currentState = 15;
		}
		else if (currentState ==18){
				var_2=input[0][base_0 + processed_0];
			currentState = 19;
			processed_0++;
		}
		else if (currentState ==19){
			if((var_2!=59)){
				currentState = 20;
			}
			else if((var_2 == 59)){
				currentState = 21;
			}
		}
		else if (currentState ==20){
				var_2=input[0][base_0 + processed_0];
			currentState = 19;
			processed_0++;
		}
		else if (currentState ==21){
			var_1 = var_1 + 1;
			currentState = 0;
		}
		else {
			currentState = 22;
		}
		if ((processed_0 >= length_0))
			done = true;
	}
}
void FSTGPU::csv_raw_baseline_host(){
	printf("Begin csv_raw_baseline_host \n ");
	printf(" Number of block: %d \n", number_of_block);
	printf(" Number of thread per block: %d\n", number_of_thread);
	cudaEvent_t start_execution, stop_execution;
	cudaEventCreate( &start_execution);
	cudaEventCreate( &stop_execution);
	cudaEventRecord( start_execution, 0);
	csv_raw_baseline_global<<< number_of_block,number_of_thread>>> ( partition_input_base, partition_input_length, input,  partition_output_base,  simulated_output, printed_output);
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
