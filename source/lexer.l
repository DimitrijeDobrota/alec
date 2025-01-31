%{
    #include <stack>
    #include "driver.hpp"
    #include "parser.hpp"

    using namespace alec;

    #undef  YY_DECL
    #define YY_DECL int driver::yylex(parser::semantic_type* yylval, location_t *const lloc)

    #define YY_USER_INIT m_yylloc = lloc;
    #define YY_USER_ACTION copy_location();

    std::stack<int> mode_st;

    #define BEGIN_MODE(mode) do {                                        \
        if(yy_flex_debug) std::cerr<<"Starting mode: "<<mode<<std::endl; \
        mode_st.push(YY_START);                                          \
        BEGIN((mode));                                                   \
    } while(0);

    #define END_MODE() do {                                                           \
        if(yy_flex_debug) std::cerr<<"Returning to mode: "<<mode_st.top()<<std::endl; \
        BEGIN(mode_st.top());                                                          \
        mode_st.pop();                                                                \
    } while(0);
%}

%option c++ noyywrap debug nodefault
%option yyclass = "driver"
%option prefix = "yy_alec_"


LINE_END (\n|\r|\r\n)
SWITCH_BEGIN "/\*%%\*//\*"{LINE_END}
SWITCH_END "\*//\*%%\*/"{LINE_END}

%x GEN LAST

%%

%{
    using Token = parser::token;
%}


{SWITCH_BEGIN}     { BEGIN_MODE(GEN); return Token::SWITCH; }
.*{LINE_END}       {
    yylval->emplace<std::string>(yytext);
    return Token::PROLOGUE; 
}

<GEN>{LINE_END}                  { return Token::EOL; }
<GEN>^[\t ]*{LINE_END}           { return Token::EOL; }
<GEN>^[\t ]*\|*[\t ]*{LINE_END}  { return Token::EMPTY; }

<GEN>^[\t ]*"//".* {
    yylval->emplace<std::string>(yytext);
    return Token::COMMENT;
}

<GEN>, { return Token::COMMA; }

<GEN>[^,\n]*                  {
    char *p = yytext + strlen(yytext) - 1;
    while(isspace(*p)) *p-- = '\0';
	while(*yytext && isspace(*yytext)) yytext++;
	yylval->emplace<std::string>(yytext);
    return Token::LITERAL;
}

<GEN>{SWITCH_END} {
    BEGIN_MODE(LAST);
    return Token::SWITCH;
}

<LAST>.*{LINE_END} {
    yylval->emplace<std::string>(yytext);
    return Token::EPILOGUE;
}

%%

