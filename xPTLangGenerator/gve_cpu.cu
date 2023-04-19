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
void gve_cpu_global(uint32_t tID,
					uint32_t ** input_base, uint32_t ** input_length, uint32_t ** input,
					uint32_t ** output){
	uint32_t base_0 = input_base[tID][0];
	uint32_t length_0 = input_length[tID][0];
	uint32_t processed_0 = 0;
	uint32_t currentState = 0;
	uint32_t var_0 =  0;
	uint32_t var_1 =  0;
	uint32_t printed = 0;
	bool done = false;
	uint32_t cycle =0;
#ifdef CPU_DEBUG
	uint32_t debug_output[200];
	while ( cycle < 200){
		printf("=============%d=============\n", cycle);
		printf("Current State : %d\n", currentState);
		printf("Var[0]: %d\n", var_0);
		printf("Var[1]: %d\n", var_1);
		printf("Processed Input 0: %d/%d\n", processed_0, length_0);
		printf("Current Input: ");
		for ( uint32_t i =0 ; i < 20; i++){
			printf("%d ", input[0][base_0+i]);
}
		printf("\n");
		printf("Current Output: ");
		for ( uint32_t i =0 ; i < printed; i++){
			printf("%d ", debug_output[i]);
		}
		printf("\n");
		cycle++;
#else
	while (  (!done) && ( currentState != 5)){
#endif
		if(currentState == 0){
			var_1 = var_1 & 0;
			currentState = 1;
		}
		else if (currentState ==1){
				var_0=input[0][base_0 + processed_0];
			processed_0++;
			var_1 = var_0 & 127;
			var_1 = var_1 | 128;
			var_0 = var_0 >> 7;
			currentState = 2;
		}
		else if (currentState ==2){
			if((var_0!=0)){
				currentState = 3;
			}
			else if((var_0 == 0)){
				currentState = 4;
			}
		}
		else if (currentState ==3){
#ifdef CPU_DEBUG
			debug_output[printed] = var_1;
			printed++;
#else 
			output[tID][0] = var_1;
#endif
			var_1 = var_0 & 127;
			var_1 = var_1 | 128;
			var_0 = var_0 >> 7;
			currentState = 2;
		}
		else if (currentState ==4){
			var_0 = var_0 & 0;
			var_1 = var_1 & 127;
#ifdef CPU_DEBUG
			debug_output[printed] = var_1;
			printed++;
#else 
			output[tID][0] = var_1;
#endif
			currentState = 1;
		}
		else {
			currentState = 5;
		}
		if ((processed_0 >= length_0))
			done = true;
	}
}
void *Traverse_variable_length_encoding( void * input){
	struct args * data = (struct args*) input;
	gve_cpu_global( data->id, data->partition_input_base_cpu, data->partition_input_length_cpu, data->input, data->output_cpu);
	return NULL;
}


void TP::gve_cpu_host(){
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
		pthread_create( &worker[i], NULL,& Traverse_variable_length_encoding,  (void*) &threadArg[i]);
	}

	for( uint32_t  i = 0; i<  instances; i++){
		pthread_join ( worker[i], NULL);
	}
	double end_time = gettime_ms();
	printf("Execution  Finished\n");
	std::cerr << "Traversal Time of "<< instances << " Thread: " << end_time - start_time << " ms" << std::endl;
	}
