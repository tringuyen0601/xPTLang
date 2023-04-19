#include "function_state.h"

CODE_BASE_FST::CODE_BASE_FST (TP * memory_base_fst, uint32_t c_mode){
  mem_fst = memory_base_fst;
  number_of_function = mem_fst->stateCount;
  number_of_var      = mem_fst->varCount;
  number_of_stack   = mem_fst->totalStack;
  test = mem_fst->config->benchmark;
  switch(c_mode){
    case 0:
      c_mode_s = "baseline";
      break;
    case 1: 
      c_mode_s  = "function_group";
      break;
    case 2: 
      c_mode_s  = "cpu";
      break;
  }
  set_output(test);
  
  functionList = (FUNCTION *) malloc ( number_of_function * sizeof( FUNCTION));
 
  generate_function_table();
  generate_function_content();
  function_type_set();
  generate_function_previous();
  
  print_intermediate_form();
}

void CODE_BASE_FST::generate_function_table(){

  for ( uint32_t i =0 ; i < number_of_function; i++){
    functionList[i].stateID = i;
    functionList[i].number_of_tx = mem_fst->stateList[i].numberofTransition;
    functionList[i].number_of_prev_tx = 0;
    functionList[i].txID = (uint32_t * ) malloc ( functionList[i].number_of_tx * sizeof( uint32_t));
    functionList[i].nextFunction = (uint32_t * ) malloc ( functionList[i].number_of_tx * sizeof( uint32_t));
    
  }
}


