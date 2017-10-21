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
%x METH_DECL_ENC
%x DETECT_TYPE_ARG
%x INSIDE_METH_DECL_PAREN
%x DETECT_RET_TYPE

%{
#include <string.h>
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
STR_TOK         \"[^\"]*\"
INT_TOK         0|[1-9][0-9]*
IDENTIFIER      [a-z_A-Z]+([a-z_A-Z0-9]*)
OPTWHITE        [ \n\r\t\v\f]*
LETTER          [a-zA-Z]
ASSIGN          <-
%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */

{ASSIGN}          { return {ASSIGN}; }  
{DARROW}		  { return (DARROW); }/*
{CLASS}           { return (CLASS); }*/
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





" " {}
{NEWLINE} {
    curr_lineno++;
}
          

<INSIDE_SINGLE_LINE_COMMENT>{NEWLINE}   {
    BEGIN(INITIAL);
}
<INSIDE_SINGLE_LINE_COMMENT>(.|\n)  {}

<INSIDE_MULTI_LINE_COMMENT><<eof>>  {
    cool_yylval.error_msg = "EOF in comment";
    BEGIN(INITIAL);
    return ERROR;
}
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
{IDENTIFIER}/\([a-z A-Z_:,]*\){OPTWHITE}:    {
    cool_yylval.symbol = stringtable.add_string(yytext);
    BEGIN(METH_DECL_ENC);
    return OBJECTID;
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

<METH_DECL_ENC>\(   {
    BEGIN(INSIDE_METH_DECL_PAREN);
    return '(';
}
<METH_DECL_ENC>{WHITESPACE} {}

<METH_DECL_ENC>:    {
    BEGIN(DETECT_RET_TYPE);
    return ':';
}

<DETECT_RET_TYPE>{WHITESPACE}   {}
<DETECT_RET_TYPE>{IDENTIFIER}   {
    cool_yylval.symbol = stringtable.add_string(yytext);
    BEGIN(INITIAL);
    return TYPEID;
}

<INSIDE_METH_DECL_PAREN>{WHITESPACE} {}
<INSIDE_METH_DECL_PAREN>:    {
    BEGIN(DETECT_TYPE_ARG);
    return ':';
}
<INSIDE_METH_DECL_PAREN>{IDENTIFIER} {
     cool_yylval.symbol = stringtable.add_string(yytext);
     return OBJECTID;
}

<INSIDE_METH_DECL_PAREN>\)   {
    BEGIN(METH_DECL_ENC);
    return ')';
}

<DETECT_TYPE_ARG>{WHITESPACE} {}
<DETECT_TYPE_ARG>{IDENTIFIER}   {
    cool_yylval.symbol = stringtable.add_string(yytext);
    return TYPEID;
}

<DETECT_TYPE_ARG>\) {
  
    BEGIN(METH_DECL_ENC);
    return ')';
}

<DETECT_TYPE_ARG>,  {
    BEGIN(INSIDE_METH_DECL_PAREN);
    return ',';
}
    

{STR_TOK}   { 
    int len = strlen(yytext);
    char *match = strdup(yytext);
    for(int i = 1; i < len; i++)
    {
        match[i - 1] = match[i];
    }
    
    len -= 2;
    assert(len >= 0);
    match[len] = '\0';
    
    for(int i = 0; i < len; i++)
    {
        if(match[i] == '\\')
        {
            int toshift = 1;
            switch(match[i+1])
            {
                case '\\':
                    break;
                case 'n':
                    match[i] = '\n';
                    break;
                case 'b':
                    match[i] = '\b';
                    break;
                case 'f':
                    match[i] = '\f';
                    break;
                case '"':
                    match[i] = '"';
                    break;
                case '0':
                    match[i] = '\0';
                    break;
                case 't':
                    match[i] = '\t';
                    break;
                case 'r':
                    match[i] = '\r';
                    break;
                    
                default:
                    toshift = 0;
            }
            
            if(toshift)
            {
                for(int j = i + 2 ; j < len; j++)
                {
                    match[j - 1] = match[j];
                }
                
                len--;
                match[len] = '\0';
            }
        }
    }
    
    
    cool_yylval.symbol = stringtable.add_string(match);
    return {STR_CONST};
}

{INT_TOK}   {
    cool_yylval.symbol = inttable.add_string(yytext);
    return {INT_CONST};
}
                    

          
.   {

    cool_yylval.error_msg = yytext;
    return ERROR;
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

