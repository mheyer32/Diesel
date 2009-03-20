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

#include "lexer.h"
#include <iostream>
#include <algorithm>

using namespace std;

Lexer::Lexer(void)
{
}

Lexer::~Lexer(void)
{
}
void Lexer::setText(const char* text, int textlength)
{
	// cleanup blockstack
	while(!blockstack.empty()) blockstack.pop();

	if (textlength!=-1)
	{
		m_text.assign(text, textlength);
	}
	else
	{
		m_text.assign(text);
	}

	m_pos=0;
	m_pos.setMax(m_text.size());

	skipWhiteSpace();
}
void Lexer::setCaseInsensitive()
{
	std::transform(m_text.begin(), m_text.end(),m_text.begin(), ::tolower);
}
void Lexer::skipWhiteSpace()
{
	while (true)
	{
		//FIXME: since m_text is an array of signed char, checking for >0x7F is somewhat useless
		while ((m_text[m_pos]<0x21) || (m_text[m_pos]>0x7F)) ++m_pos;
		
		if (m_text[m_pos]=='/')// possible start of comment
		{
			if (m_text[m_pos+1]=='/')	// its a C++ comment!
			{
				m_pos+=2;
				while (m_text[m_pos++]!='\n'); // skip to end of line
				continue; //continue skipping whitespace
			}
			else
			{
				if (m_text[m_pos+1]=='*') // its a C-comment!
				{
					m_pos+=2;
					while ((m_text[m_pos]!='*') || (m_text[m_pos+1]!='/')) // find end of C-comment 
					{
						++m_pos;
					}
					m_pos+=2;
					continue; //continue skipping whitespace
				}
			}
		}		
		break;
	}
}

void Lexer::nextLine()
{
	while (true)
	{
		while ((m_text[m_pos]!='\n') && (m_text[m_pos]!='/')) ++m_pos;
		if (m_text[m_pos]=='\n')
		{
			++m_pos;					
			return;	// found end of line
		}
		else if (m_text[m_pos]=='/')	// may be some comment
		{
			if (m_text[m_pos+1]=='/')	// C++ comment
			{
				m_pos+=2;
				while (m_text[m_pos++]!='\n');
				break; // found endof line
			}
			else
			{
				//FIXME: what about EOL in a c-comment ?
				if (m_text[m_pos+1]=='*') // C-comment
				{
					m_pos+=2;
					while ((m_text[m_pos]!='*') || (m_text[m_pos+1]!='/'))
					{
						++m_pos;
					}
					m_pos+=2;
					continue; //continue skipping whitespace
				}
			}
			++m_pos; // backslash was not start of a comment
		}
	}
}

void Lexer::skipUntilWhiteSpace()
{
	while ((m_text[m_pos]>=0x21) &&  (m_text[m_pos]<=0x80)) ++m_pos;
}

char Lexer::toLower(char c)
{
	if ((c<0x41) || (c> 0x5A)) return c;
	else return c|0x20;
}
void Lexer::skipUntil(char c)
{
	skipWhiteSpace();
	while (m_text[m_pos]!=c)
	{
		if (match('\"'))
		{
			++m_pos;
			while (!match('\"'))
				++m_pos;
		}
		++m_pos;
		skipWhiteSpace();
	}
}
void Lexer::skipBeyondNext(char c)
{	
	skipUntil(c);
	++m_pos;
}

bool Lexer::match(char c)
{
	return m_text[m_pos]==c;
}

bool Lexer::matchAndSkip(char c)
{
	if (m_text[m_pos]==c)
	{
		++m_pos;
		return true;
	}
	return false;
}

bool Lexer::matchDigit()
{
	return ((m_text[m_pos]>='0') && (m_text[m_pos]<='9')) ;
}

