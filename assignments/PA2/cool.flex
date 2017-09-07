/*
 *  The scanner definition for COOL.
 */

 
 
 

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
 
%option noyywrap
%x INSIDE_MULTI_LINE_COMMENT 
%x INSIDE_SINGLE_LINE_COMMENT 
%{

#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */

%}

/*
 * Define names for regular expressions here.
 */

DARROW          =>
CLASS           (?i:class)
ELSE            (?i:else)
FI              (?i:fi)
IF              (?i:if)
IN              (?i:in)
INHERITS        (?i:inherits)
ISVOID          (?i:isvoid)
LET             (?i:let)
LOOP            (?i:loop)
POOL            (?i:pool)
THEN            (?i:then)
WHILE           (?i:while)
CASE            (?i:case)
ESAC            (?i:esac)
NEW             (?i:new)
OF              (?i:of)
NOT             (?i:not)
TRUE            t(?i:rue)
FALSE           f(?i:alse)
WHITE_SPACE     [ \n\r\t\v\f]+
SINGLE_LINE_COMMENT_START   --
NEWLINE         \n
MULTI_LINE_COMMENT_START    \(\*
MULTI_LINE_COMMENT_END      \*\)
STR_TOK      ".+"
INT_TOK         "[0-9]+"
IDENTIFIER      [a-zA-Z]+([a-zA-Z0-9]*)

LETTER          [a-zA-Z]
%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */
  
  

<INSIDE_SINGLE_LINE_COMMENT>{NEWLINE}   {
                                            BEGIN(INITIAL);
                                        }
<INSIDE_SINGLE_LINE_COMMENT>(.|\n)  {}

<INSIDE_MULTI_LINE_COMMENT>{MULTI_LINE_COMMENT_END}     BEGIN(INITIAL);

<INSIDE_MULTI_LINE_COMMENT>(.|\n)  {}



{SINGLE_LINE_COMMENT_START} {   BEGIN(INSIDE_SINGLE_LINE_COMMENT); }
{MULTI_LINE_COMMENT_START}  {   BEGIN(INSIDE_MULTI_LINE_COMMENT); }/*
(?<={CLASS}{WHITE_SPACE}){IDENTIFIER}(?={INHERITS}{IDENTIFIER})
*/
.(?=a)    {
                                    printf(yytext);
                                    cool_yylval.symbol = stringtable.add_string(yytext);
                                    return (TYPEID);
                                    }

{STR_TOK}         { 
                    cool_yylval.symbol = inttable.add_string(yytext);
                    return {STR_CONST};
                  }

{INT_TOK}         {
                    cool_yylval.symbol = inttable.add_string(yytext);
                    return {INT_CONST};
                  }
                    
{DARROW}		  { return (DARROW); }
{CLASS}           { return (CLASS); }
{ELSE}            { return (ELSE); }
{FI}              { return (FI); }
{IF}              { return (IF); }
{IN}              { return (IN); }
{INHERITS}        { return (INHERITS); }
{ISVOID}          { return (ISVOID); }
{LET}             { return (LET); }
{LOOP}            { return (LOOP); }
{POOL}            { return (POOL); }
{THEN}            { return (THEN); }
{WHILE}           { return (WHILE); }
{CASE}            { return (CASE); }
{ESAC}            { return (ESAC); }
{NEW}             { return (NEW); }
{OF}              { return (OF); }
{NOT}             { return (NOT); }/*
TRUE            t(?i:rue)  
FALSE           f(?i:alse)
WHITE_SPACE     (\n | \r | \t | \v | \f)+*/
\+                { return  '+'; }
"/"               { return  '/'; }
"-"               { return  '-'; } 
"*"               { return  '*'; } 
"="               { return  '='; } 
"<"               { return  '<'; } 
"."               { return  '.'; }
"~"               { return  '~'; }
","               { return  ','; } 
";"               { return  ';'; } 
":"               { return  ':'; }
"("               { return  '('; } 
")"               { return  ')'; } 
"@"               { return  '@'; }
"{"               { return  '{'; } 
"}"               { return  '}'; } 


 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */


 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */


%%

// int main()
// {
//     int x;
//     while( (x = yylex()) != 0 )
//     {
//         printf("encountered token %d", x);
//     }
//     return 0;
// }
