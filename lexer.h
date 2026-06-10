#ifndef LEXER_H
#define LEXER_H

#define MAX_KEYWORDS    20
#define MAX_TOKEN_SIZE  100

typedef enum
{
    KEYWORD,
    OPERATOR,
    SPECIAL_CHARACTER,
    CONSTANT,
    IDENTIFIER,
    STRING_LITERAL,
    MACRO,
    COMMENT,
    UNKNOWN
} TokenType;

typedef struct
{
    char      lexeme[MAX_TOKEN_SIZE];
    TokenType type;
} Token;

void  initializeLexer(const char* fileName);
Token getNextToken(void);

/* helper functions */
int isKeyword(const char* text);
int isOperator(const char* text);
int isSpecialCharacter(char ch);
int isConstant(const char* text);
int isIdentifier(const char* text);

/* error checker */
int checkSourceErrors(const char* fileName);

#endif