void CODE_BASE_FST::set_output(int test){
  std::string base;
  switch(test){
    case RLE_ENCODE_OPT:
      base            = "./code_base/rl_encode_opt";
      test_name       = "run_length_encoding";
      meta_data       = base + "/rle_encode_meta.txt";
      function_file   = base + "/rle_encode_function.txt";
      baseline_file   = "./rle_" + c_mode_s+".cu";
      global_function_name = "rle_" + c_mode_s + "_global"; 
      host_function_name = "rle_"+ c_mode_s + "_host";
      break;
    case RLE_DECODE_OPT:
      base            = "./code_base/rl_decode_opt";
      test_name       = "run_length_decoding";
      meta_data       = base + "/rl_decode_meta.txt";
      function_file   = base + "/rl_decode_function.txt";
      baseline_file   = "./rld_" + c_mode_s+".cu";
      global_function_name = "rld_" + c_mode_s + "_global"; 
      host_function_name = "rld_"+ c_mode_s + "_host";
      break;
    case BP_ENCODE:
      base            = "./code_base/bp_encode";
      test_name       = "bit_packing_encoding";
      meta_data       = base + "/bp_encode_meta.txt";
      function_file   = base + "/bp_encode_function.txt";
      baseline_file   = "./bpe_" + c_mode_s+".cu";
      global_function_name = "bpe_" + c_mode_s + "_global"; 
      host_function_name = "bpe_"+ c_mode_s + "_host";
      break;
    case BP_DECODE:
      test_name       = "bit_packing_decoding";
      base            = "./code_base/bp_decode";
      meta_data       = base + "/bp_decode_meta.txt";
      function_file   = base + "/bp_decode_function.txt";
      baseline_file   = "./bpd_" + c_mode_s+".cu";
      global_function_name = "bpd_" + c_mode_s + "_global"; 
      host_function_name = "bpd_"+ c_mode_s + "_host";
      break;
    case GV_ENCODE:
      base            = "./code_base/gv_encode";
      test_name       = "variable_length_encoding";
      meta_data       = base + "/gv_encode_meta.txt";
      function_file   = base + "/gv_encode_function.txt";
      baseline_file   = "./gve_" + c_mode_s+".cu";
      global_function_name = "gve_" + c_mode_s + "_global"; 
      host_function_name = "gve_"+ c_mode_s + "_host";
      break;
    case GV_DECODE:
      base            = "./code_base/gv_decode";
      test_name       = "variable_length_decoding";
      meta_data       = base + "/gv_decode_meta.txt";
      function_file   = base + "/gv_decode_function.txt";
      baseline_file   = "./gvd_" + c_mode_s+".cu";
      global_function_name = "gvd_" + c_mode_s + "_global"; 
      host_function_name = "gvd_"+ c_mode_s + "_host";
      break;
    case  COO_CSR:
      base            = "./code_base/coo_csr";
      test_name       = "coo_csr";
      meta_data       = base + "/coo_csr_meta.txt";
      function_file   = base + "/coo_csr_function.txt";
      baseline_file   = "./coo_csr_" + c_mode_s+".cu";
      global_function_name = "coo_csr_" + c_mode_s + "_global"; 
      host_function_name = "coo_csr_"+ c_mode_s + "_host";
      break;
    case DENSE_CSR_PTR:
      base            = "./code_base/dense_csr";
      test_name       = "dense_csr";
      meta_data       = base + "/dense_csr_meta.txt";
      function_file   = base + "/dense_csr_function.txt";
      baseline_file   = "./dense_csr_" + c_mode_s+".cu";
      global_function_name = "dense_csr_" + c_mode_s + "_global"; 
      host_function_name = "dense_csr_"+ c_mode_s + "_host";
      break;
    case CSV_ENC_DET:
      base            = "./code_base/csv_enc";
      test_name       = "csv_enc";
      meta_data       = base + "/csv_enc_meta.txt";
      function_file   = base + "/csv_enc_function.txt";
      baseline_file   = "./csv_enc_" + c_mode_s+".cu";
      global_function_name = "csv_enc_" + c_mode_s + "_global"; 
      host_function_name = "csv_enc_"+ c_mode_s + "_host";
      break;
    case CSV_ENC_INDET:
      base            = "./code_base/csv_raw";
      test_name       = "csv_raw";
      meta_data       = base + "/csv_raw_meta.txt";
      function_file   = base + "/csv_raw_function.txt";
      baseline_file   = "./csv_raw_" + c_mode_s+".cu";
      global_function_name = "csv_raw_" + c_mode_s + "_global"; 
      host_function_name = "csv_raw_"+ c_mode_s + "_host";
      break;
    case HISTOGRAM:
      base            = "./code_base/histogram";
      test_name       = "hist";
      meta_data       = base + "/hist_meta.txt";
      function_file   = base + "/hist_function.txt";
      baseline_file   = "./hist_" + c_mode_s+".cu";
      global_function_name = "hist_" + c_mode_s + "_global"; 
      host_function_name = "hist_"+ c_mode_s + "_host";
      break;
    }
}
void CODE_BASE_FST::generate_function_content(){

  for ( uint32_t i =0 ; i < number_of_function; i++){
    uint32_t base =  mem_fst->stateList[i].baseID ;
    for ( uint32_t j = 0 ; j < functionList[i].number_of_tx; j++){
      functionList[i].txID[j] = base+ j;
      functionList[i].nextFunction[j] = mem_fst->transitionList[base+j].nextState;

    }      
  }
}

void CODE_BASE_FST::generate_function_previous(){
  for ( uint32_t i =0 ; i < number_of_function; i++){
    for ( uint32_t j = 0 ; j < functionList[i].number_of_tx; j++){
      functionList[ functionList[i].nextFunction[j]].number_of_prev_tx++;
    }      
  }


}