bool Lexer::matchAlpha()
{
	return (
			((m_text[m_pos]>='a') && (m_text[m_pos]<='z'))
			||((m_text[m_pos]>='A') && (m_text[m_pos]<='Z'))
			);
}
bool Lexer::matchAlphaNum()
{
	return matchAlpha()||matchDigit();
}
bool Lexer::match(const char *text)
{
	maxint temp=m_pos;
	try
	{
		while ((*text!=0)&&((*text)==(m_text[temp]))) //&&(*string2!=0) shouldn´t be needed
		{
			++text;
			++temp;
		};
	}
	catch (LexException &)
	{
		return false;	// if matching would need reading beyond end of block
	}

	if (!(*text)) return true;
	return false;
}
bool Lexer::matchAndSkip(const char *text)
{
	if (match(text))
	{
		m_pos+=(int)strlen(text);
		return true;
	}
	return false;
}

void Lexer::skipUntil(const char* text)
{
	skipUntil(text[0]);
	while(!match(text))
	{
		if (matchAndSkip('\"'))
		{
			while (!match('\"'))
				++m_pos;
		}
		++m_pos;
		skipUntil(text[0]);
	}
}
void Lexer::skipBeyondNext(const char* text)
{
	skipUntil(text[0]);
	while(!matchAndSkip(text))
	{
		if (matchAndSkip('\"'))
		{
			while (!match('\"'))
				++m_pos;
		}
		++m_pos;
		skipUntil(text[0]);
	}
}

int Lexer::readQuotedString(char* dest)
{
	int count=0;
	do
	{
		skipBeyondNext('"');
		while (m_text[m_pos]!='"')
		{
			dest[count++]=m_text[m_pos++];
		}

		++m_pos; //skip trailing '"'
		try
		{
			skipWhiteSpace();	
		}
		catch (LexException &)
		{
			break;
		};

	} while(match('\\'));// support concatenated multi-line strings
		
	dest[count]=0; // 

	return count;
}
int Lexer::readUnquotedString(char* dest)
{
	skipWhiteSpace();
	int count=0;
	try
	{
		while ((m_text[m_pos]>=0x21) &&  (m_text[m_pos]<0x7F))
		{
			dest[count++]=m_text[m_pos];
			++m_pos;
		}
	}
	catch (LexException &) {}; // don´t read beyond end of block

	dest[count]=0;
	return count;
}

int  Lexer::readStringUntil(char c, char *dest)
{
	skipWhiteSpace();
	int count=0;
	while (m_text[m_pos]!=c)
	{
		dest[count++]=m_text[m_pos++];
	}
	dest[count]=0;
	return count;
}

int  Lexer::readStringUntilAndSkip(char c, char *dest)
{
	int count=readStringUntil(c, dest);
	m_pos++;
	return count;
}

int	 Lexer::readIdentifier(char *dest)
{
	skipWhiteSpace();
	int count=0;

	try
	{
		if (match('_') || matchAlpha())
		{
			dest[count++]=m_text[m_pos];
			++m_pos;
			while (match('_')||matchAlphaNum())
			{
				dest[count++]=m_text[m_pos];
				++m_pos;
			}
		}
	}
	catch (LexException &) {}; // don´t read beyond end of block

	dest[count]=0;
	return count;
}
double Lexer::readDouble()
{
	skipWhiteSpace();
	int startpos=m_pos;
	try
	{
		if (m_text[m_pos]=='-') ++m_pos;
		if (m_text[m_pos]=='.') ++m_pos;
		while ((m_text[m_pos]>='0') && (m_text[m_pos]<='9')) ++m_pos;
		if (m_text[m_pos]=='.') 
		{
			++m_pos;
			while ((m_text[m_pos]>='0') && (m_text[m_pos]<='9')) ++m_pos;
		}
	}
	catch (LexException &) {};	//if we read beyond the current end

	char t=m_text[m_pos];
    m_text[m_pos]=0; // speed up atof    
	double value=atof(&m_text[startpos]);
	m_text[m_pos]=t; // speed up atof 
	return value;
}

