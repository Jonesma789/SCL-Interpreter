#include "AST_NODES.h"
#include "scanner.h"

using namespace std;

SCL_TYPE makeVoid();

AST_NODE_PROGRAM::AST_NODE_PROGRAM(AST_NODE_FORWARD_DECLARE * declare, AST_NODE_IMPLEMENTATIONS * implement) {
    this->declarations = declare;
    this->implementations = implement;
}
SCL_TYPE AST_NODE_PROGRAM::evaluate(Environment * environment) {
    environment->populateFunctionTable(implementations->function_list);
    return implementations->evaluate(environment);
}


AST_NODE_FORWARD_DECLARE::AST_NODE_FORWARD_DECLARE(std::vector<AST_NODE_FUNCTION_PROTO *> function_proto_list) {
    this->function_proto_list = function_proto_list;
}

AST_NODE_FUNCTION_PROTO::AST_NODE_FUNCTION_PROTO(AST_NODE_ID * ident, AST_NODE_TYPE * type) {
    this->identifier = ident;
    this->type = type;
}

AST_NODE_IMPLEMENTATIONS::AST_NODE_IMPLEMENTATIONS(std::vector<AST_NODE_FUNCTION *> function_list) {
    this->function_list = function_list;
}
SCL_TYPE AST_NODE_IMPLEMENTATIONS::evaluate(Environment * environment){
    auto it = function_list.begin();
    return (*it)->evaluate(environment); 
    
}

AST_NODE_FUNCTION::AST_NODE_FUNCTION(AST_NODE_ID * function_name, 
                                     AST_NODE_PARAMETER_LIST * parameter_list,
                                     AST_NODE_VARIBALE_LIST * variable_list,
                                     std::vector<AST_NODE_STATEMENT *> statement_list) {
    this->function_name = function_name;
    this->parameter_list = parameter_list;
    this->variable_list = variable_list;
    this->statement_list = statement_list;
}
SCL_TYPE AST_NODE_FUNCTION::evaluate(Environment * environment){
    variable_list->evaluate(environment);
    try {
    for(auto it = statement_list.begin(); it != statement_list.end(); it++) {
        (*it)->evaluate(environment);
    }
    }catch(const SCLReturned &e) {
        return e.value;
    }
    return makeVoid();
}

AST_NODE_PARAMETER_LIST::AST_NODE_PARAMETER_LIST(std::vector<AST_NODE_DEFINITION *> definition_list) {
    this->definition_list = definition_list;
}
SCL_TYPE AST_NODE_PARAMETER_LIST::evaluate(Environment * environment) {
    for(auto it = definition_list.begin(); it != definition_list.end(); it ++) {
        (*it)->evaluate(environment);
    }
    return makeVoid();
}

AST_NODE_VARIBALE_LIST::AST_NODE_VARIBALE_LIST(std::vector<AST_NODE_DEFINITION *> definition_list) {
    this->definition_list = definition_list;
}
SCL_TYPE AST_NODE_VARIBALE_LIST::evaluate(Environment * environment) {
    for(auto it = definition_list.begin(); it != definition_list.end(); it ++) {
        (*it)->evaluate(environment);
    }
    return makeVoid();
}

AST_NODE_DEFINITION::AST_NODE_DEFINITION(AST_NODE_ID * identifier, AST_NODE_TYPE * type) {
    this->identifier = identifier;
    this->type = type;
}
SCL_TYPE AST_NODE_DEFINITION::evaluate(Environment * environment) {
    string key = identifier->varibale_name;
    SCL_TYPE empty;
    empty.actualType = type->type;
    environment->vAddKey(key, empty);
    return makeVoid();
}

AST_NODE_TYPE::AST_NODE_TYPE(int type) {
    this->type = type;
}

AST_NODE_STATEMENT_EXPRESSION::AST_NODE_STATEMENT_EXPRESSION(AST_NODE_EXPR * expr){
    this->expr = expr;
}
SCL_TYPE AST_NODE_STATEMENT_EXPRESSION::evaluate(Environment * environment) {
    return expr->evaluate(environment);
}


