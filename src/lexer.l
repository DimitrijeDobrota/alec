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
^[\t ]*

^[\t ]*"//".* { yylval.n = strdup(yytext); return COMMENT; }

,                        { return COMMA; }
[^,\n]*                  {
    char *p = yytext + strlen(yytext) - 1;
    while(isspace(*p)) *p-- = '\0';
	while(*yytext && isspace(*yytext)) yytext++;
	yylval.n = strdup(yytext); return LITERAL;
}

%%

