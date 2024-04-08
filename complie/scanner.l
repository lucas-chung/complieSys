%option noyywrap
%{
#include <stdio.h>   
#include <stdlib.h>
#include <string.h>
int old_status;
void yyerror(char *s, ...);

%}
%x COMMENT
%%
	/* 下面都是正则表达式 */
int         { return INT; }
break       { return BREAK; }
continue    { return CONTINUE;}
const       { return CONST; }
else        { return ELSE; }
while         { return WHILE; }
if          { return IF; }
return      { return RETURN; }
void		{return VOID;}
	
	/* user variable */
[a-zA-Z][a-zA-Z0-9_]*       { yylval.strval = strdup(yytext); return Ident; }
	/* integer numbers */
[0-9]+                    { yylval.intval = atoi(yytext); return IntConst; }
\"(\\.|\"\"|[^"\n"])*\"     { yylval.strval = strdup(yytext); return STRING; }
	/* C-type strings */
\"(\\.|[^"\n])*$            { yyerror("Unterminated string %s", yytext); }

    /* operators */
[-+/%*!]       { return yytext[0]; }
"&&"                    { return AND; }
"||"                    { return OR; }
"="                     { return ASSIGN; }
">="					{return NL;}
"<="					{return NG;}
">"					{return GT;}
"<"					{return LS;}
"=="					{return EQ;}
"!="					{return NEQ;}
"("						{return LRBK;}
")"						{return RRBK;}
"["						{return LSBK;}
"]"						{return RSBK;}
"{"						{return LCB;}
"}"						{return RCB;}
","						{return COMMA;}
";"						{return SMC;}
    /* comments */
"//".*;
"/*"                    { old_status = YY_START; BEGIN COMMENT; }
<COMMENT>"*/"           { BEGIN old_status; }
	/* space & tab */
[ \t\n]
	/* prevent flex jam */
.           { yyerror("something goes wrong...\n"); }

%%
