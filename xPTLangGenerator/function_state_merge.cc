#include "function_state.h"

bool belong_to_list( uint32_t item, uint32_t * list, uint32_t listsize){
  bool belong = false;
  for ( uint32_t i = 0; i < listsize; i++){
    if (  list[i] == item){ 
      belong = true;
      break;
    }
  }
  return belong;
}

uint32_t CODE_BASE_FST:: find_function ( uint32_t id){
  for ( uint32_t i = 0 ; i < number_of_merge_function; i++){
    if ( merge_functionList[i].functionID[0] == id) return i;
  }
  return number_of_function;
}

void CODE_BASE_FST::calculate_merge_function(){
  
  uint32_t  * stateList = (uint32_t *) malloc(  number_of_function * sizeof( uint32_t));
  uint32_t  * stateListA = (uint32_t *) malloc(  number_of_function * sizeof( uint32_t));
  uint32_t  * stateListB = (uint32_t *) malloc(  number_of_function * sizeof( uint32_t));
  for (uint32_t i = 0; i < number_of_function; i++){
    stateList[i] = i;
    stateListA[i] = number_of_function;
    stateListB[i] = number_of_function;
  }
  uint32_t currentA = 0;
  uint32_t currentB = 0;
  // generate 3 list of function 
  for (uint32_t i = 0; i < number_of_function; i++){
  std::cout <<"Consider state: " << i << ": ";
    if ( stateList[i] == number_of_function){
      std::cout <<" Already in list" << std::endl;
      continue;
    }

    // list of state with multiple outgoing 
    if ( functionList[i].number_of_tx > 1){
      std::cout <<"multiple outgoing tx" << std::endl;
      stateListB[currentB] = i; 
      currentB++;
      stateList[i] = number_of_function;
      for ( uint32_t j =0 ; j <functionList[i].number_of_tx; j++){ 
        // check if element is already in list
        bool add = true;
        for ( uint32_t k = 0; k < currentB; k++){
          if (  functionList[i].nextFunction[j] == stateListB[k]) 
            add = false;
        }

        if ( add == true){
          stateListB[currentB] = functionList[i].nextFunction[j]; 
          currentB++;
          stateList[ functionList[i].nextFunction[j]] = number_of_function;
        }
        add = true;
      }
      continue;
    }  

    // list of state with : start state or multiple entry: possible entry
    if (functionList[i].stateID == 0){
      std::cout <<"initial state" << std::endl;
      stateListB[currentB] = i; 
      currentB++;
      stateList[i] = number_of_function;
      continue;
      }

    if (functionList[i].number_of_prev_tx >1){
      std::cout <<"multiple incoming tx" << std::endl;
      stateListB[currentB] = i; 
      currentB++;
      stateList[i] = number_of_function;
      continue;
      }

    // the rest go into a list
    std::cout <<" member state, move to list A" << std::endl;
    stateListA[currentA] = i; 
    currentA++;
    stateList[i] = number_of_function;
  }
  
  std::cout << "List A: " ;
  for (uint32_t i = 0; i < currentA; i++){
    std::cout << stateListA[i] << ", ";
  }
  std::cout << std::endl;
  std::cout << "List B: " ;
  for (uint32_t i = 0; i < currentB; i++){
    std::cout << stateListB[i] << ", ";
  }
  std::cout << std::endl;

// allocate function group
  merge_functionList = (MERGE_FUNCTION *) malloc ( number_of_function * sizeof( FUNCTION));
  number_of_merge_function = 0;
  for ( uint32_t i =0; i < number_of_function ; i++){
    merge_functionList[i].m_ID = i;
    merge_functionList[i].number_of_function = 0;
    merge_functionList[i].functionID = ( uint32_t *) malloc( number_of_function * sizeof(uint32_t));
  }

// calculate function group to traversing B list
  std::cout <<"Generating group function" << std::endl;
  std::cout <<"Consider List B: " << std::endl;
  for ( uint32_t i =0; i < currentB; i++){
    std::cout <<"Function: "<< number_of_merge_function  << std::endl;
    std::cout <<"First state: "<< stateListB[i] <<" " ;

    uint32_t currentMember = 0;
    merge_functionList[number_of_merge_function].functionID[ currentMember]  = stateListB[i];
    currentMember++;
 
    // conditional state 
    if ( functionList[stateListB[i]].number_of_tx >1){
      std::cout <<" conditional state"<< std::endl;
      merge_functionList[number_of_merge_function].number_of_function = currentMember;
      number_of_merge_function++;
      continue;
    }
  
    // non-conditional state
    std::cout <<"Unconditional state"<< std::endl;
    uint32_t nextState = functionList[stateListB[i]].nextFunction[0];
    bool reached = false;
    while ( !reached){
      // add state until reach a state in list B
      if ( (!belong_to_list( nextState, stateListB,currentB))&& ( belong_to_list( nextState, stateListA,currentA))){
        std::cout <<" adding state " << nextState<< " to function "<< number_of_merge_function << std::endl;
        merge_functionList[number_of_merge_function].functionID[ currentMember]  = nextState;
        currentMember++;
        // remove added state from listA
        for ( uint32_t j = 0; j < currentA; j++){
          if ( nextState == stateListA[j]) {
            std::cout << "Remove A["<< j <<"]= " << stateListA[j] << std::endl; 
            stateListA[j] = number_of_function;
          }
        }
        nextState = functionList[nextState].nextFunction[0]; 
      }
      else {
        std::cout <<"state " << nextState<< " already added or belong to list B "<< std::endl;
        reached = true;
      }
    }
    merge_functionList[number_of_merge_function].number_of_function = currentMember;
    number_of_merge_function++;
    currentMember = 0;
  }

  std::cout <<"Consider List A: " ;
  for ( uint32_t i =0; i < currentA; i++){
    std::cout << stateListA[i] <<"," ; 
    if (stateListA[i] == number_of_function) continue;
    std::cout << std::endl;
    std::cout <<"Function: "<< number_of_merge_function  << std::endl;
    std::cout <<"First state: "<< stateListA[i] <<" " ;
    std::cout << "Remove A["<< i <<"]= " << stateListA[i] << std::endl; 

    uint32_t currentMember = 0;
    merge_functionList[number_of_merge_function].functionID[ currentMember]  = stateListA[i];
    currentMember++;
    uint32_t nextState = functionList[stateListA[i]].nextFunction[0];
    bool reached = false;
    while ( !reached){
      // add state until reach a state in list B
      if ( (!belong_to_list( nextState, stateListB,currentB))&& ( belong_to_list( nextState, stateListA,currentA))){
        std::cout <<" adding state " << nextState<< " to function "<< number_of_merge_function << std::endl;
        merge_functionList[number_of_merge_function].functionID[ currentMember]  = nextState;
        currentMember++;
        // remove added state from listA
        for ( uint32_t j = 0; j < currentA; j++){
          if ( nextState == stateListA[j]) stateListA[j] = number_of_function;
        }
        nextState = functionList[nextState].nextFunction[0]; 
      }
      else {
        std::cout <<"state " << nextState<< " already added or belong to list B "<< std::endl;
        reached = true;
      }
    }
    stateListA[i] = number_of_function;
    merge_functionList[number_of_merge_function].number_of_function = currentMember;
    number_of_merge_function++;
    std::cout << "Function " << i << ", " << currentMember << " states" << std::endl;
    currentMember = 0;
  }

// connecting state
  for ( uint32_t i = 0 ; i < number_of_merge_function; i++){
    std::cout << "Function " << i << ": ";
    for ( uint32_t j =0; j<  merge_functionList[i].number_of_function; j++) {
      std:: cout << merge_functionList[i].functionID[j]<< ", ";
    }
    std::cout << std::endl;
  }
}  


