#pragma once

#include <string>
#include "AST_NODES.h"

AST_NODE_PROGRAM * program(); //done

AST_NODE_FORWARD_DECLARE * forward_declare(); //done
AST_NODE_IMPLEMENTATIONS * implementation(); //done

AST_NODE_FUNCTION_PROTO * function_proto(); //done
AST_NODE_FUNCTION * function(); //done

AST_NODE_PARAMETER_LIST * parameter_list();
AST_NODE_VARIBALE_LIST * variable_list(); //done
AST_NODE_DEFINITION * definition(); //done
AST_NODE_TYPE * type(); //done


AST_NODE_STATEMENT * statement(); //done

//types of statements
AST_NODE_ASSIGNMENT * assignment(); //done
AST_NODE_STATEMENT * ctrl_statement(); //done
AST_NODE_STATEMENT *io_statement(); //done
AST_NODE_FUNCTION_CALL *function_call();
AST_NODE_RETURN * return_statement(); //done


//assignment statements and arithmetic
AST_NODE_EXPR * expr(); //done
AST_NODE_EXPR * term(); //done
AST_NODE_EXPR * factor(); //done
AST_NODE_EXPR * primary(); //done
AST_NODE_ID * id(); //done

//Control statements
AST_NODE_IF_STATEMENT * if_statement(); //done
AST_NODE_STATEMENT * loop(); //done

//loops
AST_NODE_FOR_LOOP * for_loop(); //done
AST_NODE_WHILE_LOOP * while_loop(); //done


void error(std::string message);

bool isStatement();

int convertType(int parsedType);

void determineOutput(SCL_TYPE output);