void CODE_BASE_FST::print_intermediate_form(){
  std::cout << "============================================"<<std::endl;
  std::cout << "\tBenchmark: " << test_name<< std::endl;
  std::cout << "\tNumber of Variable: " << number_of_var<< std::endl;
  for ( uint32_t i =0 ; i < number_of_function; i++){
    std::cout <<"FUNCTION " << i << std::endl;
    std::cout <<"\tState " << i<<std::endl;
    std::cout <<"\tConditional Input: "<< functionList[i].conditional_input <<std::endl;
    std::cout <<"\tConditional Stack: "<< functionList[i].conditional_stack <<std::endl;
    std::cout <<"\tNumber of Transition: "<< functionList[i].number_of_tx <<std::endl;
    uint32_t base =  mem_fst->stateList[i].baseID ;
    for ( uint32_t j = 0 ; j < functionList[i].number_of_tx; j++){
      std::cout <<"\tTransition: ";
      std::cout <<base + j ;
      std::cout << " to function " << functionList[i].nextFunction[j]<< std::endl;
    }
    std::cout <<"\tNumber of Incoming Transition: "<< functionList[i].number_of_prev_tx <<std::endl;
    std::cout << std::endl;
  }
  
      std::cout << "Output meta data: " << meta_data <<std::endl;
      std::cout << "Output function file: " << function_file << std::endl;
  std::cout << "============================================"<<std::endl;
}

void CODE_BASE_FST::function_type_set(){

  for ( uint32_t i =0 ; i < number_of_function; i++){
    uint32_t base =  mem_fst->stateList[i].baseID ;
    functionList[i].conditional_input = is_conditional_input(i);
    functionList[i].conditional_stack = is_conditional_stack(i);
  }


}

bool CODE_BASE_FST::is_conditional_input(uint32_t id){
  uint32_t txID =  mem_fst->stateList[id].baseID ;
  uint32_t input = mem_fst->transitionList[txID].inputSymbol;
  bool input_condition = true;
  if (( input == EPSILON_MATCH) || ( input == ANY_MATCH) )
    input_condition = false;
  return input_condition;
}

bool CODE_BASE_FST::is_conditional_stack(uint32_t id){
  uint32_t txID =  mem_fst->stateList[id].baseID ;
  uint32_t var = mem_fst->transitionList[txID].inputVar;

  bool var_condition = true;
  if (( var == EPSILON_MATCH) ||(var == ANY_MATCH))
    var_condition = false; 
  return var_condition;
}

void CODE_BASE_FST::export_to_intermediate_form(){
  std::ofstream fmeta ( meta_data, std::ios::out);
  fmeta <<"Function Name: "<< test_name<< std::endl;
  fmeta <<"Number of variable: " << number_of_var << std::endl;
  fmeta <<"Number of function: " << number_of_function <<std::endl;
  fmeta.close();

  std::ofstream ffunction ( function_file, std::ios::out);

  for ( uint32_t i =0 ; i < number_of_function; i++){
    ffunction << "FunctionID: "<< i <<std::endl;
    ffunction << "Input Condition: "<< functionList[i].conditional_input <<std::endl;
    ffunction << "Stack Condition: "<< functionList[i].conditional_stack <<std::endl;
    ffunction << "Outgoing Function: "<< functionList[i].number_of_tx <<std::endl;
    ffunction << "Action: " << action_to_string(i) << std::endl;
    uint32_t base =  mem_fst->stateList[i].baseID ;
    for ( uint32_t j = 0 ; j < functionList[i].number_of_tx; j++){
      ffunction << "Transition: " << std::to_string(base+j) << std::endl;
      uint32_t inputID      = mem_fst->transitionList[base+j].inputID;
      uint32_t inputSymbol  = mem_fst->transitionList[base+j].inputSymbol;
      uint32_t IvarID       = mem_fst->transitionList[base+j].IvarID;
      uint32_t inputVar     = mem_fst->transitionList[base+j].inputVar;
      uint32_t outputID     = mem_fst->transitionList[base+j].outputID;
      uint32_t outputSymbol = mem_fst->transitionList[base+j].outputSymbol;
      uint32_t OvarID       = mem_fst->transitionList[base+j].OvarID;
      uint32_t outputVar    = mem_fst->transitionList[base+j].outputVar;

      ffunction << "\tInput Condition: " << transition_to_string( inputID)  << " == " << transition_to_string(inputSymbol) << std::endl;
      ffunction << "\tVariable Condition: " << transition_to_string( IvarID)  << " == " << transition_to_string(inputVar) << std::endl;
      ffunction << "\tOutput Write: " << transition_to_string( outputID)  << " = " << transition_to_string(outputSymbol) <<std::endl;
      ffunction << "\tVariable Write: " << transition_to_string( OvarID)  << " = " << transition_to_string(outputVar) << std::endl;
      ffunction << "\tNext State: " <<functionList[i].nextFunction[j] <<std::endl;
    }      
    ffunction << std::endl;
  }

  ffunction.close();
}

