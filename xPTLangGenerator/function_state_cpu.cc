#include "function_state.h"
void CODE_BASE_FST::cpu_write_datastruct(){
  std::ofstream ptr (baseline_file, std::ios::app);
  ptr <<"#include \"processor.h\""<< std::endl;
  ptr <<"#include <sys/time.h>" << std::endl;
  ptr <<"struct args {" << std::endl;
  ptr <<"\tuint32_t ** partition_input_base_cpu;" << std::endl;
  ptr <<"\tuint32_t ** partition_input_length_cpu;"<< std::endl;
  ptr <<"\tuint32_t ** input;"<< std::endl;
  ptr <<"\tuint32_t ** output_cpu;" << std::endl;
  ptr <<"\tuint32_t id;" << std::endl;
  ptr <<"};" << std::endl;

  ptr <<"inline double gettime_ms(){" << std::endl;
  ptr <<"timespec t;" << std::endl;
  ptr <<"clock_gettime(CLOCK_MONOTONIC_RAW, &t);" << std::endl;
  ptr <<"return (t.tv_sec + t.tv_nsec*1e-9)*1000;" << std::endl;
  ptr <<"}" << std::endl;

  ptr.close();
}
void CODE_BASE_FST::cpu_function_gen(){
  std::ofstream baseline_code ( baseline_file, std::ios::out);
  cpu_write_datastruct();
  cpu_write_global_function();
  cpu_write_host_function();
}

