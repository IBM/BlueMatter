%{
static int c2x(char c)
{
  if ( c >= '0' && c <= '9' ) return c - '0' ;
  if ( c >= 'a' && c <= 'f' ) return c - 'a' + 10 ;
  return 0 ;
}
%}

%s SHOWING

%%
"SendTo_BaseFx >" { BEGIN SHOWING ; }
"<"               { BEGIN 0 ; }
<SHOWING>[0123465789abcdef]{2}  {
                                  printf("%c",c2x(yytext[0])*16 + c2x(yytext[1])) ;
                                }
.   { }
\n  { }
%%
