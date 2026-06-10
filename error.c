/* error.c
 * Very simple error checks:
 *  - unterminated block comments
 *  - unterminated string literals
 *  - invalid / unterminated character literals
 *  - basic bracket balance
 *  - basic numeric constant validation
 *  - unknown characters
 */

 #include <stdio.h>
 #include <ctype.h>
 #include <string.h>
 #include "lexer.h"
 
 #define NUM_BUF_SIZE  MAX_TOKEN_SIZE
 
 int checkSourceErrors(const char* fileName)
 {
     FILE *fp;
     char  numBuf[NUM_BUF_SIZE];
     int   ch;
     int   line = 1;
     int   idx;
 
     int   roundCount  = 0;
     int   curlyCount  = 0;
     int   squareCount = 0;
 
     fp = fopen(fileName, "r");
     if (fp == NULL)
     {
         printf("Error: cannot open file %s\n", fileName);
         return 1;
     }
 
     while ((ch = fgetc(fp)) != EOF)
     {
         if (ch == '\n')
         {
             line++;
         }
 
         if (isspace(ch))
         {
             continue;
         }
 
         /* preprocessor line */
         if (ch == '#')
         {
             while ((ch = fgetc(fp)) != EOF && ch != '\n')
             {
             }
 
             if (ch == '\n')
             {
                 line++;
             }
 
             continue;
         }
 
         /* comments */
         if (ch == '/')
         {
             int next = fgetc(fp);
 
             if (next == '/')
             {
                 while ((ch = fgetc(fp)) != EOF && ch != '\n')
                 {
                 }
                 if (ch == '\n')
                 {
                     line++;
                 }
                 continue;
             }
             else if (next == '*')
             {
                 int prev = 0;
                 int closed = 0;
 
                 while ((ch = fgetc(fp)) != EOF)
                 {
                     if (ch == '\n')
                     {
                         line++;
                     }
 
                     if (prev == '*' && ch == '/')
                     {
                         closed = 1;
                         break;
                     }
 
                     prev = ch;
                 }
 
                 if (!closed)
                 {
                     printf("Lexical Error: unterminated block comment before line %d\n",
                            line);
                     fclose(fp);
                     return 1;
                 }
 
                 continue;
             }
             else
             {
                 if (next != EOF)
                 {
                     ungetc(next, fp);
                 }
             }
         }
 
         /* string literal */
         if (ch == '"')
         {
             int startLine = line;
             int c;
             int esc;
 
             while (1)
             {
                 c = fgetc(fp);
 
                 if (c == EOF)
                 {
                     printf("Lexical Error: unterminated string literal starting at line %d\n",
                            startLine);
                     fclose(fp);
                     return 1;
                 }
 
                 if (c == '\n')
                 {
                     line++;
                     printf("Lexical Error: unterminated string literal starting at line %d\n",
                            startLine);
                     fclose(fp);
                     return 1;
                 }
 
                 if (c == '\\')
                 {
                     esc = fgetc(fp);
                     if (esc == EOF)
                     {
                         printf("Lexical Error: unterminated string literal starting at line %d\n",
                                startLine);
                         fclose(fp);
                         return 1;
                     }
                     continue;
                 }
 
                 if (c == '"')
                 {
                     break;
                 }
             }
 
             continue;
         }
 
         /* character literal */
         if (ch == '\'')
         {
             int c1 = fgetc(fp);
             int c2;
 
             if (c1 == EOF || c1 == '\n')
             {
                 printf("Lexical Error: unterminated character literal at line %d\n",
                        line);
                 fclose(fp);
                 return 1;
             }
 
             c2 = fgetc(fp);
             if (c2 != '\'')
             {
                 printf("Lexical Error: invalid character literal at line %d\n",
                        line);
                 fclose(fp);
                 return 1;
             }
 
             continue;
         }
 
         /* brackets */
         if (ch == '(')
         {
             roundCount++;
             continue;
         }
         if (ch == ')')
         {
             roundCount--;
             if (roundCount < 0)
             {
                 printf("Syntax Error: extra ')' at line %d\n", line);
                 fclose(fp);
                 return 1;
             }
             continue;
         }
         if (ch == '{')
         {
             curlyCount++;
             continue;
         }
         if (ch == '}')
         {
             curlyCount--;
             if (curlyCount < 0)
             {
                 printf("Syntax Error: extra '}' at line %d\n", line);
                 fclose(fp);
                 return 1;
             }
             continue;
         }
         if (ch == '[')
         {
             squareCount++;
             continue;
         }
         if (ch == ']')
         {
             squareCount--;
             if (squareCount < 0)
             {
                 printf("Syntax Error: extra ']' at line %d\n", line);
                 fclose(fp);
                 return 1;
             }
             continue;
         }
 
         /* identifier / keyword (just skip) */
         if (isalpha(ch) || ch == '_')
         {
             while ((ch = fgetc(fp)) != EOF &&
                    (isalnum(ch) || ch == '_'))
             {
             }
 
             if (ch != EOF)
             {
                 ungetc(ch, fp);
             }
 
             continue;
         }
 
         /* numeric constant: simple hex / octal / decimal */
         if (isdigit(ch))
         {
             int i;
 
             idx = 0;
             numBuf[idx++] = (char)ch;
 
             while ((ch = fgetc(fp)) != EOF && isalnum(ch))
             {
                 if (idx < NUM_BUF_SIZE - 1)
                 {
                     numBuf[idx++] = (char)ch;
                 }
             }
 
             numBuf[idx] = '\0';
 
             if (ch != EOF)
             {
                 ungetc(ch, fp);
             }
 
             /* hex: 0x... */
             if (numBuf[0] == '0' &&
                 (numBuf[1] == 'x' || numBuf[1] == 'X'))
             {
                 if (numBuf[2] == '\0')
                 {
                     printf("Lexical Error: invalid hexadecimal constant '%s' at line %d\n",
                            numBuf, line);
                     fclose(fp);
                     return 1;
                 }
 
                 i = 2;
                 while (numBuf[i] != '\0')
                 {
                     if (!isxdigit((unsigned char)numBuf[i]))
                     {
                         printf("Lexical Error: invalid hexadecimal constant '%s' at line %d\n",
                                numBuf, line);
                         fclose(fp);
                         return 1;
                     }
                     i++;
                 }
 
                 continue;
             }
 
             /* octal: starts with 0 and more digits */
             if (numBuf[0] == '0' && numBuf[1] != '\0')
             {
                 i = 1;
                 while (numBuf[i] != '\0')
                 {
                     if (numBuf[i] < '0' || numBuf[i] > '7')
                     {
                         printf("Lexical Error: invalid octal constant '%s' at line %d\n",
                                numBuf, line);
                         fclose(fp);
                         return 1;
                     }
                     i++;
                 }
 
                 continue;
             }
 
             /* decimal: all digits */
             i = 0;
             while (numBuf[i] != '\0')
             {
                 if (!isdigit((unsigned char)numBuf[i]))
                 {
                     printf("Error: invalid numeric token '%s' at line %d\n",
                            numBuf, line);
                     fclose(fp);
                     return 1;
                 }
                 i++;
             }
 
             continue;
         }
 
         /* two-character operators (just skip, no validation here) */
         if (strchr("=!<>|&+-", ch) != NULL)
         {
             int next = fgetc(fp);
 
             if (!((ch == '=' && next == '=') ||
                   (ch == '!' && next == '=') ||
                   (ch == '<' && next == '=') ||
                   (ch == '<' && next == '<') ||
                   (ch == '>' && next == '=') ||
                   (ch == '>' && next == '>') ||
                   (ch == '&' && next == '&') ||
                   (ch == '|' && next == '|') ||
                   (ch == '+' && next == '+') ||
                   (ch == '-' && next == '-')))
             {
                 if (next != EOF)
                 {
                     ungetc(next, fp);
                 }
             }
 
             continue;
         }
 
         /* single operator using project helper */
         {
             char op[2];
 
             op[0] = (char)ch;
             op[1] = '\0';
 
             if (isOperator(op))
             {
                 continue;
             }
         }
 
         /* special character using project helper */
         if (isSpecialCharacter((char)ch))
         {
             continue;
         }
 
         /* unknown */
         printf("Unknown token: %c at line %d\n", ch, line);
         fclose(fp);
         return 1;
     }
 
     /* final bracket checks */
     if (roundCount != 0)
     {
         printf("Syntax Error: unbalanced parentheses, missing ')'\n");
         fclose(fp);
         return 1;
     }
 
     if (curlyCount != 0)
     {
         printf("Syntax Error: unbalanced braces, missing '}'\n");
         fclose(fp);
         return 1;
     }
 
     if (squareCount != 0)
     {
         printf("Syntax Error: unbalanced brackets, missing ']'\n");
         fclose(fp);
         return 1;
     }
 
     fclose(fp);
     return 0;
 }
 