void CODE_BASE_FST::connect_merge_function(){

  for ( uint32_t i =0; i < number_of_merge_function; i++){
    std::cout << "Group Function " << i << ": ";
    for ( uint32_t j =0; j<  merge_functionList[i].number_of_function; j++) {
      std:: cout << merge_functionList[i].functionID[j]<< ", ";
    }
    std::cout << std::endl;
    uint32_t lastState = merge_functionList[i].functionID[ merge_functionList[i].number_of_function -1];
    uint32_t number_of_next_function = functionList[lastState].number_of_tx;
    std::cout<< "Last function: " << lastState <<" with " << number_of_next_function <<" outgoing function"<< std::endl;

    merge_functionList[i].number_of_tx = number_of_next_function;
    merge_functionList[i].nextFunction = (uint32_t *) malloc( number_of_next_function * sizeof( uint32_t));
    for ( uint32_t j = 0 ; j < number_of_next_function; j++){
      uint32_t ID = find_function ( functionList[lastState].nextFunction[j]); 
      if ( ID == number_of_function) {
        std::cout <<" Cannot find function group with function " <<  functionList[lastState].nextFunction[j] << std::endl; 
        exit(0);
      }
      merge_functionList[i].nextFunction[j]= ID;
      std:: cout << "connect to group  Function: "<< ID << std::endl; 
    }
  }
}

void CODE_BASE_FST::to_DOT( std::string dst){
  std::ofstream fout ( dst, std::ios::out );
  fout << "digraph \"graph\" { "<<std::endl;
  fout << "\trankdir=LR;"<< std::endl;

//// create states////
  fout <<"\tsubgraph statemachine {"<< std::endl;

  //std::cout << " Write State " <<std::endl;
  for ( uint32_t i = 0; i < number_of_merge_function; i++){
    fout << "N" << merge_functionList[i].m_ID ;
    fout <<"\t\t[shape = circle," <<std::endl;
    fout <<"label=<ID:"<< merge_functionList[i].m_ID << " | ";
    for ( uint32_t j =0; j<  merge_functionList[i].number_of_function; j++) {
      fout<< merge_functionList[i].functionID[j]<< ", ";
    }
    fout<<  " >";
    fout <<"\t\tcolor=\"black\"];" <<std::endl;
  }
  for ( uint32_t i = 0; i < number_of_merge_function; i++){
    for ( uint32_t j = 0; j < merge_functionList[i].number_of_tx; j++){
      fout << "\tN" << merge_functionList[i].m_ID <<" -> N" << merge_functionList[i].nextFunction[j] ;       
      fout << "[color=\"black\"];" << std::endl;
    }
  }
  fout <<"\t}" << std::endl;

  fout << "\t}"<<std::endl;
  fout.close();
}

