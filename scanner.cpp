#include "scanner.h"
#include "parser.h"

using namespace std;

ifstream inputFile;
char nextChar;
char lexeme[100];
int lexSize;
int charClass;
int token;
map<string, int> reservedWords;
map<string, int> symbols;
/*
int main() { // This is the main driver for the scanner. It populates the reserved words and reserved symbol tables,
             // opens the input scl file, and runs lex until the EOF is reached.
    populateReseservedWordsAndSymbols();
    inputFile.open("test.scl");
    if(inputFile.fail()) {
        cout << "unable to open file. check file name and try again" << endl;
        exit(1);
    }else {
        getChar();
        while(!inputFile.eof()) {
            lex();
        }
    }

}*/

void getChar() { // This function is used to read in characters from the input file. it uses the "get()" 
                 // function from ifstreams to read in the next character from the input file and saves 
                 // that value in nextChar. It also sets the charClass variable correctly depending on 
                 // what kind of character was received from the input file.
    if(!inputFile.eof()) {
        inputFile.get(nextChar);
        if(isalpha(nextChar)){
            charClass = ALPHA;
        }else if(isdigit(nextChar)){
            charClass = NUMERAL;
        }else {
            charClass = OTHER;
        }
    }else {
        charClass = EOF;
    }
}

void addChar() { // This function takes the character stored in nextChar and adds it to the lexeme array. 
                 // It also makes lexeme a null terminated string.
    if(lexSize > 99) {
        cout << "Lexeme is too long." << endl;
    }else {
        lexeme[lexSize++] = nextChar;
        lexeme[lexSize] = 0;
    }
}

void getNonBlank() { // This function iteratively calls the getChar() function until the variable nextChar is a nonwhitespace character.
                     // This will skip over spaces, tabs, newline characters, etc.
    int i = 1;
    while(isspace(nextChar) && !inputFile.eof()) {
        //cout << "got a blank. Have so far gotten " << i++ << " blanks." << endl; 
        // To see how many blanks were found, uncomment the line above this one.
        getChar();
    }
    if(isspace(nextChar)) {
        charClass = EOF;
    }
}

int lex() { // This function calls the getChar() and addChar() functions to add characters to 
            // the lexeme until it is finished, and then assigns a token to that lexeme based on which word was read in from the file.
    lexSize = 0;
    getNonBlank(); // no word in the language will start with whitespace, so skip whitespace until you have an actual character.
    string checker; // This is used to convert the lexeme array into a string that can be used to search the maps of reserved words and symbols.
    switch (charClass) // The charClass variable will hold an indicator to what kind of character nextChar holds. At this point, nextChar will 
                       // be the first character in the lexeme.
    {
    case ALPHA: // This is for when nextChar holds a letter or alphbetical character. If the first character of a lexeme is a letter, then the 
                // lexeme must be either a reserved word or an identifier.
    {
        addChar();
        getChar(); 
        while(charClass == ALPHA || charClass == NUMERAL || nextChar == '_') { // Keep reading in characters until you get a character that could not be part of an identifier or a keyword.
            addChar();
            getChar();
        }
        checker = lexeme; // Set the lexeme array to a string so we can look up the lexeme.
        map<string,int>::iterator i = reservedWords.find(checker);
        if(i == reservedWords.end()) { // The lexeme was not found in the reserved words tables, so it must be an identifier. Set the token appropriately.
            token = IDENT;
        }else { // The lexeme was found within the reserved words table, so set the token to the appropriate token within the table.
            token = i->second;
        }
        break;
    }
    case NUMERAL: // This is for when nextChar holds a numerical character. If the first character in the lexeme is a number, then the 
                  // lexeme must be a float or an integer.
    {
        addChar();
        getChar();
        while(charClass == NUMERAL) { //Keep reading in characters until you get a character that is not a number.
            addChar();
            getChar();
        }
        if(nextChar == '.') { //if the character is not a number but is a '.', then you need to add that '.' to the lexeme.
            addChar();
            getChar();
            while(charClass == NUMERAL) { // keep adding more numbers to the lexeme until there are no more numbers.
                addChar();
                getChar();
            }
            token = FLOAT_LIT; // The lexeme scanned was a float, so set th token for it to FLOAT_LIT
        }else {
            token = INT_LIT; // The lexeme scanned was an int, so set th token for it to INT_LIT
        }
        break;
    }
    case OTHER: // This is for when nextChar holds something that is not a number or a letter. Since before the switch statement, 
                // the function getNonBank() was called, then we know that this character must be a symbol.
    {
        addChar();
        getChar();
        if(lexeme[0] == '=' || lexeme[0] == '<' || lexeme[0] == '>') { //These symbols all could be part of their own token, or could
                                                                       // be part of a 2 character long token. Need to check if the next 
                                                                       // character is part of this lexeme or not.
            if(nextChar == '=') { // If this statement is true, then th next character is part of this lexeme.
                addChar();
                getChar();
            }
            checker = lexeme;
            map<string,int>::iterator i = symbols.find(checker);
            token = i->second;
        }else if(lexeme[0] == '"') { //If the first character was a '"', then this is a string literal. Keep getting characters until 
                                     //another '"' is reached, then set the token to STRING_LIT
            do {
                addChar();
                getChar();
            } while(nextChar != '"' && nextChar != EOF);
            addChar();
            getChar();
            token = STRING_LIT;
        }else if(lexeme[0] == '/') { //If the first character was a '/', then this could be a comment.
            if(nextChar =='/') { // If the next character is also a '/', then this is a comment. Keep reading characters until  new 
                                 // line character is reached, then assign the lexeme the COMMENT token. 
                while(!(nextChar == '\n')) {
                    addChar();
                    getChar();
                }
                token = COMMENT;
            }
        }else { //If none of the previous options were true, then this is a single character lexeme. Look it up in the symbol table and assign it the appropriate token.
            checker = lexeme;
            map<string,int>::iterator i = symbols.find(checker); 
            if(i == symbols.end()) {
                token = 12345;
                // exit(1);
            }else {
                token = i->second;
            }
        }
        break;
    }
    case EOF: //Once the end of file is reached, the final lexeme will be set to "EOF" and the token will be set to -1.
    {
        token = -1; //-1 will correspond to the EOF
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
    }
    }
    checker = lexeme;
    
    return token;
}

