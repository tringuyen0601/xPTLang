#include "function_state.h"

void CODE_BASE_FST::write_datastruct(){
  std::ofstream ptr ( baseline_file, std::ios::app);
  ptr <<"#include \"gpu_fst_engine.h\""<<std::endl;
  ptr <<"#include \"shared.h\""<<std::endl;
  ptr <<"typedef struct Return_message{" <<std::endl;
  ptr <<"\tuint32_t var_0;" <<std::endl;
  ptr <<"\tuint32_t var_1;" <<std::endl;
  ptr <<"\tuint32_t nextState;" <<std::endl;
  ptr <<"} Return_message;\n" <<std::endl;
  ptr.close();
}

void CODE_BASE_FST::write_host_function(){
  std::ofstream ptr ( baseline_file, std::ios::app);
  ptr <<"void FSTGPU::"<< host_function_name<<"(){"<<std::endl;

  ptr<<"\tprintf(\"Begin "<<host_function_name <<" \\n \");" <<std::endl;
  ptr<<"\tprintf(\" Number of block: \%d \\n\", number_of_block);" << std::endl;
  ptr<<"\tprintf(\" Number of thread per block: \%d\\n\", number_of_thread);" << std::endl;
  ptr<<"\tcudaEvent_t start_execution, stop_execution;" <<std::endl;
  ptr<<"\tcudaEventCreate( &start_execution);" << std::endl;
  ptr<<"\tcudaEventCreate( &stop_execution);" << std::endl;
  ptr<<"\tcudaEventRecord( start_execution, 0);" <<std::endl;
  ptr<<"\t"<< global_function_name <<"<<< number_of_block,number_of_thread>>> ( partition_input_base, partition_input_length, input,  partition_output_base,  simulated_output, printed_output);" <<std::endl;
  ptr<<"\tcudaEventRecord( stop_execution,0);" << std::endl;
  ptr<<"\tcudaEventSynchronize(stop_execution);" <<std::endl;
  ptr<<"\tfloat execution_time_ms = 0;" << std::endl;
  ptr<<"\tcudaEventElapsedTime(&execution_time_ms, start_execution, stop_execution);" << std::endl;
  ptr<<"\tprintf(\"Execution  Finished\\n\");" << std::endl;
  ptr<<"\tcudaError_t err = cudaGetLastError();" << std::endl;
  ptr<<"\tif ( err != cudaSuccess )" << std::endl;
  ptr<<"\t\tprintf(\"CUDA Error: \%s\\n\", cudaGetErrorString(err));" << std::endl;
  ptr<<"\tfprintf(stderr,\"Execution Time \%f ms\\n\", execution_time_ms);" << std::endl;
  ptr<<"}"<< std::endl;
  ptr.close();
}