AST_NODE_WHILE_LOOP::AST_NODE_WHILE_LOOP(AST_NODE_EXPR * control_expression, std::vector<AST_NODE_STATEMENT *> statement_list) {
    this->control_expression = control_expression;
    this->statement_list = statement_list;
}
SCL_TYPE AST_NODE_WHILE_LOOP::evaluate(Environment * environment) {
    SCL_TYPE expression = control_expression->evaluate(environment);
    if(expression.actualType == TYPE_BOOLEAN) {
        while(expression.boolValue) {
            for(auto it = statement_list.begin(); it != statement_list.end(); it++) {
                (*it)->evaluate(environment);
            }

            expression = control_expression->evaluate(environment);
        }
    }
    else {
        SCLError("Control expression was not a boolean"); 
    }
    
    return makeVoid();
}



AST_NODE_FOR_LOOP::AST_NODE_FOR_LOOP(AST_NODE_ID * identifier,
                                     AST_NODE_EXPR * ident_initial_value,
                                     AST_NODE_EXPR * ident_final_value,
                                     std::vector<AST_NODE_STATEMENT *> statement_list) {
    this->identifier = identifier;
    this->ident_initial_value = ident_initial_value;
    this->ident_final_value = ident_final_value;
    this->statement_list = statement_list;

}
SCL_TYPE AST_NODE_FOR_LOOP::evaluate(Environment * environment) {
    SCL_TYPE control_variable = environment->vLookUpID(identifier->varibale_name); //Wait, I don't need this, right?
    SCL_TYPE init_val = ident_initial_value->evaluate(environment);
    SCL_TYPE final_val = ident_final_value->evaluate(environment);
    if(control_variable.actualType == TYPE_INTEGER) { //Should I error check to make sure the types match for the three variables?
        for(int i = init_val.intValue; i <= final_val.intValue; i++) {
            control_variable.intValue = i;
            environment->vUpdateID(identifier->varibale_name, control_variable);
            for(auto it = statement_list.begin(); it != statement_list.end(); it++) {
                (*it)->evaluate(environment);
            }
        } 
        
    }else {
        SCLError("Control expression was not a boolean"); //Type of control expression is not bool
    }
    return control_variable; //Should I just return makeVoid() instead?
}

AST_NODE_IF_STATEMENT::AST_NODE_IF_STATEMENT(AST_NODE_EXPR * control_expression,
                                             std::vector<AST_NODE_STATEMENT *> statement_list,
                                             std::vector<AST_NODE_ELSEIF *> else_if_list,
                                             std::vector<AST_NODE_STATEMENT *> else_statement_list) {
    this->control_expression = control_expression;
    this->statement_list = statement_list;
    this->else_if_list = else_if_list;
    this->else_statement_list = else_statement_list;
}
SCL_TYPE AST_NODE_IF_STATEMENT::evaluate(Environment * environment) {
    bool if_evaluated = false;
    SCL_TYPE controller = control_expression->evaluate(environment);
    if(controller.actualType == TYPE_BOOLEAN) {
        if(controller.boolValue) {
            if_evaluated = true;
            for(auto it = statement_list.begin(); it != statement_list.end(); it++) {
                (*it)->evaluate(environment);
            }
        }if(!if_evaluated) {
            for(auto it = else_if_list.begin(); it != else_if_list.end(); it++) {
                SCL_TYPE eval = (*it)->evaluate(environment);
                if(eval.boolValue) {
                    if_evaluated = true;
                    break;
                }
                
            }
        }
        if(!if_evaluated) {
            for(auto it = else_statement_list.begin(); it != else_statement_list.end(); it++) {
                (*it)->evaluate(environment);
            }
        }
        
        return makeVoid();
    }else {
        SCLError("Control expression was not a boolean");
    }
    
}