int populateReseservedWordsAndSymbols() {
    reservedWords["import"] = 10;
    reservedWords["use"] = 11;
    reservedWords["forward"] = 12;
    reservedWords["interface"] = 13;
    reservedWords["struct"] = 14;
    reservedWords["structtype"] = 15;
    reservedWords["definetype"] = 16;
    reservedWords["function"] = 17;
    reservedWords["return"] = 19;
    reservedWords["pointer"] = 20;
    reservedWords["of"] = 21;
    reservedWords["array"] = 22;
    reservedWords["type"] = 23;
    reservedWords["count"] = 24;
    reservedWords["integer"] = 25;
    reservedWords["short"] = 26;
    reservedWords["real"] = 27;
    reservedWords["float"] = 28;
    reservedWords["double"] = 29;
    reservedWords["char"] = 30;
    reservedWords["byte"] = 31;
    reservedWords["bool"] = 32;
    reservedWords["string"] = 33;
    reservedWords["length"] = 34;
    reservedWords["enum"] = 35;
    reservedWords["specifications"] = 36;
    reservedWords["description"] = 37;
    reservedWords["global"] = 38;
    reservedWords["declarations"] = 39;
    reservedWords["constants"] = 40;
    reservedWords["variables"] = 41;
    reservedWords["define"] = 42;
    reservedWords["persistent"] = 43;
    reservedWords["shared"] = 44;
    reservedWords["static"] = 45;
    reservedWords["long"] = 46;
    reservedWords["void"] = 47;
    reservedWords["value"] = 48;
    reservedWords["implementations"] = 51;
    reservedWords["begin"] = 52;
    reservedWords["endfun"] = 53;
    reservedWords["accesor"] = 54;
    reservedWords["mutator"] = 55;
    reservedWords["precondition"] = 56;
    reservedWords["or"] = 57;
    reservedWords["and"] = 58;
    reservedWords["not"] = 59;   
    reservedWords["true"] = 62;
    reservedWords["false"] = 63;
    reservedWords["parameters"] = 69;
    reservedWords["alters"] = 70;
    reservedWords["preserves"] = 71;
    reservedWords["produces"] = 72;
    reservedWords["consumes"] = 73;
    reservedWords["band"] = 76;
    reservedWords["bor"] = 77;
    reservedWords["bxor"] = 78;
    reservedWords["address"] = 82;
    reservedWords["deref"] = 83;
    reservedWords["negate"] = 84;
    reservedWords["add"] = 85;
    reservedWords["to"] = 86;
    reservedWords["subtract"] = 87;
    reservedWords["from"] = 88;
    reservedWords["set"] = 89;
    reservedWords["read"] = 90;
    reservedWords["input"] = 91;
    reservedWords["display"] = 92;
    reservedWords["displayn"] = 93;
    reservedWords["close"] = 94;
    reservedWords["open"] = 95;
    reservedWords["file"] = 96;
    reservedWords["increment"] = 97;
    reservedWords["decement"] = 98;
    reservedWords["call"] = 99;
    reservedWords["if"] = 100;
    reservedWords["then"] = 101;
    reservedWords["endif"] = 102;
    reservedWords["for"] = 103;
    reservedWords["do"] = 104;
    reservedWords["repeat"] = 105;
    reservedWords["until"] = 106;
    reservedWords["endrepeat"] = 107;
    reservedWords["while"] = 108;
    reservedWords["endwhile"] = 109;
    reservedWords["case"] = 110;
    reservedWords["endcase"] = 111;
    reservedWords["break"] = 112;
    reservedWords["exit"] = 113;
    reservedWords["potcondition"] = 114;
    reservedWords["elseif"] = 115;
    reservedWords["downto"] = 116;
    reservedWords["using"] = 117;
    reservedWords["when"] = 118;
    reservedWords["else"] = 120;
    reservedWords["default"] = 121;
    reservedWords["output"] = 122;
    reservedWords["write"] = 123;
    reservedWords["dot"] = 124;
    reservedWords["is"] = 126;
    reservedWords["endfor"] = 127;  


    symbols[">"] = 65;
    symbols["<"] = 66;
    symbols[">="] = 67;
    symbols["<="] = 68;
    symbols["="] = 49;
    symbols[","] = 50;
    symbols["=="] = 64;
    symbols[":"] = 119;
    symbols["*"] = 79;
    symbols["/"] = 80;
    symbols["%"] = 81;
    symbols["+"] = 74;
    symbols["-"] = 75;
    symbols["("] = 60;
    symbols[")"] = 61; 
    symbols["//"] = 125;
    


    return 0;
}
