#pragma once

#include <vector>
#include <string>
#include <map>

#define TYPE_INTEGER 1
#define TYPE_FLOAT 2
#define TYPE_BOOLEAN 3
#define TYPE_STRING 4
#define TYPE_VOID 5


#define SCLError(message) (throw SCLException(__FILE__, __LINE__, message))

struct SCL_TYPE{
    int intValue;
    float floatValue;
    bool boolValue;
    std::string stringValue;
    int actualType;

    SCL_TYPE();
};

class SCLException: public std::exception {
public:
std::string file;
int line;
std::string message;
SCLException(std::string file, int line, std::string message);
};

class SCLReturned {
    public:
    SCL_TYPE value;

    SCLReturned(SCL_TYPE value);
};

class AST_NODE_FUNCTION;
class Environment{
    public:
    std::map<std::string, SCL_TYPE> variableTable;
    std::map<std::string, AST_NODE_FUNCTION *> functionTable;

    Environment(std::map<std::string, SCL_TYPE> variableTable, std::map<std::string, AST_NODE_FUNCTION *> functionTable);

    SCL_TYPE vLookUpID(std::string key);
    void vUpdateID(std::string key, SCL_TYPE value);
    void vAddKey(std::string key, SCL_TYPE empty_value);
    
    AST_NODE_FUNCTION * fLookUpID(std::string key);
    void populateFunctionTable(std::vector<AST_NODE_FUNCTION *> functions);
    
};

class AST_NODE_ID {
    public:
    std::string varibale_name;
    AST_NODE_ID(std::string varibale_name);
    //virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_EXPR {
    public:
    virtual SCL_TYPE evaluate(Environment * environment) = 0;
    
};

class AST_NODE_VARIABLE :public AST_NODE_EXPR {
    public:
    AST_NODE_ID * identifier;
    AST_NODE_VARIABLE(AST_NODE_ID * identifier);
    virtual SCL_TYPE evaluate(Environment * environment);
};