AST_NODE_ELSEIF::AST_NODE_ELSEIF(AST_NODE_EXPR * control_expression, std::vector<AST_NODE_STATEMENT *> statement_list) {
    this->control_expression = control_expression;
    this->statement_list = statement_list;
}
SCL_TYPE AST_NODE_ELSEIF::evaluate(Environment * environment) {
    SCL_TYPE controller = control_expression->evaluate(environment);
    if(controller.actualType == TYPE_BOOLEAN) {
        if(controller.boolValue) {
            for(auto it = statement_list.begin(); it != statement_list.end(); it++) {
                (*it)->evaluate(environment);
            }
        }
        return controller;
    }else {
        SCLError("Control expression was not a boolean");
    }
}

AST_NODE_ASSIGNMENT::AST_NODE_ASSIGNMENT(AST_NODE_ID * identifier, AST_NODE_EXPR * expression){
    this->identifier = identifier;
    this->expression = expression;
}
SCL_TYPE AST_NODE_ASSIGNMENT::evaluate(Environment * environment) {
    SCL_TYPE assigner = expression->evaluate(environment);
    SCL_TYPE checker = environment->vLookUpID(identifier->varibale_name);
    if(checker.actualType != assigner.actualType) {
        cout << "assigner's actual type is: " << assigner.actualType << endl;
        cout << "checker's actual type is: " << checker.actualType << endl;
        cout << "trying to set varriable: " << identifier->varibale_name << endl;
        SCLError("Cannot assign value to variable");
    }
    environment->vUpdateID(identifier->varibale_name, assigner);
    assigner = environment->vLookUpID(identifier->varibale_name);
    return makeVoid();
}

AST_NODE_INPUT::AST_NODE_INPUT(AST_NODE_STRING * string_value, AST_NODE_ID * input) {
    this->string_value = string_value;
    this->input = input;
}
SCL_TYPE AST_NODE_INPUT::evaluate(Environment * environment) {
    cout << string_value->value << endl;
    string key = input->varibale_name;
    SCL_TYPE variable = environment->vLookUpID(key);
    switch (variable.actualType) {
        case TYPE_INTEGER:
        {
            cin >> variable.intValue;
            environment->vUpdateID(key,variable);
            break;
        }
        case TYPE_FLOAT:
        {
            cin >> variable.floatValue;
            environment->vUpdateID(key,variable);
            break;
        }
        case TYPE_BOOLEAN:
        {
            cin >> variable.boolValue;
            environment->vUpdateID(key,variable);
            break;
        }
        case TYPE_STRING: 
        {
            cin >> variable.stringValue;
            environment->vUpdateID(key,variable);
            break;
        }
        default:
        {
            SCLError("Unknown input type.");
        }
    }

    return makeVoid();
}


AST_NODE_DISPLAY::AST_NODE_DISPLAY(std::vector<AST_NODE_EXPR *> expressions_list) {
    this->expressions_list = expressions_list;
}
SCL_TYPE AST_NODE_DISPLAY::evaluate(Environment * environment){
    for(auto it = expressions_list.begin(); it != expressions_list.end(); it++) {
        SCL_TYPE printer = (*it)->evaluate(environment);
        if(printer.actualType == TYPE_INTEGER) {
            cout << printer.intValue;
        }else if(printer.actualType == TYPE_FLOAT) {
            cout << printer.floatValue;
        }else if(printer.actualType == TYPE_BOOLEAN) {
            cout << printer.boolValue;
        }else if(printer.actualType == TYPE_STRING) {
            cout << printer.stringValue;
        }
    }
    cout << endl;
    return makeVoid();
}

