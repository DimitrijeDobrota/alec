%option noyywrap nodefault yylineno

%{
    #include "generator.h"
    #include "parser.h"
    #include <ctype.h>
%}

LINE_END (\n|\r|\r\n)

%x GEN
%x LAST

%%


"%%"{LINE_END}     { BEGIN GEN; return SWITCH; }
.*{LINE_END}       { yylval.n = strdup(yytext); return BEFORE; }

<GEN>{LINE_END}                  { return EOL; }
<GEN>^[\t ]*{LINE_END}           { return EOL; }
<GEN>^[\t ]*\|*[\t ]*{LINE_END}  { return EOL; }

<GEN>^[\t ]*"//".* { yylval.n = strdup(yytext); return COMMENT; }

<GEN>, { return COMMA; }

<GEN>[^,\n]*                  {
    char *p = yytext + strlen(yytext) - 1;
    while(isspace(*p)) *p-- = '\0';
	while(*yytext && isspace(*yytext)) yytext++;
	yylval.n = strdup(yytext); return LITERAL;
}

<GEN>"%%"{LINE_END}        { BEGIN LAST; return SWITCH; }

<LAST>.*{LINE_END}       { yylval.n = strdup(yytext); return AFTER; }

%%

