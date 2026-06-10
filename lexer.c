#include <string.h>
#include <ctype.h>
#include "lexer.h"

//hello

static const char* keywordList[MAX_KEYWORDS] =
{
    /*geo*/
    "int", "float", "return", "if", "else",
    "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case",
    "default", "const", "static", "sizeof", "struct"
};

static const char* operatorChars = "+-*/%=!<>|&^";
static const char* specialCharList = ",;{}()[]";

int isKeyword(const char* text)
{
    for (int i = 0; i < MAX_KEYWORDS; i++)
        if (strcmp(text, keywordList[i]) == 0)
            return 1;
    return 0;
}

int isOperator(const char* text)
{
    if (!text) return 0;

    if (strlen(text) == 1 && strchr(operatorChars, text[0]))
        return 1;

    if (strcmp(text, "==") == 0 || strcmp(text, "!=") == 0 ||
        strcmp(text, "<=") == 0 || strcmp(text, ">=") == 0 ||
        strcmp(text, "&&") == 0 || strcmp(text, "||") == 0 ||
        strcmp(text, "++") == 0 || strcmp(text, "--") == 0)
        return 1;

    return 0;
}

int isSpecialCharacter(char ch)
{
    return strchr(specialCharList, ch) != NULL;
}

int isConstant(const char* text)
{
    int dot = 0;
    if (!text || !*text) return 0;

    for (int i = 0; text[i]; i++)
    {
        if (text[i] == '.')
        {
            if (++dot > 1) return 0;
        }
        else if (!isdigit((unsigned char)text[i]))
            return 0;
    }
    return 1;
}

int isIdentifier(const char* text)
{
    if (!text || !*text) return 0;
    if (!isalpha((unsigned char)text[0]) && text[0] != '_')
        return 0;

    for (int i = 1; text[i]; i++)
        if (!isalnum((unsigned char)text[i]) && text[i] != '_')
            return 0;

    return 1;
}
