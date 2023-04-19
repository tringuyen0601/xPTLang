CUDAROOT=/usr/local/cuda-11.5
CUDA_LIB_DIR= -L$(CUDAROOT)/lib64
CUDA_INC_DIR= -I$(CUDAROOT)/include
CC = g++
CUDA= nvcc
PTHREAD = -lpthread
OPT = -g
WARN = -Wall
VERSION =  -std=c++11 
CFLAGS = $(OPT) $(VERSION)
CUDAINFO = --ptxas-options=-v
CUDAFLAG = -arch=sm_61

FST_CPU   = helper.o  argument.o  processor.o processor_helper.o processor_io.o processor_partition.o 
FST_ENGINE = gpu_fst_engine_io.o gpu_fst_engine_partition.o 
FST_CODE_GEN = bpd_baseline.o bpe_baseline.o gvd_baseline.o gve_baseline.o rld_baseline.o rle_baseline.o coo_csr_baseline.o dense_csr_baseline.o csv_enc_baseline.o hist_baseline.o csv_raw_baseline.o 
FST_CODE_BASE = function_state.o function_state_codegen_base.o function_state_helper.o function_state_merge.o function_state_cpu.o
FST_FUNCTION_GROUP = bpd_function_group.o bpe_function_group.o gvd_function_group.o gve_function_group.o rld_function_group.o rle_function_group.o coo_csr_function_group.o dense_csr_function_group.o csv_enc_function_group.o hist_function_group.o csv_raw_function_group.o
FST_CPU_GEN = rle_cpu.o rld_cpu.o  bpe_cpu.o bpd_cpu.o gve_cpu.o gvd_cpu.o coo_csr_cpu.o dense_csr_cpu.o hist_cpu.o csv_enc_cpu.o csv_raw_cpu.o
CORE_OBJ =   $(FST_CPU)  $(FST_ENGINE)
GPULOAD = main_gpu_load_test.o
CPULOAD = main_cpu_load_test.o
CODE_GEN = main_intermediate_gen.o
#################################

# default rule
all:  object	generate

object:
	$(CC) $(CFLAGS) -c *.cc 
	$(CUDA) $(CUDAFLAG) $(CUDA_LIB_DIR) $(CUDA_INC_DIR)  -lcuda -lcudart -c *.cu  
generate:
	$(CUDA) -o bin/cpu_load_test  $(CUDA_LIB_DIR) $(CUDA_INC_DIR)  -lcudart $(CFLAGS) $(CORE_OBJ) $(CPULOAD)  $(FST_CPU_GEN)
	$(CUDA) -o bin/gpu_load_test  $(CUDA_LIB_DIR) $(CUDA_INC_DIR) -lcuda -lcudart $(CFLAGS) $(FST_CPU)  $(FST_ENGINE) $(FST_CODE)  $(GPULOAD) $(FST_CODE_GEN) $(FST_FUNCTION_GROUP) 
	$(CUDA) -o bin/code_gen  $(CUDA_LIB_DIR) $(CUDA_INC_DIR) -lcuda -lcudart $(CFLAGS) $(FST_CPU)  $(FST_ENGINE)  $(FST_CODE_BASE) $(CODE_GEN)  
clean:

	rm -f *.o
	rm -f bin/*
