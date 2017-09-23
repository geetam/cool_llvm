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
%x CLASS_DEF
%x CLASS_NAME_EXTRCTD
%x CLASS_INHERITS
%x IDEN_DEC_ENC
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
WHITESPACE     [ \n\r\t\v\f]+
SINGLE_LINE_COMMENT_START   --
NEWLINE         \n
MULTI_LINE_COMMENT_START    \(\*
MULTI_LINE_COMMENT_END      \*\)
STR_TOK      ".+"
INT_TOK         [0-9]+
IDENTIFIER      [a-z_A-Z]+([a-z_A-Z0-9]*)

LETTER          [a-zA-Z]
%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */
  
" " {}
{NEWLINE} {
    curr_lineno++;
}
          

<INSIDE_SINGLE_LINE_COMMENT>{NEWLINE}   {
    BEGIN(INITIAL);
}
<INSIDE_SINGLE_LINE_COMMENT>(.|\n)  {}

<INSIDE_MULTI_LINE_COMMENT>{MULTI_LINE_COMMENT_END}     BEGIN(INITIAL);

<INSIDE_MULTI_LINE_COMMENT>(.)  {}
<INSIDE_MULTI_LINE_COMMENT>(\n) {
    curr_lineno++;
}

{CLASS} {
    BEGIN(CLASS_DEF);
    return (CLASS);
}


<CLASS_DEF>{WHITESPACE} {}
<CLASS_DEF>{IDENTIFIER} {
    cool_yylval.symbol = stringtable.add_string(yytext);
    BEGIN(CLASS_NAME_EXTRCTD);
    return TYPEID;
}
<CLASS_NAME_EXTRCTD>{WHITESPACE} {}
<CLASS_NAME_EXTRCTD>"{" {
    BEGIN(INITIAL);
    return '{';
}
<CLASS_NAME_EXTRCTD>{INHERITS} {
    BEGIN(CLASS_INHERITS);
    return INHERITS;
}
<CLASS_INHERITS>{WHITESPACE} {}
<CLASS_INHERITS>{IDENTIFIER} {
    cool_yylval.symbol = stringtable.add_string(yytext);
    BEGIN(INITIAL);
    return TYPEID;
}

{SINGLE_LINE_COMMENT_START} {   BEGIN(INSIDE_SINGLE_LINE_COMMENT); }
{MULTI_LINE_COMMENT_START}  {   BEGIN(INSIDE_MULTI_LINE_COMMENT); }/*




(?<={CLASS}{WHITESPACE}){IDENTIFIER}(?={INHERITS}{IDENTIFIER})*/


{IDENTIFIER}/{WHITESPACE}:  {
    cool_yylval.symbol = stringtable.add_string(yytext);
    BEGIN(IDEN_DEC_ENC);
    return OBJECTID;
}
{IDENTIFIER}/\({WHITESPACE}{IDENTIFIER}{WHITESPACE}:    {
    cool_yylval.symbol = stringtable.add_string(yytext);
    BEGIN(METH_DECL_ENC);
}
{IDENTIFIER}    {
    cool_yylval.symbol = stringtable.add_string(yytext);
    return OBJECTID;
}
                
<IDEN_DEC_ENC>":"   {
    return ':';
}

<IDEN_DEC_ENC>{WHITESPACE} {}
<IDEN_DEC_ENC>{IDENTIFIER}  {
    cool_yylval.symbol = stringtable.add_string(yytext);
    BEGIN(INITIAL);
    return TYPEID;
}

<METH_DECL_ENC>\(   {   return '('; }
<METH_DECL_ENC>{WHITESPACE} {
                            }
<METH_DECL_ENC>:    {
    return ':';
}
<METH_DECL_ENC>{IDENTIFIER} {
     cool_yylval.symbol = stringtable.add_string(yytext);
     return OBJECTID;
}

{STR_TOK}   { 
    cool_yylval.symbol = inttable.add_string(yytext);
    return {STR_CONST};
}

{INT_TOK}   {
    cool_yylval.symbol = inttable.add_string(yytext);
    return {INT_CONST};
}
                    


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

