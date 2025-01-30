%require "3.8.2"
%language "c++"

%code requires {
    #include <string>
    #include <cstdint>
    #include "location.hpp"
    #include "generator.h"

    namespace alec {
        class driver;
    } // namespace alec
}

%define api.namespace { alec }
%define api.parser.class { parser }
%define api.value.type variant
%define api.location.type { location_t }

%locations
%define parse.error detailed

%header
%verbose

%parse-param {driver &drv}
%parse-param {const bool debug}

%initial-action
{
    #if YYDEBUG != 0
        set_debug_level(debug);
    #endif
};

%code {
    #include "driver.hpp"

    namespace alec {
        template<typename RHS>
        void calcLocation(location_t &current, const RHS &rhs, const std::size_t n);

        std::vector<record> records;
        std::vector<std::string> epilogue;
        std::vector<std::string> prologue;
    } // namespace alec

    #define YYLLOC_DEFAULT(Cur, Rhs, N) calcLocation(Cur, Rhs, N)
    #define yylex drv.yylex
}

%left <char *> LITERAL COMMENT PROLOGUE EPILOGUE
%token EOL COMMA SWITCH EMPTY

%type <record> record
%type <std::vector<char*>> list items
%type <char *> name

%start document

%%

document: prologue grammar epilogue

prologue: %empty
      | prologue PROLOGUE { prologue.push_back($2); }
      ;

epilogue: SWITCH
     | epilogue EPILOGUE { epilogue.push_back($2); }
     ;

grammar: SWITCH
   | grammar EOL
   | grammar record { records.push_back(std::move($2)); }
   ;

record: name list list list { $$ = record($1, $2, $3, $4); }
      | COMMENT             { $$ = record($1, {}, {}, {}); }
      ;

name: LITERAL EOL   { $$ = $1; }
    ;

list: EMPTY       { $$ = {}; }
    | items EOL   { $$ = $1; }
    ;

items: LITERAL             { $$ = { $1 }; }
     | items COMMA LITERAL { $1.push_back($3); $$ = $1; }
     ;

%%

namespace alec {
    template<typename RHS>
    inline void calcLocation(location_t &current, const RHS &rhs, const std::size_t n)
    {
        current = location_t(YYRHSLOC(rhs, 0).first, YYRHSLOC(rhs, n).second);
    }

    void parser::error(const location_t &location, const std::string &message)
    {
        std::cerr << "Error at lines " << location << ": " << message << std::endl;
    }
} // namespace alec