std::string CODE_BASE_FST::variable_to_value( uint32_t id){
  std::string value ="";
  if ( mem_fst->stackMap[id] == true){
    value =  "stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[id]) +") * STACK_DEPTH + var_" + std::to_string(id) +"-1 ]";
  }
  else{
    value = "var_" + std::to_string(id);
  }
  return value;
}

std::string CODE_BASE_FST::action_to_string( uint32_t id){
  uint8_t opt = mem_fst->stateList[id].opt;
  uint32_t src1 = mem_fst->stateList[id].src1;
  uint32_t src2 = mem_fst->stateList[id].src2;
  uint32_t dst = mem_fst->stateList[id].dst;
  std::string code;
  switch ( opt) {
    case 1: // ADD
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " + " + variable_to_value(src2) +"";
      break;
    case 2: // ADD I
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " + " + std::to_string(src2) +"";
      break;
    case 3: //  SUB
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " - " + variable_to_value(src2) +"";
      break;
    case 4: // SUBI
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " - " + std::to_string(src2) +"";
      break;
    case 5: //  MUL
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " * " + variable_to_value(src2) +"";
      break;
    case 6: // MULI:
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " * " + std::to_string(src2) +"";
      break;
    case 7: // DIV
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " / " + variable_to_value(src2) +"";
      break;
    case 8: //DIVI
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " / " + std::to_string(src2) +"";
      break;
    case 9:
      code = "NO LONGER SUPPORT SET";
      break;
    case 10:
      code = "NO LONGER SUPPORT POP";
      break;
    case 11:  // LSHIFT
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " << " + variable_to_value(src2) +"";
      break;
    case 12:  // LSHIFTI
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " << " + std::to_string(src2) +"";
      break;
    case 13:  // RSHIFT
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " >> " + variable_to_value(src2) +"";
      break;
    case 14:  // RSHIFTI
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " >> " + std::to_string(src2) +"";
      break;
    case 15:  //  OR
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " | " + variable_to_value(src2) +"";
      break;
    case 16:  //  ORI
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " | " + std::to_string(src2) +"";
      break;
    case 17:  //  AND
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " & " + variable_to_value(src2) +"";
      break;
    case 18:  //  ANDI
      code = variable_to_value(dst) +" = " + variable_to_value(src1) + " & " + std::to_string(src2) +"";
      break;
    case 0:
      code = "None";
      break;
  }
  return code;

}

std::string CODE_BASE_FST::transition_to_string( uint32_t alphabet){
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
    code = "var_"+ std::to_string( alphabet - uint32_t (VARSTART)) ;
  }
  else if ( ( alphabet  >= INPUTSTART)  && (alphabet  < (INPUTSTART + MAXVAR) )  ) {
    code = "Input_" +std::to_string(alphabet-uint32_t (INPUTSTART)) ;
  }
  else if (   alphabet  == ANY_MATCH) {
    code = "any";
  }
  else if ( ( alphabet  >= OUTPUTSTART) && (alphabet  < (OUTPUTSTART + MAXVAR) )  ){
    code = "Output_" +std::to_string(alphabet-uint32_t( OUTPUTSTART)) ;
  }
  else if ( ( alphabet  >=  (VARSTART + ALPHABETSIZE) )  && ( alphabet < ( VARSTART + ALPHABETSIZE + MAXVAR)) ) {
    code = "!var_"+ std::to_string( alphabet -uint32_t( VARSTART) - uint32_t(ALPHABETSIZE)) ;
  }
  else if  ( (alphabet  >= (INPUTSTART + ALPHABETSIZE) ) && (alphabet < (INPUTSTART + MAXVAR + ALPHABETSIZE) )  ) {
    code = "!Input_" +std::to_string(alphabet- uint32_t(INPUTSTART) -uint32_t(ALPHABETSIZE)) ;
  }
  else if (   alphabet  == ANY_PUSH) {
    code = "Push";
  }
  return code;
}