void CODE_BASE_FST::cpu_write_host_function(){
  std::ofstream ptr (baseline_file, std::ios::app);

  ptr<<"void *Traverse_"<<test_name <<"( void * input){"<< std::endl;
  ptr<<"\tstruct args * data = (struct args*) input;" << std::endl;
  ptr<<"\t"<<  global_function_name <<"( data->id, data->partition_input_base_cpu, data->partition_input_length_cpu, data->input, data->output_cpu);"<< std::endl;
  ptr<<"\treturn NULL;" << std::endl;
  ptr<<"}"<< std::endl;
  ptr<<std::endl;
  ptr<<std::endl;

  ptr <<"void TP::"<< host_function_name <<"(){" << std::endl;
  ptr<<"\tprintf(\"Begin host function \\n \");" << std::endl; 
  ptr<<"\tprintf(\" Number of thread: %d\\n\", instances);" << std::endl;
  ptr<<"\tstruct args * threadArg = (struct args*)malloc (instances  * sizeof( struct args));" << std::endl;
  ptr<<"\tpthread_t worker[instances];" << std::endl;
  ptr<<"\tfor ( uint32_t i = 0; i<  instances; i++){" << std::endl;
  ptr<<"\t\tthreadArg[i].id           =  i;" <<std::endl;
  ptr<<"\t\tthreadArg[i].partition_input_base_cpu = partition_input_base_cpu;" << std::endl;
  ptr<<"\t\tthreadArg[i].partition_input_length_cpu =partition_input_length_cpu;" <<std::endl;
  ptr<<"\t\tthreadArg[i].input  = input;"<< std::endl;
  ptr<<"\t\tthreadArg[i].output_cpu = output_cpu;" << std::endl;
  ptr<<"\t}"<< std::endl;
  ptr<<"\tdouble start_time = gettime_ms();"<< std::endl;
  ptr<<"\tfor ( uint32_t i = 0; i<  instances; i++){"<< std::endl;
  ptr<<"\t\tpthread_create( &worker[i], NULL,& Traverse_"<<test_name <<",  (void*) &threadArg[i]);"<< std::endl;
  ptr<<"\t}" <<std::endl;
  ptr<<std::endl;
  ptr<<"\tfor( uint32_t  i = 0; i<  instances; i++){"<< std::endl;
  ptr<<"\t\tpthread_join ( worker[i], NULL);" << std::endl;
  ptr<<"\t}"<< std::endl;
  ptr<<"\tdouble end_time = gettime_ms();" << std::endl;
  ptr<<"\tprintf(\"Execution  Finished\\n\");"<<std::endl;
  ptr<<"\tstd::cerr << \"Traversal Time of \"<< instances << \" Thread: \" << end_time - start_time << \" ms\" << std::endl;"<< std::endl;
  ptr<<"\t}"<<std::endl;
  ptr.close();

}
void CODE_BASE_FST::cpu_write_global_function(){
  std::ofstream ptr (baseline_file, std::ios::app);

  ptr<<"void "<<global_function_name<<"(uint32_t tID,"<< std::endl;
  ptr<<"\t\t\t\t\tuint32_t ** input_base, uint32_t ** input_length, uint32_t ** input,"<<std::endl;
  ptr<<"\t\t\t\t\tuint32_t ** output){" <<std::endl;
  for ( uint32_t i = 0 ; i< mem_fst->inputCount; i++){
    ptr<<"\tuint32_t base_" <<i  << " = input_base[tID][" << i<<"];" << std::endl;
    ptr<<"\tuint32_t length_" <<i<<" = input_length[tID]["<<i<<"];" << std::endl;
    ptr<<"\tuint32_t processed_"<< i <<" = 0;" << std::endl;
  }

  ptr<<"\tuint32_t currentState = 0;"<<std::endl;
  for ( uint32_t i = 0 ; i < number_of_var; i++){
    ptr<<"\tuint32_t var_" << i <<" =  0;" << std::endl;
  }
  if ( number_of_stack > 0){
    for (uint32_t i = 0; i<number_of_stack; i++){
      ptr<<"\t\tuint32_t stack_"<<i<<"["<<CPU_STACK_DEPTH<<"];" << std::endl;
    }
  }

  ptr<<"\tuint32_t printed = 0;"<<std::endl;
  ptr<<"\tbool done = false;"<<std::endl;
  ptr<<"\tuint32_t cycle =0;"<<std::endl;

  ptr<<"#ifdef CPU_DEBUG"<< std::endl;
  ptr<<"\tuint32_t debug_output[800];"<< std::endl;
  ptr<<"\twhile ( cycle < 200){" << std::endl;
  ptr<<"\t\tprintf(\"=============\%d=============\\n\", cycle);"<<std::endl;
  ptr<<"\t\tprintf(\"Current State : \%d\\n\", currentState);" << std::endl;
  for ( uint32_t i = 0 ; i < number_of_var; i++){
    if ( mem_fst->stackMap[i] == 0){
      ptr<<"\t\tprintf(\"Var[" << i<<"]: \%d\\n\", var_" << i <<");" << std::endl;
    }
    else {
      ptr<<"\t\tprintf(\"Stack[ \%d]:depth \%d\\n\", "<<mem_fst->IdMap[i]<<", var_" << i <<");" << std::endl;

      ptr<<"\t\tfor ( uint32_t v = 0; v < var_"<< i<<"; v++){"<< std::endl;
      ptr<<"\t\t\tprintf(\"\%d, \", stack_"<< mem_fst->IdMap[i]<<"[v]);" << std::endl;
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
  ptr<<"\t\t\tprintf(\"\%d \", debug_output[i]);" << std::endl;
  ptr<<"\t\t}" << std::endl;
  ptr<<"\t\tprintf(\"\\n\");" << std::endl;
  ptr<<"\t\tcycle++;" << std::endl;
  ptr<<"#else"<< std::endl;
  ptr<<"\twhile (  (!done) && ( currentState != "<<number_of_merge_function <<")){" << std::endl;
  ptr<<"#endif" << std::endl;
  ptr.close();

  write_group_function_cpu();

  //write end
  std::ofstream ptr_ret ( baseline_file, std::ios::app);
  ptr_ret<< "\t\telse {" << std::endl;
  ptr_ret<< "\t\t\tcurrentState = " << number_of_merge_function <<";" << std:: endl;
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

void CODE_BASE_FST::write_group_function_cpu(){
  std::ofstream ptr ( baseline_file, std::ios::app);
  for ( uint32_t i = 0; i < number_of_merge_function;i++){
    if (i ==0) ptr<< "\t\tif(currentState == 0){" << std::endl;
    else ptr<<"\t\telse if (currentState ==" << i << "){"<< std::endl;

    std::cout <<"generate code for function group " << i << std::endl;
    if ( merge_functionList[i].number_of_function >1){
      for ( uint32_t j = 0; j <  (merge_functionList[i].number_of_function-1) ;j++){
        std::cout <<"Member function " << j << std::endl;
        std::string action = action_to_string_cpu( merge_functionList[i].functionID[j]);
        if ( action !="None")
          ptr<<"\t\t\t" << action <<";" << std::endl;
          ptr << write_group_member_condition_cpu( merge_functionList[i].functionID[j]);
      }
      std::cout <<"Member function " << merge_functionList[i].number_of_function-1 << std::endl;
      std::string action = action_to_string_cpu( merge_functionList[i].functionID[merge_functionList[i].number_of_function-1]);   
      if ( action !="None")
        ptr<<"\t\t\t" << action <<";" << std::endl; 
        ptr << write_group_condition_cpu( merge_functionList[i].functionID[merge_functionList[i].number_of_function-1]);

    }
    else{
      std::string action = action_to_string_cpu( merge_functionList[i].functionID[0]);
      if ( action !="None")
        ptr<<"\t\t\t" << action <<";" << std::endl;
        ptr << write_group_condition_cpu( merge_functionList[i].functionID[0]);
    }
    ptr<<"\t\t}" << std::endl;
  }
  ptr.close();
}

std::string CODE_BASE_FST::variable_to_value_cpu( uint32_t id){
  std::string value ="";
  if ( mem_fst->stackMap[id] == true){
    value =  "stack_"+ std::to_string(mem_fst->IdMap[id]) +"[var_" + std::to_string(id) +"-1 ]";
  }
  else{
    value = "var_" + std::to_string(id);
  }
  return value;
}


std::string CODE_BASE_FST::action_to_string_cpu( uint32_t id){
  uint8_t opt = mem_fst->stateList[id].opt;
  uint32_t src1 = mem_fst->stateList[id].src1;
  uint32_t src2 = mem_fst->stateList[id].src2;
  uint32_t dst = mem_fst->stateList[id].dst;
  std::string code;
  switch ( opt) {
    case 1: // ADD
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " + " + variable_to_value_cpu(src2) +"";
      break;
    case 2: // ADD I
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " + " + std::to_string(src2) +"";
      break;
    case 3: //  SUB
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " - " + variable_to_value_cpu(src2) +"";
      break;
    case 4: // SUBI
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " - " + std::to_string(src2) +"";
      break;
    case 5: //  MUL
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " * " + variable_to_value_cpu(src2) +"";
      break;
    case 6: // MULI:
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " * " + std::to_string(src2) +"";
      break;
    case 7: // DIV
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " / " + variable_to_value_cpu(src2) +"";
      break;
    case 8: //DIVI
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " / " + std::to_string(src2) +"";
      break;
    case 9:
      code = "NO LONGER SUPPORT SET";
      break;
    case 10:
      code = "NO LONGER SUPPORT POP";
      break;
    case 11:  // LSHIFT
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " << " + variable_to_value_cpu(src2) +"";
      break;
    case 12:  // LSHIFTI
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " << " + std::to_string(src2) +"";
      break;
    case 13:  // RSHIFT
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " >> " + variable_to_value_cpu(src2) +"";
      break;
    case 14:  // RSHIFTI
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " >> " + std::to_string(src2) +"";
      break;
    case 15:  //  OR
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " | " + variable_to_value_cpu(src2) +"";
      break;
    case 16:  //  ORI
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " | " + std::to_string(src2) +"";
      break;
    case 17:  //  AND
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " & " + variable_to_value_cpu(src2) +"";
      break;
    case 18:  //  ANDI
      code = variable_to_value_cpu(dst) +" = " + variable_to_value_cpu(src1) + " & " + std::to_string(src2) +"";
      break;
    case 0:
      code = "None";
      break;
  }
  return code;
}

std::string CODE_BASE_FST::write_group_member_condition_cpu(uint32_t id){
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
          cond = cond + "(" + transition_to_string_codegen_cpu(inputID)  + transition_to_string_codegen_condition_side_cpu(inputSymbol, IcompareCode)+")";
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen_cpu(IvarID)  + transition_to_string_codegen_condition_side_cpu(inputVar, ScompareCode)+")";
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen_cpu(IvarID) + transition_to_string_codegen_condition_side_cpu(inputVar, ScompareCode)+")";
        cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen_cpu (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output_cpu( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen_cpu (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output_cpu( OvarID, outputVar);
        }
        //cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"}" ;
      }
      else{
      // no condition, just output
        if ( transition_to_string_codegen_cpu (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output_cpu( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen_cpu (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output_cpu( OvarID, outputVar);
        }
        //cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
      }
      if (( inputSymbol ==ANY_MATCH) || ( inputID != EPSILON_MATCH))
        cond = cond + "\t\t\tprocessed_"+std::to_string(inputID-(INPUTSTART)) +"++;\n";
    }
  return cond;
}


std::string CODE_BASE_FST::write_group_condition_cpu(uint32_t id){
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
          cond = cond + "(" + transition_to_string_codegen_cpu(inputID)  + transition_to_string_codegen_condition_side_cpu(inputSymbol, IcompareCode)+")";
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen_cpu(IvarID)  + transition_to_string_codegen_condition_side_cpu(inputVar, ScompareCode)+")";
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen_cpu(IvarID) + transition_to_string_codegen_condition_side_cpu(inputVar, ScompareCode)+")";
        cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen_cpu (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output_cpu( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen_cpu (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output_cpu( OvarID, outputVar);
        }
        cond = cond + "\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"}" ;
      }
      else{
      // no condition, just output
        if ( transition_to_string_codegen_cpu (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output_cpu( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen_cpu (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output_cpu( OvarID, outputVar);
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
          cond = cond + "(" + transition_to_string_codegen_cpu(inputID) +  transition_to_string_codegen_condition_side_cpu(inputSymbol, IcompareCode)+")";
        if (Icond && Scond)
          cond = cond + "&&(" + transition_to_string_codegen_cpu(IvarID) +  transition_to_string_codegen_condition_side_cpu(inputVar, ScompareCode)+")";
        else if ( !Icond && Scond)
          cond = cond + "(" + transition_to_string_codegen_cpu(IvarID) + transition_to_string_codegen_condition_side_cpu(inputVar, ScompareCode)+")";
          cond = cond +"){\n" ;

        // output
        if ( transition_to_string_codegen_cpu (outputID) != "None"){
          cond = cond + transition_to_string_codegen_output_cpu( outputID, outputSymbol);
        }
        if ( transition_to_string_codegen_cpu (OvarID) != "None"){
          cond = cond + transition_to_string_codegen_stack_output_cpu( OvarID, outputVar);
        }
        cond = cond + "\t\t\t\tcurrentState = "+ std::to_string(nextState) +";\n";
        cond = cond +"\t\t\t}\n" ;
      }
      if (( inputSymbol ==ANY_MATCH) || ( inputID != EPSILON_MATCH))
        cond = cond + "\t\t\tprocessed_"+std::to_string(inputID-(INPUTSTART)) +"++;\n";
    }
  return cond;
}


std::string CODE_BASE_FST::transition_to_string_codegen_cpu( uint32_t alphabet){
  std::string code;

  if ( alphabet < ALPHABETSIZE)  code =   std::to_string(alphabet);
  else if ( (alphabet >= ALPHABETSIZE)  && ( alphabet < EPSILON_MATCH) ){
    code =   std::to_string (uint32_t ( alphabet - ALPHABETSIZE)  );
    code = "!" + code;
  }
  else if (   alphabet  == EPSILON_MATCH) {
    code = "None";
  }
  else if ( ( alphabet  >= VARSTART ) && (alphabet  < (VARSTART + MAXVAR ) ) ) {
    uint32_t dst = alphabet - uint32_t (VARSTART);
    if ( mem_fst->stackMap[dst] == 1){
      code =  "stack_" + std::to_string(mem_fst->IdMap[dst]) +"[var_" + std::to_string(dst) +"-1]";  
    }
    else{
      code = "var_"+ std::to_string( alphabet - uint32_t (VARSTART)) ;
    }
  }
  else if ( ( alphabet  >= INPUTSTART)  && (alphabet  < (INPUTSTART + MAXVAR) )  ) {
    std::string inputID = std::to_string(alphabet-uint32_t (INPUTSTART));
    code = "input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"]" ;
  }
  else if (   alphabet  == ANY_MATCH) {
    code = "any";
  }
  else if ( ( alphabet  >= OUTPUTSTART) && (alphabet  < (OUTPUTSTART + MAXVAR) )  ){
    code = "output[tID][0]" ;
  }
  else if ( ( alphabet  >=  (VARSTART + ALPHABETSIZE) )  && ( alphabet < ( VARSTART + ALPHABETSIZE + MAXVAR)) ) { 
    uint32_t dst = alphabet - uint32_t (VARSTART+ ALPHABETSIZE);
    if ( mem_fst->stackMap[dst] == 1){
      code =  "!stack_"+ std::to_string(mem_fst->IdMap[dst]) +"[var_" + std::to_string(dst) +"-1]"; 
    }
    else{
      code = "!var_"+ std::to_string( alphabet -uint32_t( VARSTART) - uint32_t(ALPHABETSIZE)) ;
    }
  }
  else if  ( (alphabet  >= (INPUTSTART + ALPHABETSIZE) ) && (alphabet < (INPUTSTART + MAXVAR + ALPHABETSIZE) )  ) { 
    std::string inputID = std::to_string(alphabet-uint32_t (INPUTSTART + ALPHABETSIZE));
    code = "!input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"]" ;
  }
  return code;
}

std::string CODE_BASE_FST::transition_to_string_codegen_condition_side_cpu( uint32_t alphabet, uint32_t cond){
  std::string code;
  std::string cond_s = " == ";
  if ( cond == 1) cond_s = " < ";
  else if ( cond ==2) cond_s = " > ";

  if ( alphabet < ALPHABETSIZE)  code =   cond_s + std::to_string(alphabet);
  else if ( (alphabet >= ALPHABETSIZE)  && ( alphabet < EPSILON_MATCH) ){
    code =   std::to_string (uint32_t ( alphabet - ALPHABETSIZE)  );
    code = "!=" + code;
  }
  else if (   alphabet  == EPSILON_MATCH) {
    code = "None";
  }
  else if ( ( alphabet  >= VARSTART ) && (alphabet  < (VARSTART + MAXVAR ) ) ) {
    uint32_t dst = alphabet - uint32_t (VARSTART);
    if ( mem_fst->stackMap[dst] == 1){
      code =  cond_s + "stack_"  + std::to_string(mem_fst->IdMap[dst]) +"[var_" + std::to_string(dst) +"-1]";
    }
    else{
      code = cond_s + "var_"+ std::to_string( alphabet - uint32_t (VARSTART)) ;
    }
  }
  else if ( ( alphabet  >= INPUTSTART)  && (alphabet  < (INPUTSTART + MAXVAR) )  ) {
    std::string inputID = std::to_string(alphabet-uint32_t (INPUTSTART));
    code =cond_s +  "input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"]" ;
  }
  else if (   alphabet  == ANY_MATCH) {
    code = "any";
  }
  else if ( ( alphabet  >= OUTPUTSTART) && (alphabet  < (OUTPUTSTART + MAXVAR) )  ){
    code = "output[tID][0]" ;
  }
  else if ( ( alphabet  >=  (VARSTART + ALPHABETSIZE) )  && ( alphabet < ( VARSTART + ALPHABETSIZE + MAXVAR)) ) {
    uint32_t dst = alphabet - uint32_t (VARSTART+ ALPHABETSIZE);
    if ( mem_fst->stackMap[dst] == 1){
      code =  "!stack_"+  std::to_string(mem_fst->IdMap[dst]) +"[var_" + std::to_string(dst) +"-1]";
    }
    else{
      code = "!= var_"+ std::to_string( alphabet -uint32_t( VARSTART) - uint32_t(ALPHABETSIZE)) ;
    }
  }
  else if  ( (alphabet  >= (INPUTSTART + ALPHABETSIZE) ) && (alphabet < (INPUTSTART + MAXVAR + ALPHABETSIZE) )  ) {
    std::string inputID = std::to_string(alphabet-uint32_t (INPUTSTART + ALPHABETSIZE));
    code = "!= input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"]" ;
  }
  return code;
}

std::string CODE_BASE_FST::transition_to_string_codegen_output_cpu( uint32_t lhs, uint32_t rhs){
  std::string code = "";
  std::string outputID = std::to_string(lhs - uint32_t (INPUTSTART + ALPHABETSIZE));

  if ( rhs < ALPHABETSIZE) {
    code = code + "#ifdef CPU_DEBUG\n";
    code = code + "\t\t\t\tdebug_output[printed] = " + std::to_string(rhs) + ";\n";
    code = code + "\t\t\t\tprinted++;\n";
    code = code + "#else \n";
    code = code + "\t\t\t\toutput[tID][0]= " + std::to_string(rhs) +";\n";
    code = code + "#endif\n";
  }
  else if ( (rhs >= ALPHABETSIZE)  && ( rhs < EPSILON_MATCH) ){
  }
  else if (   rhs  == EPSILON_MATCH) {
  }
  else if ( ( rhs  >= VARSTART ) && ( rhs  < (VARSTART + MAXVAR ) ) ) {
    uint32_t target_stackID  = rhs - 2*ALPHABETSIZE -1;
    if ( mem_fst->stackMap[target_stackID] == 1){
      code = code + "\t\t\t\tfor( uint32_t i = 0; i < var_"+ std::to_string(target_stackID) +"; i++){\n";
      code = code + "#ifdef CPU_DEBUG\n";
      code = code + "\t\t\t\tdebug_output[printed] = stack_" + std::to_string(mem_fst->IdMap[target_stackID]) +"[i];\n";
      code = code + "\t\t\t\tprinted++;\n";
      code = code + "#else \n";
      code = code + "\t\t\t\toutput[tID][0] = stack_"+  std::to_string(mem_fst->IdMap[target_stackID]) +"[i];\n";
      code = code + "#endif\n";
      code = code + "\t\t\t\t}\n";
    }
    else{
      code = code + "#ifdef CPU_DEBUG\n";
      code = code + "\t\t\tdebug_output[printed] = var_"+ std::to_string(target_stackID) +";\n";
      code = code + "\t\t\tprinted++;\n";
      code = code + "#else \n";
      code = code + "\t\t\toutput[tID][0] = var_"+ std::to_string(target_stackID) +";\n";
      code = code + "#endif\n";
    }
  }
  else if ( ( rhs  >= INPUTSTART)  && (rhs  < (INPUTSTART + MAXVAR) )  ) {
    std::string inputID = std::to_string(rhs - uint32_t (INPUTSTART));
    code = code + "#ifdef CPU_DEBUG\n";
    code = code + "\t\t\t\tdebug_output[printed] =input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
    code = code + "\t\t\t\tprinted++;\n";
    code = code + "#else \n";
    code = code + "\t\t\t\toutput[tID][0]=input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
    code = code + "#endif\n";
  }
  else if (   rhs  == ANY_MATCH) {
  }
  else if ( ( rhs  >= OUTPUTSTART) && ( rhs < (OUTPUTSTART + MAXVAR) )  ){
  }
  else if ( ( rhs  >=  (VARSTART + ALPHABETSIZE) )  && ( rhs < ( VARSTART + ALPHABETSIZE + MAXVAR)) ) {
  }
  else if  ( (rhs  >= (INPUTSTART + ALPHABETSIZE) ) && (rhs < (INPUTSTART + MAXVAR + ALPHABETSIZE) )  ) {
  }
  return code;


}

std::string CODE_BASE_FST::transition_to_string_codegen_stack_output_cpu ( uint32_t lhs, uint32_t rhs){
  std::string code ="";
  std::string stackID = std::to_string(mem_fst->IdMap[lhs - uint32_t (2*ALPHABETSIZE) -1] );
  uint32_t stackID_i = mem_fst->IdMap[lhs - uint32_t (2*ALPHABETSIZE) -1] ;
  std::string varID = std::to_string(lhs - uint32_t (2*ALPHABETSIZE) -1);
  uint32_t varID_i = lhs - uint32_t (2*ALPHABETSIZE) -1;

  if ( rhs < ALPHABETSIZE) {
    if ( mem_fst->stackMap[varID_i] == 1){
      code = code + "\t\t\t\tstack_" + stackID +"[var_" + varID + "-1]";
      code = code + "= " + std::to_string(rhs) + ";\n";
      code = code + "\t\t\t\tvar_" + varID+"++;\n";
    }
    else {
      code = code + "\t\t\t\tvar_"  + varID + "= " + std::to_string(rhs) + ";\n";
    }
  }
  else if ( (rhs >= ALPHABETSIZE)  && ( rhs < EPSILON_MATCH) ){
  }
  else if (   rhs  == EPSILON_MATCH) {
  }
  else if ( ( rhs  >= VARSTART ) && ( rhs  < (VARSTART + MAXVAR ) ) ) {
    uint32_t target_stackID  = rhs - 2*ALPHABETSIZE -1;
    if ( mem_fst->stackMap[varID_i] == 1){
      if ( mem_fst->stackMap[target_stackID] == 1){
        code = code + "\t\t\t\tstack_"+  stackID +"[var_" + varID + "-1]";
        code = code + "= stack_" + std::to_string(mem_fst->IdMap[target_stackID]) +"[var_" + std::to_string(mem_fst->IdMap[target_stackID]) + "-1];";
        code = code + "\t\t\t\tvar_" + varID+"++;\n";
      }
      else{
        code = code + "\t\t\t\tstack_" + stackID +"[var_" + varID + "-1]";
        code = code + "= var_" + std::to_string(target_stackID)+";";
        code = code + "\t\t\t\tvar_" + varID+"++;\n";
      }
    }
    else{
      if ( mem_fst->stackMap[target_stackID] == 1){
        code = code + "\t\t\t\tvar_" + varID;
        code = code + "= stack_" + std::to_string(mem_fst->IdMap[target_stackID]) +"[var_" + std::to_string(mem_fst->IdMap[target_stackID]) + "-1];";
      }
      else{
        code = code + "\t\t\t\tvar_" + varID + "= var_" + std::to_string(target_stackID)+";";
      }

    }
  }
  else if ( ( rhs  >= INPUTSTART)  && (rhs  < (INPUTSTART + MAXVAR) )  ) {
    std::string inputID = std::to_string(rhs - uint32_t (INPUTSTART));
    if ( mem_fst->stackMap[varID_i] == 1){
      code = code + "\t\t\t\tstack_" + stackID +"[var_" + varID + "-1]";
      code = code + "=input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
      code = code + "\t\t\t\tvar_" + varID+"++;\n";
    }
    else{
      code = code + "\t\t\t\tvar_"+ varID ;
      code = code + "=input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
    }
  }
  else if (   rhs  == ANY_PUSH) {
    code = code + "\t\t\t\tvar_" + varID+"=1;\n";
    code = code + "\t\t\t\tstack_" +stackID +"[0]";

    code = code + "=0;\n" ;
  }
  return code;


}


