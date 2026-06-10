#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

static FILE *sourceFile = NULL;

void initializeLexer(const char* fileName)
{
    sourceFile = fopen(fileName, "r");
}

Token getNextToken(void)
{
    Token token;
    int ch, next;
    int index = 0;

    token.lexeme[0] = '\0';
    token.type = UNKNOWN;

    if (!sourceFile)
        return token;

    /* skip whitespace */
    do {
        ch = fgetc(sourceFile);
    } while (isspace(ch));

    if (ch == EOF)
        return token;

    /* ---------- MACRO ---------- */
    if (ch == '#')
    {
        token.lexeme[index++] = '#';

        while ((ch = fgetc(sourceFile)) != EOF &&
               ch != '\n' &&
               index < MAX_TOKEN_SIZE - 1)
        {
            token.lexeme[index++] = ch;
        }

        token.lexeme[index] = '\0';
        token.type = MACRO;
        return token;
    }

    /* ---------- COMMENTS ---------- */
    if (ch == '/')
    {
        next = fgetc(sourceFile);

        /* single-line comment */
        if (next == '/')
        {
            token.lexeme[index++] = '/';
            token.lexeme[index++] = '/';

            while ((ch = fgetc(sourceFile)) != EOF &&
                   ch != '\n' &&
                   index < MAX_TOKEN_SIZE - 1)
            {
                token.lexeme[index++] = ch;
            }

            token.lexeme[index] = '\0';
            token.type = COMMENT;
            return token;
        }

        /* block comment (FIXED) */
        if (next == '*')
        {
            token.lexeme[index++] = '/';
            token.lexeme[index++] = '*';

            while ((ch = fgetc(sourceFile)) != EOF &&
                   index < MAX_TOKEN_SIZE - 1)
            {
                token.lexeme[index++] = ch;

                if (ch == '*')
                {
                    next = fgetc(sourceFile);
                    if (next == '/')
                    {
                        token.lexeme[index++] = '/';
                        break;
                    }
                    else if (next != EOF)
                    {
                        ungetc(next, sourceFile);
                    }
                }
            }

            token.lexeme[index] = '\0';
            token.type = COMMENT;
            return token;
        }

        if (next != EOF)
            ungetc(next, sourceFile);
    }

    /* ---------- IDENTIFIER / KEYWORD ---------- */
    if (isalpha(ch) || ch == '_')
    {
        while ((isalnum(ch) || ch == '_') &&
               index < MAX_TOKEN_SIZE - 1)
        {
            token.lexeme[index++] = ch;
            ch = fgetc(sourceFile);
        }

        token.lexeme[index] = '\0';
        if (ch != EOF)
            ungetc(ch, sourceFile);

        token.type = isKeyword(token.lexeme) ? KEYWORD : IDENTIFIER;
        return token;
    }

    /* ---------- NUMBER (DECIMAL / HEX) ---------- */
    if (isdigit(ch))
    {
        token.lexeme[index++] = ch;
        ch = fgetc(sourceFile);

        if (token.lexeme[0] == '0' && (ch == 'x' || ch == 'X'))
        {
            token.lexeme[index++] = ch;
            while (isxdigit(ch = fgetc(sourceFile)) &&
                   index < MAX_TOKEN_SIZE - 1)
            {
                token.lexeme[index++] = ch;
            }
        }
        else
        {
            while (isdigit(ch) && index < MAX_TOKEN_SIZE - 1)
            {
                token.lexeme[index++] = ch;
                ch = fgetc(sourceFile);
            }
        }

        token.lexeme[index] = '\0';
        if (ch != EOF)
            ungetc(ch, sourceFile);

        token.type = CONSTANT;
        return token;
    }

    /* ---------- STRING ---------- */
    if (ch == '"')
    {
        token.lexeme[index++] = '"';

        while ((ch = fgetc(sourceFile)) != EOF &&
               index < MAX_TOKEN_SIZE - 1)
        {
            token.lexeme[index++] = ch;
            if (ch == '"')
                break;
        }

        token.lexeme[index] = '\0';
        token.type = STRING_LITERAL;
        return token;
    }

    /* ---------- CHARACTER ---------- */
    if (ch == '\'')
    {
        token.lexeme[index++] = '\'';

        ch = fgetc(sourceFile);
        token.lexeme[index++] = ch;

        if (ch == '\\')
        {
            ch = fgetc(sourceFile);
            token.lexeme[index++] = ch;
        }

        ch = fgetc(sourceFile);
        token.lexeme[index++] = '\'';

        token.lexeme[index] = '\0';
        token.type = CONSTANT;
        return token;
    }

    /* ---------- OPERATOR ---------- */
    if (strchr("+-*/%=!<>|&^", ch))
    {
        token.lexeme[0] = ch;
        token.lexeme[1] = '\0';
        token.type = OPERATOR;
        return token;
    }

    /* ---------- SPECIAL CHARACTER ---------- */
    if (isSpecialCharacter(ch))
    {
        token.lexeme[0] = ch;
        token.lexeme[1] = '\0';
        token.type = SPECIAL_CHARACTER;
        return token;
    }

    token.lexeme[0] = ch;
    token.lexeme[1] = '\0';
    token.type = UNKNOWN;
    return token;
}
