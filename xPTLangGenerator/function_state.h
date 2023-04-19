#pragma once
#include "processor.h"
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

class FUNCTION {
  public:
    uint32_t stateID;
    uint32_t number_of_tx;
    uint32_t * txID;
    uint32_t * nextFunction;

    uint32_t number_of_prev_tx;
    uint32_t * prevFunction;

    bool conditional_input;
    bool conditional_stack;
};

class MERGE_FUNCTION {
  public:
    uint32_t m_ID;
    uint32_t number_of_function;
    uint32_t * functionID;

    uint32_t number_of_tx;
    uint32_t * nextFunction;

};

class CODE_BASE_FST {
  public: 
    int test;
    std::string test_name;
    std::string meta_data;
    std::string function_file;
    std::string baseline_file;
    std:: string global_function_name;
    std::string host_function_name;
    std::string c_mode_s; 
  
    uint32_t number_of_var; 
    uint32_t number_of_stack; 
    uint32_t number_of_function; 
    FUNCTION * functionList;
    MERGE_FUNCTION * merge_functionList; 
    uint32_t number_of_merge_function;
    TP * mem_fst;
  
    CODE_BASE_FST (TP * memory_based_fst, uint32_t c_mode);
    void generate_function_table();
    void generate_function_content();
    void generate_function_previous();
    void calculate_merge_function();
    void connect_merge_function();
    void set_output( int test);  
    uint32_t find_function ( uint32_t id);

    std::string variable_to_value(uint32_t id);
    std::string action_to_string(uint32_t id);
    std::string transition_to_string( uint32_t alphabet); 
    std::string transition_to_string_codegen( uint32_t alphabet); 
    std::string transition_to_string_codegen_output( uint32_t lhs, uint32_t rhs);
    std::string transition_to_string_codegen_stack_output( uint32_t lhs, uint32_t rhs);
    std::string transition_to_string_codegen_condition_side( uint32_t alphabet, uint32_t cond); 

    void function_type_set();
    bool is_conditional_input(uint32_t id);
    bool is_conditional_stack(uint32_t id);
    void export_to_intermediate_form ();
    void print_intermediate_form();
    void to_DOT(std::string dst);

    void write_datastruct();
    void write_global_function();
    void write_host_function();
    void write_state_function();
    void write_group_function();
    void print_mergeFunction_info();
    void write_report();
    std::string write_condition(uint32_t id);
    std::string write_group_condition(uint32_t id);
    std::string write_group_member_condition(uint32_t id);
    void baseline_gen();
    void merge_function_gen();

//  cpu code gen
    void cpu_function_gen();
    void cpu_write_datastruct();
    void cpu_write_host_function();
    void cpu_write_global_function();
    void write_group_function_cpu();
    std::string action_to_string_cpu(uint32_t id);
    std::string variable_to_value_cpu(uint32_t id);
    std::string write_group_condition_cpu(uint32_t id);
    std::string transition_to_string_codegen_cpu( uint32_t alphabet); 
    std::string transition_to_string_codegen_condition_side_cpu( uint32_t alphabet, uint32_t cond); 
    std::string transition_to_string_codegen_output_cpu( uint32_t lhs, uint32_t rhs);
    std::string write_group_member_condition_cpu(uint32_t id);
    std::string transition_to_string_codegen_stack_output_cpu( uint32_t lhs, uint32_t rhs);
};


