#include "function_state.h"

void CODE_BASE_FST::baseline_gen(){
  std::ofstream baseline_code ( baseline_file, std::ios::out); 
 
  write_datastruct();
  write_global_function();
  write_host_function();


}



