#pragma once

#include <string>

#define yyFlexLexer yy_alec_FlexLexer
#if !defined(yyFlexLexerOnce)
#  include <FlexLexer.h>
#endif

#include "location.hpp"
#include "parser.hpp"

namespace alec
{

class driver : public yy_alec_FlexLexer
{
  int m_current_line = 0;

  parser::semantic_type* m_yylval = nullptr;
  location_t* m_yylloc = nullptr;

  void copy_location()
  {
    *m_yylloc = location_t(m_current_line, m_current_line);
  }

public:
  driver(std::istream& ins, const bool debug)
      : yy_alec_FlexLexer(&ins)
  {
    yy_alec_FlexLexer::set_debug(static_cast<int>(debug));
  }

  int yylex(parser::semantic_type* yylval, location_t* lloc);
};

}  // namespace alec