std::string CODE_BASE_FST::transition_to_string_codegen( uint32_t alphabet){
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
      code =  "stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[dst]) +") * STACK_DEPTH + var_" + std::to_string(dst) +"-1 ]";
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
    code = "output[globalID][0]" ;
  }
  else if ( ( alphabet  >=  (VARSTART + ALPHABETSIZE) )  && ( alphabet < ( VARSTART + ALPHABETSIZE + MAXVAR)) ) {
    uint32_t dst = alphabet - uint32_t (VARSTART+ ALPHABETSIZE);
    if ( mem_fst->stackMap[dst] == 1){
      code =  "!stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[dst]) +") * STACK_DEPTH + var_" + std::to_string(dst) +"-1 ]";
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

std::string CODE_BASE_FST::transition_to_string_codegen_condition_side( uint32_t alphabet, uint32_t cond){
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
      code =  cond_s + "stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[dst]) +") * STACK_DEPTH + var_" + std::to_string(dst) +"-1 ]";
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
    code = "output[globalID][0]" ;
  }
  else if ( ( alphabet  >=  (VARSTART + ALPHABETSIZE) )  && ( alphabet < ( VARSTART + ALPHABETSIZE + MAXVAR)) ) {
    uint32_t dst = alphabet - uint32_t (VARSTART+ ALPHABETSIZE);
    if ( mem_fst->stackMap[dst] == 1){
      code =  "!stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[dst]) +") * STACK_DEPTH + var_" + std::to_string(dst) +"-1 ]";
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
std::string CODE_BASE_FST::transition_to_string_codegen_output ( uint32_t lhs, uint32_t rhs){
  std::string code = "";
  std::string outputID = std::to_string(lhs - uint32_t (INPUTSTART + ALPHABETSIZE));
//  code = "input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"] = " ;

  if ( rhs < ALPHABETSIZE) {
    code = code + "#ifdef DEBUG\n";
    code = code + "\t\t\t\ttest_output[printed] = " + std::to_string(rhs) + ";\n";
    code = code + "\t\t\t\tprinted++;\n";
    code = code + "#else \n";
    code = code + "\t\t\t\toutput[globalID][0]= " + std::to_string(rhs) +";\n";
    code = code + "#endif\n";
  }
  else if ( (rhs >= ALPHABETSIZE)  && ( rhs < EPSILON_MATCH) ){
  }
  else if (   rhs  == EPSILON_MATCH) {
  }
  else if ( ( rhs  >= VARSTART ) && ( rhs  < (VARSTART + MAXVAR ) ) ) {
    uint32_t target_stackID  = rhs - 2*ALPHABETSIZE -1;
    if ( mem_fst->stackMap[target_stackID] == 1){
      //code = code + "\t\t\t\tuint32_t target_stackID  = rhs - 2*ALPHABETSIZE -1 ;\n";
      //code = code + "\t\t\t\tuint32_t target_stacksize = top_"+ std::to_string(target_stackID) +" -1;\n";
      code = code + "\t\t\t\tfor( uint32_t i = 0; i < var_"+ std::to_string(target_stackID) +"; i++){\n";
      code = code + "#ifdef DEBUG\n";
      code = code + "\t\t\t\ttest_output[printed] = stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[target_stackID]) +") * STACK_DEPTH + i];\n";
      code = code + "\t\t\t\tprinted++;\n";
      code = code + "#else \n";
      code = code + "\t\t\t\toutput[globalID][0] = stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[target_stackID]) +") * STACK_DEPTH + i];\n";
      code = code + "#endif\n";
      code = code + "\t\t\t\t}\n";
    }
    else{
      code = code + "#ifdef DEBUG\n";
      code = code + "\t\t\ttest_output[printed] = var_"+ std::to_string(target_stackID) +";\n";
      code = code + "\t\t\tprinted++;\n";
      code = code + "#else \n";
      code = code + "\t\t\toutput[globalID][0] = var_"+ std::to_string(target_stackID) +";\n";
      code = code + "#endif\n";
    }
  }
  else if ( ( rhs  >= INPUTSTART)  && (rhs  < (INPUTSTART + MAXVAR) )  ) {
    std::string inputID = std::to_string(rhs - uint32_t (INPUTSTART));
    code = code + "#ifdef DEBUG\n";
    code = code + "\t\t\t\ttest_output[printed] =input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
    code = code + "\t\t\t\tprinted++;\n";
    code = code + "#else \n";
    code = code + "\t\t\t\toutput[globalID][0]=input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
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
std::string CODE_BASE_FST::transition_to_string_codegen_stack_output ( uint32_t lhs, uint32_t rhs){
  std::string code ="";
  std::string stackID = std::to_string(mem_fst->IdMap[lhs - uint32_t (2*ALPHABETSIZE) -1] );
  uint32_t stackID_i = mem_fst->IdMap[lhs - uint32_t (2*ALPHABETSIZE) -1] ;
  std::string varID = std::to_string(lhs - uint32_t (2*ALPHABETSIZE) -1);
  uint32_t varID_i = lhs - uint32_t (2*ALPHABETSIZE) -1;
//  code = "input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"] = " ;

  if ( rhs < ALPHABETSIZE) {
    if ( mem_fst->stackMap[varID_i] == 1){
      code = code + "\t\t\t\tstack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + stackID +") * STACK_DEPTH + var_" + varID + "-1]";
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
        code = code + "\t\t\t\tstack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + stackID +") * STACK_DEPTH + var_" + varID + "-1]";
        code = code + "= stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[target_stackID]) +") * STACK_DEPTH + var_" + std::to_string(mem_fst->IdMap[target_stackID]) + "-1];";
        code = code + "\t\t\t\tvar_" + varID+"++;\n";
      }
      else{
        code = code + "\t\t\t\tstack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + stackID +") * STACK_DEPTH + var_" + varID + "-1]";
        code = code + "= var_" + std::to_string(target_stackID)+";";
        code = code + "\t\t\t\tvar_" + varID+"++;\n";
      }
    }
    else{
      if ( mem_fst->stackMap[target_stackID] == 1){
        code = code + "\t\t\t\tvar_" + varID;
        code = code + "= stack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + std::to_string(mem_fst->IdMap[target_stackID]) +") * STACK_DEPTH + var_" + std::to_string(mem_fst->IdMap[target_stackID]) + "-1];";
      }
      else{
        code = code + "\t\t\t\tvar_" + varID + "= var_" + std::to_string(target_stackID)+";";
      }

    }
  }
  else if ( ( rhs  >= INPUTSTART)  && (rhs  < (INPUTSTART + MAXVAR) )  ) {
    std::string inputID = std::to_string(rhs - uint32_t (INPUTSTART));
    if ( mem_fst->stackMap[varID_i] == 1){
      code = code + "\t\t\t\tstack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " + stackID +") * STACK_DEPTH + var_" + varID + "-1]";
      code = code + "=input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
      code = code + "\t\t\t\tvar_" + varID+"++;\n";
    }
    else{
      code = code + "\t\t\t\tvar_"+ varID ;
      code = code + "=input[" + inputID +"][base_" +inputID + " + processed_" +inputID +"];\n" ;
    }
  }
  else if (   rhs  == ANY_PUSH) {
    code = code + "\t\t\t\tvar_" + stackID+"=1;\n";
    code = code + "\t\t\t\tstack[(threadIdx.x * "+ std::to_string(number_of_stack)+ " + " +stackID +") * STACK_DEPTH ]";

    code = code + "=0;\n" ;
  }
  return code;


}