int Lexer::readInteger()
{
	skipWhiteSpace();
	int startpos=m_pos;
	try
	{
		if (m_text[m_pos]=='-') ++m_pos;
		while ((m_text[m_pos]>='0') && (m_text[m_pos]<='9')) ++m_pos;
	}
	catch (LexException &) {}; // don´t read beyond end of block

	char t=m_text[m_pos];
    m_text[m_pos]=0; // speed up atof    
	int value=atoi(&m_text[startpos]);
	m_text[m_pos]=t; // speed up atof 
	return value;
}

int  Lexer::readLine(char *dest)
{
	skipWhiteSpace();
	int count=0;

	try
	{
		while (m_text[m_pos]!=0x0D && m_text[m_pos]!='\n')
		{
			try
			{
				if (m_text[m_pos]=='/')
				{
					if (m_text[m_pos+1]=='/')
					{
						break; // c++ style comment, end of line reached
					}
					else if (m_text[m_pos+1]=='*') // c-style comment, not thoroughly tested!
					{
						m_pos+=2;	
						LOOP:
							if (m_text[m_pos]=='*' && m_text[m_pos+1]=='/')
							{
								m_pos+=2;
								continue; // end of comment reached, continue with outer loop
							}
							else if (m_text[m_pos]!=0x0D && m_text[m_pos]!='\n')
							{
								std::cout<<"WARNING: Lexer::readLine() end of line inside c-comment"<<std::endl;
								break;	// stop outer loop, end of line found
							}
							++m_pos;
						goto LOOP;
					}
				}
			}
			catch (LexException &) {}; // probing for comments beyond end of block should not harm parsing the line

			dest[count++]=m_text[m_pos++];
		}
	}
	catch (LexException &) {}; // don´t read beyond end of block

	dest[count]=0;
	return count;
}

void Lexer::nextToken()
{
	skipUntilWhiteSpace();
	skipWhiteSpace();
}

// returns position of closing character
int Lexer::getEndOfNextBlock(char open, char close)
{
	pushPosition();

	try
	{
		skipBeyondNext(open);
		int num_pairs=1;
		while (num_pairs>0)
		{	
			skipWhiteSpace();
			if (matchAndSkip(open))
			{
				++num_pairs;
			}
			else if (matchAndSkip(close))
			{
				--num_pairs;
			}
			else if (matchAndSkip('\"'))
			{
				// don´t look for brackets inside strings
				while (!matchAndSkip('\"'))
				{
					++m_pos;
				}
			}
			else
			{
				++m_pos;
			}
		}
	}
	catch (LexException &e)
	{
		popPosition(); // get stack back to initial state
		throw e; //something went wrong
	}

	int temp=m_pos;
	popPosition();

	return temp;
}
void Lexer::skipNextBlock(char open, char close)
{
	int end=getEndOfNextBlock(open, close);
	m_pos=end+1;
}

void Lexer::enterBlock(char open, char close)
{
	skipUntil(open);
    int endofblock=getEndOfNextBlock(open,close);
	
	++m_pos;	// skip opening character

	blockstack.push(maxint(endofblock+1,m_pos.getMax())); // automatically jump over the block when exiting it

	m_pos.setMax(endofblock-1); // end of block is one position before closing character
}
void Lexer::exitBlock()
{
	m_pos=blockstack.top();
	blockstack.pop();
}

bool Lexer::tryFind(const char *text)
{
	blockstack.push(m_pos);
	bool rval=true;
	try 
	{
		skipUntil(text);
	}
	catch (LexException &)
	{
		rval=false;
		m_pos=blockstack.top(); // reset back 
	}
	blockstack.pop();
	return rval;
}
bool Lexer::tryFindAndSkip (const char* text)
{
	blockstack.push(m_pos);
	bool rval=true;
	try 
	{
		skipBeyondNext(text);
	}
	catch (LexException &)
	{
		rval=false;
		m_pos=blockstack.top(); // reset back 
	}
	blockstack.pop();
	return rval;
}
void Lexer::pushPosition()
{
	blockstack.push(m_pos);
}
void Lexer::popPosition()
{
	m_pos=blockstack.top();
	blockstack.pop();
}

