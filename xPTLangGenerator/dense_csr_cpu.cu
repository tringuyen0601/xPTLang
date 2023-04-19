#include "processor.h"
#include <sys/time.h>
struct args {
	uint32_t ** partition_input_base_cpu;
	uint32_t ** partition_input_length_cpu;
	uint32_t ** input;
	uint32_t ** output_cpu;
	uint32_t id;
};
inline double gettime_ms(){
timespec t;
clock_gettime(CLOCK_MONOTONIC_RAW, &t);
return (t.tv_sec + t.tv_nsec*1e-9)*1000;
}
void dense_csr_cpu_global(uint32_t tID,
					uint32_t ** input_base, uint32_t ** input_length, uint32_t ** input,
					uint32_t ** output){
	uint32_t base_0 = input_base[tID][0];
	uint32_t length_0 = input_length[tID][0];
	uint32_t processed_0 = 0;
	uint32_t base_1 = input_base[tID][1];
	uint32_t length_1 = input_length[tID][1];
	uint32_t processed_1 = 0;
	uint32_t currentState = 0;
	uint32_t var_0 =  0;
	uint32_t var_1 =  0;
	uint32_t var_2 =  0;
	uint32_t var_3 =  0;
	uint32_t var_4 =  0;
	uint32_t var_5 =  0;
	uint32_t printed = 0;
	bool done = false;
	uint32_t cycle =0;
#ifdef CPU_DEBUG
	uint32_t debug_output[800];
	while ( cycle < 200){
		printf("=============%d=============\n", cycle);
		printf("Current State : %d\n", currentState);
		printf("Var[0]: %d\n", var_0);
		printf("Var[1]: %d\n", var_1);
		printf("Var[2]: %d\n", var_2);
		printf("Var[3]: %d\n", var_3);
		printf("Var[4]: %d\n", var_4);
		printf("Var[5]: %d\n", var_5);
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
			printf("%d ", debug_output[i]);
		}
		printf("\n");
		cycle++;
#else
	while (  (!done) && ( currentState != 9)){
#endif
		if(currentState == 0){
				var_0= 0;
				var_2=input[0][base_0 + processed_0];
			processed_0++;
				var_2=input[0][base_0 + processed_0];
			processed_0++;
				var_4=input[0][base_0 + processed_0];
			processed_0++;
				var_1= 0;
				var_3= 0;
			currentState = 1;
		}
		else if (currentState ==1){
				var_3=input[1][base_1 + processed_1];
			currentState = 2;
			processed_1++;
		}
		else if (currentState ==2){
			if((var_3!=0)){
				currentState = 3;
			}
			else if((var_3 == 0)){
				currentState = 4;
			}
		}
		else if (currentState ==3){
			var_5 = var_5 + 1;
			currentState = 4;
		}
		else if (currentState ==4){
			var_1 = var_1 + 1;
			currentState = 5;
		}
		else if (currentState ==5){
			if((var_1 == var_2)){
				currentState = 6;
			}
			else if((var_1!= var_2)){
				currentState = 1;
			}
		}
		else if (currentState ==6){
				var_1= 0;
			var_0 = var_0 + 1;
#ifdef CPU_DEBUG
			debug_output[printed] = var_5;
			printed++;
#else 
			output[tID][0] = var_5;
#endif
			currentState = 7;
		}
		else if (currentState ==7){
			if((var_5 == var_4)){
				currentState = 8;
			}
			else if((var_5!= var_4)){
				currentState = 1;
			}
		}
		else if (currentState ==8){
				var_0= 0;
				var_1= 0;
				var_3= 0;
			currentState = 1;
		}
		else {
			currentState = 9;
		}
		if ((processed_0 >= length_0)&& (processed_1 >= length_1))
			done = true;
	}
}
void *Traverse_dense_csr( void * input){
	struct args * data = (struct args*) input;
	dense_csr_cpu_global( data->id, data->partition_input_base_cpu, data->partition_input_length_cpu, data->input, data->output_cpu);
	return NULL;
}


void TP::dense_csr_cpu_host(){
	printf("Begin host function \n ");
	printf(" Number of thread: %d\n", instances);
	struct args * threadArg = (struct args*)malloc (instances  * sizeof( struct args));
	pthread_t worker[instances];
	for ( uint32_t i = 0; i<  instances; i++){
		threadArg[i].id           =  i;
		threadArg[i].partition_input_base_cpu = partition_input_base_cpu;
		threadArg[i].partition_input_length_cpu =partition_input_length_cpu;
		threadArg[i].input  = input;
		threadArg[i].output_cpu = output_cpu;
	}
	double start_time = gettime_ms();
	for ( uint32_t i = 0; i<  instances; i++){
		pthread_create( &worker[i], NULL,& Traverse_dense_csr,  (void*) &threadArg[i]);
	}

	for( uint32_t  i = 0; i<  instances; i++){
		pthread_join ( worker[i], NULL);
	}
	double end_time = gettime_ms();
	printf("Execution  Finished\n");
	std::cerr << "Traversal Time of "<< instances << " Thread: " << end_time - start_time << " ms" << std::endl;
	}
