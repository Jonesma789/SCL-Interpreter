#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <string>



//Character types:
#define ALPHA 1
#define NUMERAL 2
#define OTHER 3

//Literals:
#define IDENT 1000
#define INT_LIT 2000
#define FLOAT_LIT 2001 
#define STRING_LIT 3000
#define BOOL_LIT 3050
#define COMMENT 4000

//Reserved Words:
#define FORWARD 12
#define DECLARATIONS 39
#define FUNCTION 17
#define RETURN 19
#define IMPLEMENTATIONS 51
#define IS 126
#define BEGIN 52
#define ENDFUN 53
#define VARIABLES 41
#define DEFINE 42
#define OF 21
#define TYPE 23
#define FLOAT 28
#define INTEGER 25
#define BOOL 32
#define STRING 33
#define SET 89
#define AND 58
#define OR 57
#define IF 100
#define THEN 101
#define ELSEIF 115
#define ELSE 120
#define ENDIF 102
#define FOR 103
#define TO 86
#define DO 104
#define ENDFOR 127
#define WHILE 108
#define ENDWHILE 109
#define INPUT 91
#define DISPLAY 92
#define RETURN 19
#define TRUE 62
#define FALSE 63
#define CALL 99
#define PARAMETERS 69

//Symbols:
#define EQ_OP 49
#define DEQ_OP 64
#define ADD_OP 74
#define SUB_OP 75
#define MULT_OP 79
#define DIV_OP 80
#define GREATER 65
#define LESS 66
#define GRETER_EQ 67
#define LESS_EQ 68
#define L_PAREN 60
#define R_PAREN 61
#define COMMA 50

int populateReseservedWordsAndSymbols();

void addChar();
void getChar();
void getNonBlank();
int lex();

extern std::ifstream inputFile;
extern char nextChar;
extern char lexeme[100];
extern int lexSize;
extern int charClass;
extern int token;
extern std::map<std::string, int> reservedWords;
extern std::map<std::string, int> symbols;