AST_NODE_FUNCTION_CALL::AST_NODE_FUNCTION_CALL(AST_NODE_ID * function_name, vector<AST_NODE_EXPR *> parameter_list) {
    this->function_name = function_name;
    this->parameter_list = parameter_list;
}
SCL_TYPE AST_NODE_FUNCTION_CALL::evaluate(Environment * environment) {
    string key = function_name->varibale_name;
    AST_NODE_FUNCTION * function = environment->fLookUpID(key);
    if(function == NULL) {
        SCLError("Function does not exist");
    }
    map<string,SCL_TYPE> newVariableTable;
    for(int i = 0; i < parameter_list.size(); i++) {
        newVariableTable[function->parameter_list->definition_list[i]->identifier->varibale_name] = parameter_list[i]->evaluate(environment);
    }
    Environment newEnvironment(newVariableTable, environment->functionTable);
    return function->evaluate(&newEnvironment);
}

AST_NODE_RETURN::AST_NODE_RETURN(AST_NODE_EXPR * expression) {
    this->expression = expression;
}
SCL_TYPE AST_NODE_RETURN::evaluate(Environment * environment) {
    SCL_TYPE result = expression->evaluate(environment);
    throw(SCLReturned(result));
    return result;
}

AST_NODE_BIN_OP::AST_NODE_BIN_OP(AST_NODE_EXPR * left, AST_NODE_EXPR * right, int operation) {
    this->left = left;
    this->right = right;
    this->operation = operation;
}
SCL_TYPE AST_NODE_BIN_OP::evaluate(Environment * environment) {
    SCL_TYPE left_side = left->evaluate(environment);
    SCL_TYPE right_side = right->evaluate(environment);
    if(operation == AND) {//For these types of operations, do I care what type the input is, or does it always return a bool SCL_TYPE?
        SCL_TYPE result;
        result.actualType = TYPE_BOOLEAN;
        //Is this else if chain necessary, or is there a better way to do this?
        if(left_side.actualType == TYPE_BOOLEAN && right_side.actualType == TYPE_BOOLEAN){
            result.boolValue = left_side.boolValue && right_side.boolValue;
        }else {
            SCLError("At least one operand was not a boolean");
        }
        return result;
    }else if(operation == OR) {
        SCL_TYPE result;
        result.actualType = TYPE_BOOLEAN;
        //Is this else if chain necessary, or is there a better way to do this?
         if(left_side.actualType == TYPE_BOOLEAN && right_side.actualType == TYPE_BOOLEAN){
            result.boolValue = left_side.boolValue || right_side.boolValue;
        }else {
              SCLError("At least one operand was not a boolean");
        }
        return result;
    }else if(operation == DEQ_OP) {
        SCL_TYPE result;
        result.actualType = TYPE_BOOLEAN;
        //Is this else if chain necessary, or is there a better way to do this?
        if(left_side.actualType != right_side.actualType) {
              SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) { //Will I need another way of checking the type of the right side, or is it okay to assume?
            result.boolValue = left_side.intValue == right_side.intValue;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.boolValue = left_side.floatValue == right_side.floatValue;
        }else if(left_side.actualType == TYPE_BOOLEAN){
            result.boolValue = left_side.boolValue == right_side.boolValue;
        }else if(left_side.actualType == TYPE_STRING){
            result.boolValue = left_side.stringValue == right_side.stringValue;
        }
        return result;
    }else if(operation == GREATER) {
        SCL_TYPE result;
        result.actualType = TYPE_BOOLEAN;
        if(left_side.actualType != right_side.actualType) {
            SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) {
            result.boolValue = left_side.intValue > right_side.intValue;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.boolValue = left_side.floatValue > right_side.floatValue;
        }else if(left_side.actualType == TYPE_BOOLEAN){
            SCLError("Cannot perform that operation on that type");
        }else if(left_side.actualType == TYPE_STRING){
            result.boolValue = left_side.stringValue > right_side.stringValue;
        }
        return result;
    }else if(operation == LESS) {
        SCL_TYPE result;
        result.actualType = TYPE_BOOLEAN;
        if(left_side.actualType != right_side.actualType) {
           SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) { 
            result.boolValue = left_side.intValue < right_side.intValue;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.boolValue = left_side.floatValue < right_side.floatValue;
        }else if(left_side.actualType == TYPE_BOOLEAN){
            SCLError("Cannot perform that operation on that type");
        }else if(left_side.actualType == TYPE_STRING){
            result.boolValue = left_side.stringValue < right_side.stringValue;
        }
        return result;
    }else if(operation == GRETER_EQ) {
        SCL_TYPE result;
        result.actualType = TYPE_BOOLEAN;
        if(left_side.actualType != right_side.actualType) {
            SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) {
            result.boolValue = left_side.intValue >= right_side.intValue;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.boolValue = left_side.floatValue >= right_side.floatValue;
        }else if(left_side.actualType == TYPE_BOOLEAN){
            SCLError("Cannot perform that operation on that type");
        }else if(left_side.actualType == TYPE_STRING){
            result.boolValue = left_side.stringValue >= right_side.stringValue;
        }
        return result;
    }else if(operation == LESS_EQ) {
        SCL_TYPE result;
        result.actualType = TYPE_BOOLEAN;
        if(left_side.actualType != right_side.actualType) {
            SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) {
            result.boolValue = left_side.intValue <= right_side.intValue;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.boolValue = left_side.floatValue <= right_side.floatValue;
        }else if(left_side.actualType == TYPE_BOOLEAN){
            SCLError("Cannot perform that operation on that type");
        }else if(left_side.actualType == TYPE_STRING){
            result.boolValue = left_side.stringValue <= right_side.stringValue;
        }
        return result;
    }else if(operation == ADD_OP) {
        SCL_TYPE result;

        if(left_side.actualType != right_side.actualType) {
            SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) { //Will I need another way of checking the type of the right side, or is it okay to assume?
            result.intValue = left_side.intValue + right_side.intValue;
            result.actualType = TYPE_INTEGER;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.floatValue = left_side.floatValue + right_side.floatValue;
            result.actualType = TYPE_FLOAT;
        }else if(left_side.actualType == TYPE_BOOLEAN){
            SCLError("Cannot perform that operation on that type");
        }else if(left_side.actualType == TYPE_STRING){
            result.stringValue = left_side.stringValue + right_side.stringValue;
            result.actualType = TYPE_STRING;
        }
        return result;
    }else if(operation == SUB_OP) {
        SCL_TYPE result;

        if(left_side.actualType != right_side.actualType) {
            SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) { //Will I need another way of checking the type of the right side, or is it okay to assume?
            result.intValue = left_side.intValue - right_side.intValue;
            result.actualType = TYPE_INTEGER;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.floatValue = left_side.floatValue - right_side.floatValue;
            result.actualType = TYPE_FLOAT;
        }else if(left_side.actualType == TYPE_BOOLEAN || left_side.actualType == TYPE_STRING){
            SCLError("Cannot perform that operation on that type");
        }
        return result;
    }else if(operation == MULT_OP) {
        SCL_TYPE result;

        if(left_side.actualType != right_side.actualType) {
            SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) { //Will I need another way of checking the type of the right side, or is it okay to assume?
            result.intValue = left_side.intValue *  right_side.intValue;
            result.actualType = TYPE_INTEGER;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.floatValue = left_side.floatValue * right_side.floatValue;
            result.actualType = TYPE_FLOAT;
        }else if(left_side.actualType == TYPE_BOOLEAN || left_side.actualType == TYPE_STRING){
            SCLError("Cannot perform that operation on that type");
        }
        return result;
    }else if(operation == DIV_OP) {
        SCL_TYPE result;

        if(left_side.actualType != right_side.actualType) {
            SCLError("Operand types do not match");
        }
        if(left_side.actualType == TYPE_INTEGER) { //Will I need another way of checking the type of the right side, or is it okay to assume?
            result.intValue = left_side.intValue / right_side.intValue;
            result.actualType = TYPE_INTEGER;
        }else if(left_side.actualType == TYPE_FLOAT){
            result.floatValue = left_side.floatValue / right_side.floatValue;
            result.actualType = TYPE_FLOAT;
        }else if(left_side.actualType == TYPE_BOOLEAN || left_side.actualType == TYPE_STRING){
           SCLError("Cannot perform that operation on that type");
        }
        return result;
    }

}