class AST_NODE_STRING :public AST_NODE_EXPR {
    public:
    std::string value;
    AST_NODE_STRING(std::string value);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_BOOLEAN :public AST_NODE_EXPR {
    public:
    bool value;
    AST_NODE_BOOLEAN(bool value);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_FLOAT :public AST_NODE_EXPR {
    public:
    float value;
    AST_NODE_FLOAT(float value);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_INTEGER :public AST_NODE_EXPR {
    public:
    int value;
    AST_NODE_INTEGER(int value);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_BIN_OP :public AST_NODE_EXPR {
    public:
    AST_NODE_EXPR * left;
    AST_NODE_EXPR * right;
    int operation;
    
    AST_NODE_BIN_OP(AST_NODE_EXPR * left, AST_NODE_EXPR * right, int operation);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_STATEMENT{
    public:
    virtual SCL_TYPE evaluate(Environment * environment) = 0;

};

class AST_NODE_RETURN :public AST_NODE_STATEMENT {
    public:
    AST_NODE_EXPR * expression;
    AST_NODE_RETURN(AST_NODE_EXPR * expression);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_STATEMENT_EXPRESSION :public AST_NODE_STATEMENT{
    public:
    AST_NODE_EXPR * expr;

    AST_NODE_STATEMENT_EXPRESSION(AST_NODE_EXPR * expr);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_FUNCTION_CALL :public AST_NODE_EXPR {
    public:
    AST_NODE_ID * function_name;
    std::vector<AST_NODE_EXPR *> parameter_list;

    AST_NODE_FUNCTION_CALL(AST_NODE_ID * function_name, std::vector<AST_NODE_EXPR *> parameter_list);
    virtual SCL_TYPE evaluate(Environment * environment);
};

class AST_NODE_DISPLAY :public AST_NODE_STATEMENT {
    public:
    std::vector<AST_NODE_EXPR *> expressions_list;

    AST_NODE_DISPLAY(std::vector<AST_NODE_EXPR *> expressions_list);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_INPUT :public AST_NODE_STATEMENT {
    public:
    AST_NODE_STRING * string_value;
    AST_NODE_ID * input;

    AST_NODE_INPUT(AST_NODE_STRING * string_value, AST_NODE_ID * input);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_WHILE_LOOP :public AST_NODE_STATEMENT {
    public:
    AST_NODE_EXPR * control_expression;
    std::vector<AST_NODE_STATEMENT *> statement_list;

    AST_NODE_WHILE_LOOP(AST_NODE_EXPR * control_expression, std::vector<AST_NODE_STATEMENT *> statement_list);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_FOR_LOOP :public AST_NODE_STATEMENT {
    public:
    AST_NODE_ID * identifier;
    AST_NODE_EXPR * ident_initial_value;
    AST_NODE_EXPR * ident_final_value;
    std::vector<AST_NODE_STATEMENT *> statement_list;

    AST_NODE_FOR_LOOP(AST_NODE_ID * identifier,
                      AST_NODE_EXPR * ident_initial_value,
                      AST_NODE_EXPR * ident_final_value,
                      std::vector<AST_NODE_STATEMENT *> statement_list);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_ELSEIF {
    public:
    AST_NODE_EXPR * control_expression;
    std::vector<AST_NODE_STATEMENT *> statement_list;

    AST_NODE_ELSEIF(AST_NODE_EXPR * control_expression, std::vector<AST_NODE_STATEMENT *> statement_list);
    virtual SCL_TYPE evaluate(Environment * environment);
};

class AST_NODE_IF_STATEMENT :public AST_NODE_STATEMENT{
    public:
    AST_NODE_EXPR * control_expression;
    std::vector<AST_NODE_STATEMENT *> statement_list;
    std::vector<AST_NODE_ELSEIF *> else_if_list;
    std::vector<AST_NODE_STATEMENT *> else_statement_list;

    AST_NODE_IF_STATEMENT(AST_NODE_EXPR * control_expression,
                          std::vector<AST_NODE_STATEMENT *> statement_list,
                          std::vector<AST_NODE_ELSEIF *> else_if_list,
                          std::vector<AST_NODE_STATEMENT *> else_statement_list);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_ASSIGNMENT :public AST_NODE_STATEMENT {
    public:
    AST_NODE_ID * identifier;
    AST_NODE_EXPR * expression;

    AST_NODE_ASSIGNMENT(AST_NODE_ID * identifier, AST_NODE_EXPR * expression);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_TYPE{
    public:
    int type;

    AST_NODE_TYPE(int type);

};

class AST_NODE_DEFINITION {
    public:
    AST_NODE_ID * identifier;
    AST_NODE_TYPE * type;

    AST_NODE_DEFINITION(AST_NODE_ID * identifier, AST_NODE_TYPE * type);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_VARIBALE_LIST{
    public:
    std::vector<AST_NODE_DEFINITION *> definition_list;

    AST_NODE_VARIBALE_LIST(std::vector<AST_NODE_DEFINITION *> definition_list);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_PARAMETER_LIST{
    public:
    std::vector<AST_NODE_DEFINITION *> definition_list;

    AST_NODE_PARAMETER_LIST(std::vector<AST_NODE_DEFINITION *> definition_list);
    virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_FUNCTION{
    public:
    AST_NODE_ID * function_name;
    AST_NODE_PARAMETER_LIST * parameter_list;
    AST_NODE_VARIBALE_LIST * variable_list;
    std::vector<AST_NODE_STATEMENT *> statement_list;

    AST_NODE_FUNCTION(AST_NODE_ID * function_name, 
                      AST_NODE_PARAMETER_LIST * parameter_list,                        
                      AST_NODE_VARIBALE_LIST * variable_list,
                      std::vector<AST_NODE_STATEMENT *> statement_list);
    virtual SCL_TYPE evaluate(Environment * environment);
};

class AST_NODE_IMPLEMENTATIONS{
    public:
    std::vector<AST_NODE_FUNCTION *> function_list;

    AST_NODE_IMPLEMENTATIONS(std::vector<AST_NODE_FUNCTION *> function_list);
    virtual SCL_TYPE evaluate(Environment * environment);


};

class AST_NODE_FUNCTION_PROTO{
    public:
    AST_NODE_ID * identifier;
    AST_NODE_TYPE * type;

    AST_NODE_FUNCTION_PROTO(AST_NODE_ID * ident, AST_NODE_TYPE * type);
    //virtual SCL_TYPE evaluate(Environment * environment);

};

class AST_NODE_FORWARD_DECLARE {
    public:
    std::vector<AST_NODE_FUNCTION_PROTO *> function_proto_list;

    AST_NODE_FORWARD_DECLARE(std::vector<AST_NODE_FUNCTION_PROTO *> function_proto_list);
    //virtual SCL_TYPE evaluate(Environment * environment);

};



class AST_NODE_PROGRAM {
    public:
    AST_NODE_FORWARD_DECLARE * declarations;
    AST_NODE_IMPLEMENTATIONS * implementations;

    AST_NODE_PROGRAM(AST_NODE_FORWARD_DECLARE * declare, AST_NODE_IMPLEMENTATIONS * implement);
    virtual SCL_TYPE evaluate(Environment * environment);
};

