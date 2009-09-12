/*
(c) 2002-2009 by Mathias Heyer
*/

#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED
#ifdef _MSC_VER
#	pragma once
#endif

#include "misc/Exception.h"


#include <stack>
#include <string>
#include <locale>

/*	LexException is raised whenever the user is trying to read beyond the currently set block.
	EOF is also signaled this way.
	There is currently no way to tell the difference between "end of block" and "error in parsing"	
*/
class LexException : public CException
{
public:
	LexException(){};
};

/*	Lexer is a utility to parse ASCII files.
	It does not divide the text into tokens. Instead, the user has to explicitely parse the text in a straight-forward manner.
	Therefore Lexer should be only used in files that do not contain a deep hierarchy, like c++ source or similar.
	Lexer will automatically skip C and C++ comments and other whitespaces.
*/

class Lexer
{
public:
	Lexer(void);
	~Lexer(void);

	void setText(const char* text, int textlength=-1);
	void setText(const std::string &text);
	const std::string getText() const {return m_text;}
	const char* getCurrentPointer() {return &m_text[(int)m_pos];};
	int getPosition() {return m_pos;};
	void setPosition( int pos) {m_pos=pos;};
	void setMaxPosition (int maxpos) {m_pos.setMax(maxpos);};
	int getMaxPosition() {return m_pos.getMax();};

	void setCaseSensitive(bool sensitive);

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

	char	readChar();
	double readDouble();
	int	 readInteger();
	int	 readString(char *dest);
	int  readQuotedString(char* dest);
	int	 readUnquotedString(char* dest);
	int	 readIdentifier(char *dest);
	int  readFilename(char *dest);
	int  readLine(char *dest);
	int  readStringUntil(char c, char *dest);
	int  readStringUntilAndSkip(char c, char *dest);
	int	 readNumber(char *dest);

	bool matchDigit();
	bool matchAlpha();
	bool matchAlphaNum();
	bool matchIdentifier(const char *identifier);	//take care to provide a proper identifier!
	bool matchAndSkipIdentifier(const char *identifier);

	bool match(char c);
	bool matchAndSkip(char c);
	bool match(const char *text);
	bool matchAndSkip(const char *text);
	bool matchWhitespace();
	bool matchWhitespaceAndSkip();

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

	inline  operator int() const {return val;}

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
			throw LexException();
	}
	int val;
	int maxval;
 };

 class LexInput
 {
 public:
	 LexInput(const char *buffer=0, bool casesensitive=false);

	 void setBuffer(const char *buffer) ;
	 const char *getBuffer() const  ;

	void setCaseSensitive(bool sensitive);
	void copy(char *dest, int startpos, int length) const;
	char operator [] (const int pos) const;

protected:

	 const char		*m_buffer;
	 bool			m_casesensitive;
	 
	 char			m_tolower[256];
 };

	maxint				m_pos;
	std::string			m_text;
	LexInput			m_input;
	std::stack<maxint>	m_blockstack;

	int	m_isalpha[256];
	int	m_isalphanum[256];
	int	m_isdigit[256];
	int	m_isspace[256];
};

#endif