AST_NODE_INTEGER::AST_NODE_INTEGER(int value) {
    this->value = value;
}
SCL_TYPE AST_NODE_INTEGER::evaluate(Environment * environment) {
    SCL_TYPE integer;
    integer.actualType = TYPE_INTEGER;
    integer.intValue = value;
    return integer;
}

AST_NODE_FLOAT::AST_NODE_FLOAT(float value) {
    this->value = value;
}
SCL_TYPE AST_NODE_FLOAT::evaluate(Environment * environment) {
    SCL_TYPE flt;
    flt.actualType = TYPE_FLOAT;
    flt.floatValue = value;
    return flt;
}

AST_NODE_BOOLEAN::AST_NODE_BOOLEAN(bool value) {
    this->value = value;
}
SCL_TYPE AST_NODE_BOOLEAN::evaluate(Environment * environment) {
    SCL_TYPE boolean;
    boolean.actualType = TYPE_BOOLEAN;
    boolean.boolValue = value;
    return boolean;
}

AST_NODE_STRING::AST_NODE_STRING(std::string value) {
    this->value = value;
}
SCL_TYPE AST_NODE_STRING::evaluate(Environment * environment) {
    SCL_TYPE str;
    str.actualType = TYPE_STRING;
    str.stringValue = value;
    return str;
}

