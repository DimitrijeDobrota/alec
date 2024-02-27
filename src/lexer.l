%option noyywrap nodefault yylineno

%{
    #include "generator.h"
    #include "parser.h"
    #include <ctype.h>
%}

LINE_END (\n|\r|\r\n)

%%

{LINE_END}               { return EOL; }
^[\t ]*{LINE_END}        { return EOL; }
,                        { return COMMA; }
[^,\n]*                  {
	while(*yytext && isspace(*yytext)) yytext++;
	yylval.n = strdup(yytext); return LITERAL;
}

%%