void CODE_BASE_FST::write_global_function(){
  std::ofstream ptr ( baseline_file, std::ios::app);
  ptr<<"__global__"<< std::endl;
  ptr<< "void "<< global_function_name<<"( uint32_t ** input_base, uint32_t ** input_length, uint32_t ** input," << std::endl;
  ptr <<"\t\t\t\t\t\tuint32_t **output_base, uint32_t ** output, uint32_t * test_output){" << std::endl;
  ptr<<"\tuint32_t globalID = blockIdx.x * blockDim.x + threadIdx.x;" << std::endl;
  ptr<<"\tuint32_t startStack_shared = threadIdx.x * " << number_of_stack << ";" << std::endl;
  for ( uint32_t i = 0 ; i< mem_fst->inputCount; i++){
    ptr<<"\tuint32_t base_" <<i  << " = input_base[globalID][" << i<<"];" << std::endl;
    ptr<<"\tuint32_t length_" <<i<<" = input_length[globalID]["<<i<<"];" << std::endl;
    ptr<<"\tuint32_t processed_"<< i <<" = 0;" << std::endl;
  }
  ptr<<"\tuint32_t currentState = 0;"<< std::endl;
  for ( uint32_t i = 0 ; i < number_of_var; i++){
    ptr<<"\tuint32_t var_" << i <<" =  0;" << std::endl;
  }
  if ( number_of_stack > 0){
    ptr<<"\t__shared__ uint32_t stack [" << number_of_stack <<" * STACK_DEPTH * THREAD_PER_BLOCK];" << std::endl;
    ptr<<"\tfor (uint32_t i = 0; i < "<< number_of_stack <<"; i++){" << std::endl;
    ptr<<"\t\t stack[ (threadIdx.x *"<< number_of_stack<<" +i)* STACK_DEPTH ] = 0;" << std::endl;
    ptr<<"\t}"<< std::endl;
    ptr<<"\t__syncthreads();"<< std::endl;
  }

  ptr<<"\tuint32_t printed = 0;" << std::endl;
  ptr<<"\tbool done = false;" << std::endl;
  ptr<<"\tuint32_t cycle =0;" << std::endl;
  ptr<<"\tReturn_message message;" << std::endl;

  ptr<<"#ifdef DEBUG"<< std::endl;
  ptr<<"\twhile ( cycle < 20){" << std::endl;
  ptr<<"\t\tprintf(\"=============\%d=============\\n\", cycle);"<<std::endl;
  ptr<<"\t\tprintf(\"Current State : \%d\\n\", currentState);" << std::endl;
  for ( uint32_t i = 0 ; i < number_of_var; i++){
    if ( mem_fst->stackMap[i] == 0){ 
      ptr<<"\t\tprintf(\"Var[" << i<<"]: \%d\\n\", var_" << i <<");" << std::endl;
    }
    else {
      ptr<<"\t\tprintf(\"Stack[ \%d]:depth \%d\\n\", startVar_shared + "<<mem_fst->IdMap[i]<<", var_" << i <<");" << std::endl;

      ptr<<"\t\tfor ( uint32_t v = 0; v < var_"<< i<<"; v++){"<< std::endl;
      ptr<<"\t\t\tprintf(\"\%d, \", stack[(startVar_shared +"<< mem_fst->IdMap[i]<<") * STACK_DEPTH + v]);" << std::endl;
      ptr<<"\t\t}" << std::endl;
      ptr<<"\t\tprintf(\"\\n\");"<< std::endl;
    }
  }
  for ( uint32_t i = 0 ; i< mem_fst->inputCount; i++){
    ptr<<"\t\tprintf(\"Processed Input " << i <<": \%d/\%d\\n\", processed_" <<i <<", length_" <<i<<");" << std::endl;
    ptr<<"\t\tprintf(\"Current Input: \");" << std::endl;
    ptr<<"\t\tfor ( uint32_t i =0 ; i < 20; i++){" << std::endl;
    ptr<<"\t\t\tprintf(\"\%d \", input[" <<i<<"][base_" << i<<"+i]);" << std::endl;
    ptr<<"}" << std::endl;
    ptr<<"\t\tprintf(\"\\n\");" << std::endl;
  }
  ptr<<"\t\tprintf(\"Current Output: \");" << std::endl;
  ptr<<"\t\tfor ( uint32_t i =0 ; i < printed; i++){" << std::endl;
  ptr<<"\t\t\tprintf(\"\%d \", test_output[i]);" << std::endl;
  ptr<<"\t\t}" << std::endl;
  ptr<<"\t\tprintf(\"\\n\");" << std::endl;
  ptr<<"\t\tcycle++;" << std::endl;
  ptr<<"#else"<< std::endl;
  if ( c_mode_s == "baseline"){
    ptr<<"\twhile (  (!done) && ( currentState != "<<number_of_function <<")){" << std::endl;
  }
  else if ( c_mode_s == "function_group"){
    ptr<<"\twhile (  (!done) && ( currentState != "<<number_of_merge_function <<")){" << std::endl;
  }
  ptr<<"#endif" << std::endl;
  ptr.close();

  //write function
  if ( c_mode_s == "baseline"){
    write_state_function();
  }
  else if ( c_mode_s == "function_group"){
    write_group_function();
  }
  //write end
  std::ofstream ptr_ret ( baseline_file, std::ios::app);
  ptr_ret<< "\t\telse {" << std::endl;
  if ( c_mode_s == "baseline"){
    ptr_ret<< "\t\t\tcurrentState = " << number_of_function <<";" << std:: endl; 
  }
  else if ( c_mode_s == "function_group"){
    ptr_ret<< "\t\t\tcurrentState = " << number_of_merge_function <<";" << std:: endl; 
  }
  ptr_ret<<"\t\t}" << std::endl;
  ptr_ret<<"\t\tif ("; 
  ptr_ret<<"(processed_0 >= length_0)";
  for ( uint32_t i = 1; i < mem_fst->inputCount; i++)
    ptr_ret<<"&& (processed_" << i <<" >= length_" << i<<")";
  ptr_ret<<")"<<std::endl; 
  ptr_ret <<"\t\t\tdone = true;"<< std::endl;

  ptr_ret<<"\t}" << std::endl;
  ptr_ret<<"}" << std::endl;
  ptr_ret.close();
}

void CODE_BASE_FST::write_state_function(){
  std::ofstream ptr ( baseline_file, std::ios::app);
  for ( uint32_t i = 0; i < number_of_function;i++){
    if (i ==0) ptr<< "\t\tif(currentState == 0){" << std::endl;
    else ptr<<"\t\telse if (currentState ==" << i << "){"<< std::endl;
    std::string action = action_to_string(i);
    if ( action !="None")
      ptr<<"\t\t\t" << action <<";" << std::endl;  
    ptr << write_condition(i);
    ptr<<"\t\t}" << std::endl;
  }
  ptr.close();
}

