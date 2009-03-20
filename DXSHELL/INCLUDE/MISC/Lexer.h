/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#pragma once

#include <stack>
#include <string>

#include <misc/Exception.h>

class LexException : public CException
{
public:
	LexException(const std::string &text):CException(text) {};
};

class Lexer
{
public:
	Lexer(void);
	~Lexer(void);

	void setText(const char* text, int textlength=-1);
	const char* getCurrentPointer() {return &m_text[m_pos];};
	int getPosition() {return m_pos;};
	void setPosition( int pos) {m_pos=pos;};
	void setMaxPosition (int maxpos) {m_pos.setMax(maxpos);};
	int getMaxPosition() {return m_pos.getMax();};

	void setCaseInsensitive();

	void skipUntil(char c);
	void skipBeyondNext(char c);
	void skipUntil(const char* text);
	void skipBeyondNext(const char* text);
	void skipWhiteSpace();
	void skipUntilWhiteSpace();

	void skipNextBlock(char open='{', char close='}');
	int  getEndOfNextBlock(char open='{', char close='}');
	void enterBlock(char open='{', char close='}');
	void exitBlock();

	double readDouble();
	int	 readInteger();
	int  readQuotedString(char* dest);
	int	 readUnquotedString(char* dest);
	int	 readIdentifier(char *dest);
	int  readLine(char *dest);
	int  readStringUntil(char c, char *dest);
	int  readStringUntilAndSkip(char c, char *dest);

	bool matchDigit();
	bool matchAlpha();
	bool matchAlphaNum();
	bool match(char c);
	bool matchAndSkip(char c);
	bool match(const char *text);
	bool matchAndSkip(const char *text);

	bool tryFind(const char *text);
	bool tryFindAndSkip (const char *text);

	void nextToken();
	void nextLine();

	void pushPosition();
	void popPosition();

protected:

 class maxint // used to conveniently forget about checking for EOF
 {
  public:
	inline maxint() : val(0), maxval(0) {};
	inline maxint(int value, int maxvalue) : val(value), maxval(maxvalue) {check();}
	//inline maxint(const maxint &mint2){val=mint2.val;}; // for temporary objects only 
	void setMax(int newmaxval) {maxval=newmaxval;};
	int getMax() {return maxval;};

	inline  operator int() {return val;}
	inline maxint& operator = (int newval) {val=newval;check();return *this;}
	inline maxint& operator = (const maxint &mint2) {val=mint2.val; maxval=mint2.maxval; return *this;};
	inline maxint& operator ++ () { val++; check(); return *this;};
	inline maxint operator ++ (int) { maxint temp=*this; ++(*this); check(); return temp;};
	inline maxint operator + (int val2) const { return maxint(val+val2,maxval);};
	inline maxint& operator += (int val2) {val+=val2; check(); return *this;};
    
 protected:
	inline void check() const
	{
		if (val>maxval)
			throw LexException("Lexer::maxint:: val> maxval\nprobably tried to read beyond current block");
	}
	int val;
	int maxval;
 };

	char toLower(char c);

	maxint	m_pos;
	std::string	m_text;
	std::stack<maxint> blockstack;
};
