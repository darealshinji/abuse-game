/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com or
 *  Jonathan Clark.
 */

#ifndef __PARSE_HPP_
#define __PARSE_HPP_


enum  { sEND,sNUMBER,sSTRING,sWORD,sOPERATOR,sLEFT_BRACE,sRIGHT_BRACE,
        sLEFT_PAREN,sRIGHT_PAREN,sASSIGNMENT,sCOMMA  } ;
extern char *ttype[];

void expect(int thing, int type, char *where);
void skip_space(char *&s);
int get_token(char *&s, char *buffer);
int token_type(char *s);
void next_token(char *&s);
int get_number(char *&s);
void get_filename(char *&s, char *buffer);
void match_right(char *&s);

#endif
