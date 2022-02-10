#pragma once

#include <string>
#include "AST_NODES.h"

AST_NODE_PROGRAM * program(); 

AST_NODE_FORWARD_DECLARE * forward_declare();
AST_NODE_IMPLEMENTATIONS * implementation(); 

AST_NODE_FUNCTION_PROTO * function_proto(); 
AST_NODE_FUNCTION * function(); 

AST_NODE_PARAMETER_LIST * parameter_list();
AST_NODE_VARIBALE_LIST * variable_list();
AST_NODE_DEFINITION * definition(); 
AST_NODE_TYPE * type(); 


AST_NODE_STATEMENT * statement(); 


AST_NODE_ASSIGNMENT * assignment();
AST_NODE_STATEMENT * ctrl_statement();
AST_NODE_STATEMENT *io_statement(); 
AST_NODE_FUNCTION_CALL *function_call();
AST_NODE_RETURN * return_statement();


AST_NODE_EXPR * expr();
AST_NODE_EXPR * term();
AST_NODE_EXPR * factor();
AST_NODE_EXPR * primary();
AST_NODE_ID * id();


AST_NODE_IF_STATEMENT * if_statement(); 
AST_NODE_STATEMENT * loop(); 


AST_NODE_FOR_LOOP * for_loop();
AST_NODE_WHILE_LOOP * while_loop(); 


void error(std::string message);

bool isStatement();

int convertType(int parsedType);

void determineOutput(SCL_TYPE output);