std::string CODE_BASE_FST::write_condition(uint32_t id){
  uint32_t base =  mem_fst->stateList[id].baseID ;
  bool Icond = functionList[id].conditional_input;
  bool Scond = functionList[id].conditional_stack;

  // load first transition
  uint32_t inputID,IcompareCode,  inputSymbol, IvarID,ScompareCode, inputVar;
  uint32_t outputID, outputSymbol, OvarID, outputVar;
  uint32_t nextState;
  inputID      = mem_fst->transitionList[base].inputID;
  inputSymbol  = mem_fst->transitionList[base].inputSymbol;
  IcompareCode = mem_fst->transitionList[base].IcompareCode;
  IvarID       = mem_fst->transitionList[base].IvarID;
  ScompareCode = mem_fst->transitionList[base].ScompareCode;
  inputVar     = mem_fst->transitionList[base].inputVar;
  outputID     = mem_fst->transitionList[base].outputID;
  outputSymbol = mem_fst->transitionList[base].outputSymbol;
  OvarID       = mem_fst->transitionList[base].OvarID;
  outputVar    = mem_fst->transitionList[base].outputVar;
  nextState    = mem_fst->transitionList[base].nextState;

  
  std::string cond = "";
    if (functionList[id].number_of_tx <2){
      // only 1 outgoing transition
      if ( Icond || Scond){
        cond = cond +"\t\t\tif(" ;
        // condition
        if (Icond)
          cond = cond + "(" + transition_to_string_codegen(inputID)  + transition_to_string_codegen_condition_side(inputSymbol, IcompareCode)+")";  
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen(IvarID)  + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen(IvarID) + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"}" ;
      }
      else{
      // no condition, just output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
      }
      if (( inputSymbol ==ANY_MATCH) || ( inputID != EPSILON_MATCH))
        cond = cond + "\t\t\tprocessed_"+std::to_string(inputID-(INPUTSTART)) +"++;\n"; 
    }
    // multiple outgoing transition
    else {
      for ( uint32_t i = 0 ; i < functionList[id].number_of_tx; i++){
        inputID      = mem_fst->transitionList[base+i].inputID;
        inputSymbol  = mem_fst->transitionList[base+i].inputSymbol;
        IcompareCode = mem_fst->transitionList[base+i].IcompareCode;
        IvarID       = mem_fst->transitionList[base+i].IvarID;
        inputVar     = mem_fst->transitionList[base+i].inputVar;
        ScompareCode = mem_fst->transitionList[base+i].ScompareCode;
        outputID     = mem_fst->transitionList[base+i].outputID;
        outputSymbol = mem_fst->transitionList[base+i].outputSymbol;
        OvarID       = mem_fst->transitionList[base+i].OvarID;
        outputVar    = mem_fst->transitionList[base+i].outputVar;
        nextState    = mem_fst->transitionList[base+i].nextState;
        if ( i==0)
          cond = cond +"\t\t\tif(" ;
        else 
          cond = cond +"\t\t\telse if(" ;
        // condition
        if (Icond)
          cond = cond + "(" + transition_to_string_codegen(inputID) +  transition_to_string_codegen_condition_side(inputSymbol, IcompareCode)+")";  
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen(IvarID) +  transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen(IvarID) + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
          cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        cond = cond + "\t\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"\t\t\t}\n" ;
      }  
      if (( inputSymbol ==ANY_MATCH) || ( inputID != EPSILON_MATCH))
        cond = cond + "\t\t\tprocessed_"+std::to_string(inputID-(INPUTSTART)) +"++;\n"; 
    }  
  return cond;
}


