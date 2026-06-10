#include <stdio.h>
#include <string.h>
#include "lexer.h"

static const char* tokenTypeNames[] =
{
    "KEYWORD",
    "OPERATOR",
    "SPECIAL_CHAR",
    "CONSTANT",
    "IDENTIFIER",
    "STRING_LITERAL",
    "MACRO",
    "COMMENT",
    "UNKNOWN"
};

/* sanitize token for printing */
static void sanitize(char *dst, const char *src)
{
    int i = 0, j = 0;

    while (src[i] != '\0' && j < MAX_TOKEN_SIZE - 1)
    {
        if (src[i] == '\n' || src[i] == '\r')
        {
            /* skip */
        }
        else if (src[i] == '\t')
        {
            dst[j++] = ' ';
        }
        else
        {
            dst[j++] = src[i];
        }
        i++;
    }
    dst[j] = '\0';
}

int main(int argc, char *argv[])
{
    Token token;
    char cleanLexeme[MAX_TOKEN_SIZE];

    if (argc < 2)
    {
        printf("Usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    if (checkSourceErrors(argv[1]) != 0)
        return 1;

    initializeLexer(argv[1]);

    printf("\n%-30s %-15s\n", "TOKEN", "TYPE");
    printf("---------------------------------------------\n");

    while (1)
    {
        token = getNextToken();

        if (token.type == UNKNOWN && token.lexeme[0] == '\0')
            break;

        sanitize(cleanLexeme, token.lexeme);

        printf("%-30s %-15s\n",
               cleanLexeme,
               tokenTypeNames[token.type]);
    }

    return 0;
}
