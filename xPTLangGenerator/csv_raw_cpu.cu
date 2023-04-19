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
void csv_raw_cpu_global(uint32_t tID,
					uint32_t ** input_base, uint32_t ** input_length, uint32_t ** input,
					uint32_t ** output){
	uint32_t base_0 = input_base[tID][0];
	uint32_t length_0 = input_length[tID][0];
	uint32_t processed_0 = 0;
	uint32_t currentState = 0;
	uint32_t var_0 =  0;
	uint32_t var_1 =  0;
	uint32_t var_2 =  0;
		uint32_t stack_0[16];
	uint32_t printed = 0;
	bool done = false;
	uint32_t cycle =0;
#ifdef CPU_DEBUG
	uint32_t debug_output[800];
	while ( cycle < 200){
		printf("=============%d=============\n", cycle);
		printf("Current State : %d\n", currentState);
		printf("Stack[ %d]:depth %d\n", 0, var_0);
		for ( uint32_t v = 0; v < var_0; v++){
			printf("%d, ", stack_0[v]);
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
			printf("%d ", debug_output[i]);
		}
		printf("\n");
		cycle++;
#else
	while (  (!done) && ( currentState != 18)){
#endif
		if(currentState == 0){
			if((var_1 == 0)){
				currentState = 1;
			}
			else if((var_1!=0)){
				currentState = 2;
			}
		}
		else if (currentState ==1){
				var_2=input[0][base_0 + processed_0];
			currentState = 3;
			processed_0++;
		}
		else if (currentState ==2){
				var_2=input[0][base_0 + processed_0];
			currentState = 15;
			processed_0++;
		}
		else if (currentState ==3){
			if((var_2!=59)){
				currentState = 4;
			}
			else if((var_2 == 59)){
				currentState = 5;
			}
		}
		else if (currentState ==4){
				var_2=input[0][base_0 + processed_0];
			currentState = 3;
			processed_0++;
		}
		else if (currentState ==5){
			var_1 = var_1 + 1;
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
				var_2=input[0][base_0 + processed_0];
			currentState = 9;
			processed_0++;
		}
		else if (currentState ==9){
			if((var_2 == 2)){
				currentState = 10;
			}
			else if((var_2!=2)){
				currentState = 11;
			}
		}
		else if (currentState ==10){
				for( uint32_t i = 0; i < var_0; i++){
#ifdef CPU_DEBUG
				debug_output[printed] = stack_0[i];
				printed++;
#else 
				output[tID][0] = stack_0[i];
#endif
				}
				var_0=1;
				stack_0[0]=0;
			currentState = 12;
		}
		else if (currentState ==11){
				var_0=1;
				stack_0[0]=0;
			currentState = 12;
		}
		else if (currentState ==12){
				var_2=input[0][base_0 + processed_0];
			currentState = 13;
			processed_0++;
		}
		else if (currentState ==13){
			if((var_2!=10)){
				currentState = 14;
			}
			else if((var_2 == 10)){
				currentState = 0;
			}
		}
		else if (currentState ==14){
				var_2=input[0][base_0 + processed_0];
			processed_0++;
			var_1 = var_1 & 0;
			currentState = 13;
		}
		else if (currentState ==15){
			if((var_2!=59)){
				currentState = 16;
			}
			else if((var_2 == 59)){
				currentState = 17;
			}
		}
		else if (currentState ==16){
				var_2=input[0][base_0 + processed_0];
			currentState = 15;
			processed_0++;
		}
		else if (currentState ==17){
			var_1 = var_1 + 1;
			currentState = 0;
		}
		else {
			currentState = 18;
		}
		if ((processed_0 >= length_0))
			done = true;
	}
}
void *Traverse_csv_raw( void * input){
	struct args * data = (struct args*) input;
	csv_raw_cpu_global( data->id, data->partition_input_base_cpu, data->partition_input_length_cpu, data->input, data->output_cpu);
	return NULL;
}


void TP::csv_raw_cpu_host(){
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
		pthread_create( &worker[i], NULL,& Traverse_csv_raw,  (void*) &threadArg[i]);
	}

	for( uint32_t  i = 0; i<  instances; i++){
		pthread_join ( worker[i], NULL);
	}
	double end_time = gettime_ms();
	printf("Execution  Finished\n");
	std::cerr << "Traversal Time of "<< instances << " Thread: " << end_time - start_time << " ms" << std::endl;
	}