std::string CODE_BASE_FST::write_group_condition(uint32_t id){
  uint32_t base =  mem_fst->stateList[id].baseID ;
  bool Icond = functionList[id].conditional_input;
  bool Scond = functionList[id].conditional_stack;

  // load first transition
  uint32_t inputID,IcompareCode, inputSymbol,ScompareCode, IvarID, inputVar;
  uint32_t outputID, outputSymbol, OvarID, outputVar;
  uint32_t nextState;
  inputID      = mem_fst->transitionList[base].inputID;
  inputSymbol  = mem_fst->transitionList[base].inputSymbol;
  IcompareCode = mem_fst->transitionList[base].IcompareCode;
  IvarID       = mem_fst->transitionList[base].IvarID;
  ScompareCode = mem_fst->transitionList[base].ScompareCode;
  inputVar     = mem_fst->transitionList[base].inputVar;
  outputID     = mem_fst->transitionList[base].outputID;
  outputSymbol = mem_fst->transitionList[base].outputSymbol;
  OvarID       = mem_fst->transitionList[base].OvarID;
  outputVar    = mem_fst->transitionList[base].outputVar;
  nextState    = find_function (mem_fst->transitionList[base].nextState);
  if ( nextState == number_of_function) {
    std::cout <<" Invalid destination" << std::endl;
    exit(0);
  }
  
  std::string cond = "";
    if (functionList[id].number_of_tx <2){
      // only 1 outgoing transition
      if ( Icond || Scond){
        cond = cond +"\t\t\tif(" ;
        // condition
        if (Icond)
          cond = cond + "(" + transition_to_string_codegen(inputID)  + transition_to_string_codegen_condition_side(inputSymbol, IcompareCode)+")";  
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen(IvarID)  + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen(IvarID) + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"}" ;
      }
      else{
      // no condition, just output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
      }
      if (( inputSymbol ==ANY_MATCH) || ( inputID != EPSILON_MATCH))
        cond = cond + "\t\t\tprocessed_"+std::to_string(inputID-(INPUTSTART)) +"++;\n"; 
    }
    // multiple outgoing transition
    else {
      for ( uint32_t i = 0 ; i < functionList[id].number_of_tx; i++){
        inputID      = mem_fst->transitionList[base+i].inputID;
        inputSymbol  = mem_fst->transitionList[base+i].inputSymbol;
        IcompareCode = mem_fst->transitionList[base+i].IcompareCode;
        IvarID       = mem_fst->transitionList[base+i].IvarID;
        ScompareCode = mem_fst->transitionList[base+i].ScompareCode;
        inputVar     = mem_fst->transitionList[base+i].inputVar;
        outputID     = mem_fst->transitionList[base+i].outputID;
        outputSymbol = mem_fst->transitionList[base+i].outputSymbol;
        OvarID       = mem_fst->transitionList[base+i].OvarID;
        outputVar    = mem_fst->transitionList[base+i].outputVar;
        nextState    = find_function(mem_fst->transitionList[base+i].nextState);
        if ( nextState == number_of_function) {
          std::cout <<" Invalid destination" << std::endl;
          exit(0);
        }
        if ( i==0)
          cond = cond +"\t\t\tif(" ;
        else 
          cond = cond +"\t\t\telse if(" ;
        // condition
        if (Icond)
          cond = cond + "(" + transition_to_string_codegen(inputID) +  transition_to_string_codegen_condition_side(inputSymbol, IcompareCode)+")";  
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen(IvarID) +  transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen(IvarID) + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
          cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        cond = cond + "\t\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"\t\t\t}\n" ;
      }  
      if (( inputSymbol ==ANY_MATCH) || ( inputID != EPSILON_MATCH))
        cond = cond + "\t\t\tprocessed_"+std::to_string(inputID-(INPUTSTART)) +"++;\n"; 
    }  
  return cond;
}



std::string CODE_BASE_FST::write_group_member_condition(uint32_t id){
  uint32_t base =  mem_fst->stateList[id].baseID ;
  bool Icond = functionList[id].conditional_input;
  bool Scond = functionList[id].conditional_stack;

  // load first transition
  uint32_t inputID,IcompareCode,  inputSymbol,ScompareCode,  IvarID, inputVar;
  uint32_t outputID, outputSymbol, OvarID, outputVar;
  uint32_t nextState;
  inputID      = mem_fst->transitionList[base].inputID;
  inputSymbol  = mem_fst->transitionList[base].inputSymbol;
  IcompareCode = mem_fst->transitionList[base].IcompareCode;
  IvarID       = mem_fst->transitionList[base].IvarID;
  ScompareCode = mem_fst->transitionList[base].ScompareCode;
  inputVar     = mem_fst->transitionList[base].inputVar;
  outputID     = mem_fst->transitionList[base].outputID;
  outputSymbol = mem_fst->transitionList[base].outputSymbol;
  OvarID       = mem_fst->transitionList[base].OvarID;
  outputVar    = mem_fst->transitionList[base].outputVar;
  
  std::string cond = "";
    if (functionList[id].number_of_tx <2){
      // only 1 outgoing transition
      if ( Icond || Scond){
        std::cout <<" WRONG, group function cannot be conditional" << std::endl;
        cond = cond +"\t\t\tif(" ;
        // condition
        if (Icond)
          cond = cond + "(" + transition_to_string_codegen(inputID)  + transition_to_string_codegen_condition_side(inputSymbol, IcompareCode)+")";  
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen(IvarID)  + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen(IvarID) + transition_to_string_codegen_condition_side(inputVar, ScompareCode)+")";  
        cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        //cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"}" ;
      }
      else{
      // no condition, just output
        if ( transition_to_string_codegen (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output( OvarID, outputVar);
        }
        //cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
      }
      if (( inputSymbol ==ANY_MATCH) || ( inputID != EPSILON_MATCH))
        cond = cond + "\t\t\tprocessed_"+std::to_string(inputID-(INPUTSTART)) +"++;\n"; 
    }
  return cond;
}