void CODE_BASE_FST::merge_function_gen(){
  std::ofstream baseline_code ( baseline_file, std::ios::out);

  write_datastruct();
  write_global_function(); 
  write_host_function();
}

void CODE_BASE_FST::write_group_function(){
  std::ofstream ptr ( baseline_file, std::ios::app);
  for ( uint32_t i = 0; i < number_of_merge_function;i++){
    if (i ==0) ptr<< "\t\tif(currentState == 0){" << std::endl;
    else ptr<<"\t\telse if (currentState ==" << i << "){"<< std::endl;

    std::cout <<"generate code for function group " << i << std::endl;
    if ( merge_functionList[i].number_of_function >1){
      for ( uint32_t j = 0; j <  (merge_functionList[i].number_of_function-1) ;j++){
        std::cout <<"Member function " << j << std::endl;
        std::string action = action_to_string( merge_functionList[i].functionID[j]);
        if ( action !="None")
          ptr<<"\t\t\t" << action <<";" << std::endl;
        ptr << write_group_member_condition( merge_functionList[i].functionID[j]);
      }
      std::cout <<"Member function " << merge_functionList[i].number_of_function-1 << std::endl;
      std::string action = action_to_string( merge_functionList[i].functionID[merge_functionList[i].number_of_function-1]);
      if ( action !="None")
        ptr<<"\t\t\t" << action <<";" << std::endl;
      ptr << write_group_condition( merge_functionList[i].functionID[merge_functionList[i].number_of_function-1]);

    }
    else{
      std::string action = action_to_string( merge_functionList[i].functionID[0]);
      if ( action !="None")
        ptr<<"\t\t\t" << action <<";" << std::endl;
      ptr << write_group_condition( merge_functionList[i].functionID[0]);
    }
    ptr<<"\t\t}" << std::endl;
  }
  ptr.close();
}

void CODE_BASE_FST::print_mergeFunction_info(){
  uint32_t number_of_transition = 0;
  std::cout <<"==================================================" << std::endl;
  for ( uint32_t i = 0 ; i < number_of_merge_function; i++){
    std::cout << "Function " << i << ": ";
    for ( uint32_t j =0; j<  merge_functionList[i].number_of_function; j++) {
      std:: cout << merge_functionList[i].functionID[j]<< ", ";
    }
    std::cout << std::endl;
    number_of_transition = number_of_transition + merge_functionList[i].number_of_tx;
    std::cout << "Next Function: " ;
    for ( uint32_t j = 0; j < merge_functionList[i].number_of_tx; j++){
      std:: cout << merge_functionList[i].nextFunction[j]<< ", ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout <<"Number of Function: " << number_of_merge_function<< std::endl;
  std::cout <<"Number of Transition: " << number_of_transition << std::endl;
  std::cout <<"==================================================" << std::endl;



}

void CODE_BASE_FST::write_report(){
  std::string report_file = "./report/" + test_name + "_report.txt";
  std::ofstream ptr ( report_file, std::ios::out);

  uint32_t number_of_transition = 0;
  ptr <<"====================="<<test_name  <<"Report ========================" << std::endl;
  for ( uint32_t i = 0 ; i < number_of_merge_function; i++){
    ptr << "Function " << i << ": ";
    for ( uint32_t j =0; j<  merge_functionList[i].number_of_function; j++) {
      ptr << merge_functionList[i].functionID[j]<< ", ";
    }
    ptr << std::endl;
    number_of_transition = number_of_transition + merge_functionList[i].number_of_tx;
    ptr << "Next Function: " ;
    for ( uint32_t j = 0; j < merge_functionList[i].number_of_tx; j++){
      ptr << merge_functionList[i].nextFunction[j]<< ", ";
    }
    ptr << std::endl;
  }
  ptr << std::endl;
  ptr <<"Topology Reduction "<< std::endl;
  ptr <<"Number of State: " << mem_fst->stateCount<< std::endl;
  ptr <<"Number of Transition: " << mem_fst->transitionCount << std::endl;
  ptr << std::endl;

  ptr <<"Block Coalescing "<< std::endl;
  ptr <<"Number of State: " << number_of_merge_function<< std::endl;
  ptr <<"Number of Transition: " << number_of_transition << std::endl;
  ptr <<"==================================================" << std::endl;



}
