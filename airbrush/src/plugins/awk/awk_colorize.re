/*
    awk_colorize.re
    Copyright (C) 2009 zg

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "abplugin.h"
#include "../abpairs.h"
#include "../plugins/awk/abawk.h"

#ifdef UNICODE
typedef unsigned short UTCHAR;
#else
typedef unsigned char UTCHAR;
#endif

#define YYCTYPE unsigned long
#define YYCURSOR yycur
#define YYLIMIT yyend
#define YYMARKER yytmp
#define YYFILL(n)

/*!re2c
any = [\U00000001-\U0000ffff];
O = [0-7];
D = [0-9];
L = [a-zA-Z_];
H = [a-fA-F0-9];
E = [Ee] [+-]? D+;
FS  = [fFlL];
IS  = [uUlL]*;
ESC = [\\] ([abfnrtv?'"\\] | "x" H+ | O+);
*/

void WINAPI _export Colorize(int index,struct ColorizeParams *params)
{
  const UTCHAR *commentstart;
  const UTCHAR *line;
  int linelen,startcol;
  int lColorize=0;
  int state_data=PARSER_CLEAR;
  int *state=&state_data;
  int state_size=sizeof(state_data);
  const UTCHAR *yycur,*yyend,*yytmp=NULL,*yytok=NULL;
  struct PairStack *hl_state=NULL;
  int hl_row; int hl_col;
  if(params->data_size>=sizeof(state_data))
  {
    state=(int *)(params->data);
    state_size=params->data_size;
  }
  Info.pGetCursor(&hl_row,&hl_col);
  for(int lno=params->startline;lno<params->endline;lno++,yytok=NULL)
  {
    startcol=(lno==params->startline)?params->startcolumn:0;
    if(((lno%Info.cachestr)==0)&&(!startcol))
      if(!Info.pAddState(params->eid,lno/Info.cachestr,state_size,(unsigned char *)state)) goto colorize_exit;
    if(lno==params->topline) lColorize=1;
    if(lColorize&&(!startcol)) Info.pAddColor(lno,-1,1,0,0);
    line=(const UTCHAR*)Info.pGetLine(lno,&linelen);
    commentstart=line+startcol;
    yycur=line+startcol;
    yyend=line+linelen;
colorize_clear:
    if(yytok) if(params->callback) if(params->callback(0,lno,yytok-line,params->param)) goto colorize_exit;
    yytok=yycur;
    if(params->callback) if(params->callback(1,lno,yytok-line,params->param)) goto colorize_exit;
    if(state[0]==PARSER_STRING) goto colorize_string;
    if(state[0]==PARSER_REGEXP) goto colorize_regexp;
/*!re2c
  "#"
  { commentstart=yytok; goto colorize_comment; }
  ["]
  { state[0]=PARSER_STRING; commentstart=yytok; goto colorize_string; }
  [/]
  {
    if(lColorize) Info.pAddColor(lno,yytok-line,yycur-yytok,colors[HC_REGEXPS],colors[HC_REGEXPS+1]);
    state[0]=PARSER_REGEXP;
    commentstart=yycur;
    goto colorize_regexp;
  }
  "break"|"case"|"continue"|"default"|"delete"|"do"|"else"|"exit"|"for"|"if"|"in"|"switch"|"while"
  { if(lColorize) Info.pAddColor(lno,yytok-line,yycur-yytok,colors[HC_KEYWORD1],colors[HC_KEYWORD1+1]); goto colorize_clear; }
  L(L|D)*
  { goto colorize_clear; }
  ";"|"="|"+"|"-"|"/"|"*"|"&"|"|"|"^"|","|"."|":"|"!"|"~"|">"|"<"|"%"
  { if(lColorize) Info.pAddColor(lno,yytok-line,yycur-yytok,colors[HC_KEYWORD1],colors[HC_KEYWORD1+1]); goto colorize_clear; }
  "(" {PUSH_PAIR(0)}
  ")" {POP_PAIR(0,0)}
  "[" {PUSH_PAIR(1)}
  "]" {POP_PAIR(1,1)}
  "{" {PUSH_PAIR(2)}
  "}" {POP_PAIR(2,2)}
  [ \t\v\f]+ { goto colorize_clear; }
  [\000]
  {
    if(yytok==yyend) goto colorize_end;
    goto colorize_clear;
  }
  any
  {
    goto colorize_clear;
  }
*/

colorize_comment:
    yytok=yycur;
/*!re2c
  [\000]
  {
    if(yytok==yyend)
    {
      if(lColorize) Info.pAddColor(lno,commentstart-line,yycur-commentstart,colors[HC_COMMENT],colors[HC_COMMENT+1]);
      goto colorize_end;
    }
    goto colorize_comment;
  }
  any
  { goto colorize_comment; }
*/
colorize_string:
    yytok=yycur;
/*!re2c
  ESC
  { goto colorize_string; }
  ["]
  {
    if(lColorize) Info.pAddColor(lno,commentstart-line,yycur-commentstart,colors[HC_STRING],colors[HC_STRING+1]);
    state[0]=PARSER_CLEAR;
    goto colorize_clear;
  }
  [\000]
  { if(yytok==yyend) goto colorize_end; goto colorize_string; }
  any
  { goto colorize_string; }
*/
colorize_regexp:
    yytok=yycur;
/*!re2c
  ESC
  { goto colorize_regexp; }
  [/]
  {
    if(lColorize)
    {
      Info.pAddColor(lno,commentstart-line,yytok-commentstart,colors[HC_REGEXP],colors[HC_REGEXP+1]);
      Info.pAddColor(lno,yytok-line,yycur-yytok,colors[HC_REGEXPS],colors[HC_REGEXPS+1]);
    }
    state[0]=PARSER_CLEAR;
    goto colorize_clear;
  }
  [\000]
  { if(yytok==yyend) goto colorize_end; goto colorize_regexp; }
  any
  { goto colorize_regexp; }
*/
colorize_end:
    if(state[0]==PARSER_STRING)
      if(lColorize) Info.pAddColor(lno,commentstart-line,yyend-commentstart,colors[HC_STRING],colors[HC_STRING+1]);
    if(state[0]==PARSER_REGEXP)
      if(lColorize) Info.pAddColor(lno,commentstart-line,yyend-commentstart,colors[HC_REGEXP],colors[HC_REGEXP+1]);
  }
colorize_exit:
  PairStackClear(params->LocalHeap,&hl_state);
}