AST_NODE_VARIABLE::AST_NODE_VARIABLE(AST_NODE_ID * identifier) {
    this->identifier = identifier;
}
SCL_TYPE AST_NODE_VARIABLE::evaluate(Environment * environment) {
    return environment->vLookUpID(identifier->varibale_name);
}

AST_NODE_ID::AST_NODE_ID(std::string varibale_name) {
    this->varibale_name = varibale_name;
}

SCL_TYPE::SCL_TYPE() {
    intValue = -1;
    floatValue = -1.0;
    boolValue = false;
    stringValue = "";
    actualType = TYPE_VOID;
} 

Environment::Environment(map<string, SCL_TYPE> variableTable, map<string, AST_NODE_FUNCTION *> functionTable) {
    this->variableTable = variableTable;
    this->functionTable = functionTable;
}
SCL_TYPE Environment::vLookUpID(std::string key) {
    auto it = variableTable.find(key);
    if(it != variableTable.end()) {
        return it->second;
    }else {
        SCLError("Could not find varibale with that ID");
    }

}
void Environment::vUpdateID(std::string key, SCL_TYPE value){
    auto it = variableTable.find(key);
    if(it != variableTable.end()) {
        it->second = value;
    }else {
        SCLError("Could not find varibale with that ID");
    }

}
void Environment::vAddKey(std::string key, SCL_TYPE empty_value){
    auto it = variableTable.find(key);
    if(variableTable.find(key) != variableTable.end()) {
        SCLError("Variable with the name " + key + " already exists.");
    }
    variableTable.insert(pair<string,SCL_TYPE>(key, empty_value));
}

void Environment::populateFunctionTable(vector<AST_NODE_FUNCTION *> functions) {
    for(auto it = functions.begin(); it != functions.end(); it++) {
        AST_NODE_FUNCTION * access = *it;
        functionTable[access->function_name->varibale_name] = access;
    }
}
AST_NODE_FUNCTION * Environment::fLookUpID(std::string key){
    auto it = functionTable.find(key);
    if(it != functionTable.end()) {
        return it->second;
    }else {
        return NULL;
    }
}




SCL_TYPE makeVoid() {
    SCL_TYPE void_return;
    void_return.actualType = TYPE_VOID;
    return void_return;
}

SCLException::SCLException(string file, int line, string message) {
    this->file = file;
    this->line = line;
    this->message = message;
}

SCLReturned::SCLReturned(SCL_TYPE value) {
    this->value = value;
}

