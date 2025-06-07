/*
(c) 2002-2009 by Mathias Heyer
*/

#include "misc/Lexer.h"
#include "defs.h"

#include <cassert>
#include <iostream>

using namespace std;

Lexer::LexInput::LexInput(const char* buffer, bool casesensitive)
: m_buffer(buffer)
, m_casesensitive(casesensitive)
{
    locale loc;
    for (int c = 0; c < 256; ++c)
        m_tolower[c] = std::tolower(c, loc);
};

void Lexer::LexInput::setBuffer(const char* buffer)
{
    m_buffer = buffer;
}

void Lexer::LexInput::setCaseSensitive(bool sensitive)
{
    m_casesensitive = sensitive;
}

inline char Lexer::LexInput::operator[](const int pos) const
{
    return m_casesensitive ? m_buffer[pos] : m_tolower[m_buffer[pos]];
}

void Lexer::LexInput::copy(char* dest, int startpos, int length) const
{
    for (int c = 0; c < length; ++c) {
        *dest++ = this->operator[](startpos + c);
    }
}

const char* Lexer::LexInput::getBuffer() const
{
    return m_buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

Lexer::Lexer(void)
{
    locale loc;
    for (int c = 0; c < 256; ++c) {
        m_isalpha[c]    = std::isalpha(c, loc);
        m_isalphanum[c] = std::isalnum(c, loc);
        m_isdigit[c]    = std::isdigit(c, loc);
        m_isspace[c]    = std::isspace(c, loc);
    }
}

Lexer::~Lexer(void)
{
}

void Lexer::setText(const std::string& text)
{
    setText(text.c_str(), text.size());
}

void Lexer::setText(const char* text, int textlength)
{
    // cleanup blockstack
    while (!m_blockstack.empty())
        m_blockstack.pop();

    if (textlength != -1) {
        m_text.assign(text, textlength);
    } else {
        m_text.assign(text);
    }

    m_pos = 0;
    m_pos.setMax(static_cast<int>(m_text.size()));

    m_input.setBuffer(m_text.c_str());
}
void Lexer::setCaseSensitive(bool sensitive)
{
    m_input.setCaseSensitive(sensitive);
}

bool Lexer::matchWhitespace()
{
    return m_isspace[m_input[m_pos]];
}

bool Lexer::matchWhitespaceAndSkip()
{
    bool rval = matchWhitespace();
    if (rval)
        ++m_pos;
    return rval;
}

void Lexer::skipWhiteSpace()
{
    while (true) {
        // FIXME: since m_input is an array of signed char, checking for >0x7F is somewhat useless
        while (matchWhitespace())
            ++m_pos;

        if (m_input[m_pos] == '/')  // possible start of comment
        {
            if (m_input[m_pos + 1] == '/')  // its a C++ comment!
            {
                m_pos += 2;
                while (m_input[m_pos++] != '\n')
                    ;      // skip to end of line
                continue;  // continue skipping whitespace
            } else {
                if (m_input[m_pos + 1] == '*')  // its a C-comment!
                {
                    m_pos += 2;
                    while ((m_input[m_pos] != '*') || (m_input[m_pos + 1] != '/'))  // find end of C-comment
                    {
                        ++m_pos;
                    }
                    m_pos += 2;
                    continue;  // continue skipping whitespace
                }
            }
        }
        break;
    }
}

void Lexer::nextLine()
{
    while (true) {
        while ((m_input[m_pos] != '\n') && (m_input[m_pos] != '/'))
            ++m_pos;
        if (m_input[m_pos] == '\n') {
            ++m_pos;
            return;                        // found end of line
        } else if (m_input[m_pos] == '/')  // may be some comment
        {
            if (m_input[m_pos + 1] == '/')  // C++ comment
            {
                m_pos += 2;
                while (m_input[m_pos++] != '\n')
                    ;
                break;  // found endof line
            } else {
                // FIXME: what about EOL in a c-comment ?
                if (m_input[m_pos + 1] == '*')  // C-comment
                {
                    m_pos += 2;
                    while ((m_input[m_pos] != '*') || (m_input[m_pos + 1] != '/')) {
                        ++m_pos;
                    }
                    m_pos += 2;
                    continue;  // continue skipping whitespace
                }
            }
            ++m_pos;  // backslash was not start of a comment
        }
    }
}

void Lexer::skipUntilWhiteSpace()
{
    while (!matchWhitespace())
        ++m_pos;
}

void Lexer::skipUntil(char c)
{
    skipWhiteSpace();
    while (m_input[m_pos] != c) {
        if (match('\"')) {
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
    return m_input[m_pos] == c;
}

bool Lexer::matchAndSkip(char c)
{
    if (m_input[m_pos] == c) {
        ++m_pos;
        return true;
    }
    return false;
}

bool Lexer::matchDigit()
{
    return m_isdigit[m_input[m_pos]];
}

bool Lexer::matchAlpha()
{
    return m_isalpha[m_input[m_pos]];
}

bool Lexer::matchAlphaNum()
{
    return m_isalphanum[m_input[m_pos]];
}

bool Lexer::matchIdentifier(const char* identifier)
{
    maxint temp = m_pos;
    bool   rval = matchAndSkipIdentifier(identifier);
    m_pos       = temp;
    return rval;
}

bool Lexer::matchAndSkipIdentifier(const char* identifier)
{
    maxint temp = m_pos;
    try {
        skipWhiteSpace();
        while ((*identifier != 0) && ((*identifier) == (m_input[m_pos]))) {
            ++identifier;
            ++m_pos;
        };
    } catch (LexException&) {
        m_pos = temp;
        return false;  // if matching would need reading beyond end of block
    }
    // comparism got through the whole identifier, now check if the parsed identifier is maybe just a prefix to another
    if (*identifier || matchAlphaNum() || match('_')) {
        // the identifier in the text is longer than 'identifier'
        m_pos = temp;
        return false;
    }

    return true;
}

bool Lexer::match(const char* text)
{
    maxint temp = m_pos;
    bool   rval = matchAndSkip(text);
    m_pos       = temp;
    return rval;
}

bool Lexer::matchAndSkip(const char* text)
{
    maxint temp = m_pos;
    try {
        while ((*text != 0) && ((*text) == (m_input[m_pos])))  //&&(*string2!=0) shouldn´t be needed
        {
            ++text;
            ++m_pos;
        };
    } catch (LexException&) {
        m_pos = temp;
        return false;  // if matching would need reading beyond end of block
    }

    if (!(*text))
        return true;

    m_pos = temp;
    return false;
}

void Lexer::skipUntil(const char* text)
{
    skipUntil(text[0]);
    while (!match(text)) {
        if (matchAndSkip('\"')) {
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
    while (!matchAndSkip(text)) {
        if (matchAndSkip('\"')) {
            while (!match('\"'))
                ++m_pos;
        }
        ++m_pos;
        skipUntil(text[0]);
    }
}

int Lexer::readQuotedString(char* dest)
{
    skipWhiteSpace();
    int count = 0;
    *dest     = 0;
    if (!match('\"'))
        return -1;  // FIXME: better throw exception?
    do {
        skipBeyondNext('\"');
        while (m_input[m_pos] != '\"') {
            dest[count++] = m_input[m_pos++];
        }

        ++m_pos;  // skip trailing '"'
        try {
            skipWhiteSpace();
        } catch (LexException&) {
            break;
        };

    } while (match('\\'));  // support concatenated multi-line strings

    dest[count] = 0;  //

    return count;
}
int Lexer::readUnquotedString(char* dest)
{
    skipWhiteSpace();
    int count = 0;
    try {
        while (!matchWhitespace()) {
            dest[count++] = m_input[m_pos];
            ++m_pos;
        }
    } catch (LexException&) {
    };  // don´t read beyond end of block

    dest[count] = 0;
    return count;
}

int Lexer::readString(char* dest)
{
    skipWhiteSpace();
    if (match('\"')) {
        return readQuotedString(dest);
    } else {
        return readUnquotedString(dest);
    }
}

int Lexer::readStringUntil(char c, char* dest)
{
    skipWhiteSpace();
    int count = 0;
    while (m_input[m_pos] != c) {
        dest[count++] = m_input[m_pos++];
    }
    dest[count] = 0;
    return count;
}

int Lexer::readStringUntilAndSkip(char c, char* dest)
{
    int count = readStringUntil(c, dest);
    m_pos++;
    return count;
}

int Lexer::readIdentifier(char* dest)
{
    skipWhiteSpace();
    int count = 0;

    try {
        if (matchAlpha() || match('_'))  // identifier starts with _ or character
        {
            dest[count++] = m_input[m_pos];
            ++m_pos;
            while (matchAlphaNum() || match('_'))  // inside the identifier numbers are allowed
            {
                dest[count++] = m_input[m_pos];
                ++m_pos;
            }
        }
    } catch (LexException&) {
    };  // don´t read beyond end of block

    dest[count] = 0;
    return count;
}

int Lexer::readFilename(char* dest)
{
    skipWhiteSpace();
    if (match('\"'))
        return readQuotedString(dest);

    int count = 0;

    try {
        while (matchAlphaNum() || match('_') || match('\\') || match('/') || match('.')) {
            dest[count++] = m_input[m_pos];
            ++m_pos;
        }
    } catch (LexException&) {
    };  // don´t read beyond end of block

    BREAKIF(count > 256);

    dest[count] = 0;
    return count;
}

int Lexer::readNumber(char* dest)
{
    skipWhiteSpace();
    int startpos = m_pos;
    try {
        matchAndSkip('-');  // sign
        matchAndSkip('+');  // sign
        while (matchDigit())
            ++m_pos;            // read the numbers before point
        if (matchAndSkip('.'))  //
        {
            while (matchDigit())
                ++m_pos;  // numbers after point
        }
        if (matchAndSkip('e') || matchAndSkip('E'))  // exponent
        {
            matchAndSkip('-');  // sign of exponent
            matchAndSkip('+');  //
            while (matchDigit())
                ++m_pos;  // digits of exponent
        } else if (matchAndSkip('f')) {
            // atof does not parse numbers like " 1.0f"
        } else if (matchAndSkip('#')) {
            // special numbers like #INF or #QNAN
            while (matchAlpha())
                ++m_pos;
        }
    } catch (LexException&) {
    };  // if we read beyond the current end

    int count = m_pos - startpos;
    m_input.copy(dest, startpos, count);
    dest[count] = 0;

    return count;
}

double Lexer::readDouble()
{
    char number[128];
    int  num_chars = readNumber(number);
    assert(num_chars < 128);
    if (!num_chars)
        throw CException("Lexer::readDouble() number has no characters");

    double value = atof(number);
    return value;
}

int Lexer::readInteger()
{
    skipWhiteSpace();
    int startpos = m_pos;
    try {
        matchAndSkip('-');  // sign
        matchAndSkip('+');  // sign
        while (matchDigit())
            ++m_pos;
    } catch (LexException&) {
    };  // don´t read beyond end of block

    int value = atoi(m_input.getBuffer() + startpos);
    return value;
}

int Lexer::readLine(char* dest)
{
    skipWhiteSpace();
    int count = 0;

    try {
        while (m_input[m_pos] != 0x0D && m_input[m_pos] != '\n') {
            try {
                if (m_input[m_pos] == '/') {
                    if (m_input[m_pos + 1] == '/') {
                        break;                             // c++ style comment, end of line reached
                    } else if (m_input[m_pos + 1] == '*')  // c-style comment, not thoroughly tested!
                    {
                        m_pos += 2;
                    LOOP:
                        if (m_input[m_pos] == '*' && m_input[m_pos + 1] == '/') {
                            m_pos += 2;
                            continue;  // end of comment reached, continue with outer loop
                        } else if (m_input[m_pos] != 0x0D && m_input[m_pos] != '\n') {
                            std::cout << "WARNING: Lexer::readLine() end of line inside c-comment" << std::endl;
                            break;  // stop outer loop, end of line found
                        }
                        ++m_pos;
                        goto LOOP;
                    }
                }
            } catch (LexException&) {
            };  // probing for comments beyond end of block should not harm parsing the line

            dest[count++] = m_input[m_pos++];
        }
    } catch (LexException&) {
    };  // don´t read beyond end of block

    dest[count] = 0;
    return count;
}

char Lexer::readChar()
{
    return m_input[m_pos++];
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

    try {
        skipUntil(open);
        int num_pairs = 1;
        while (num_pairs > 0) {
            ++m_pos;
            if (match(open)) {
                ++num_pairs;
            } else if (match(close)) {
                --num_pairs;
            } else if (matchAndSkip('\"')) {
                // don´t look for brackets inside strings
                while (!match('\"')) {
                    ++m_pos;
                }
            }
        }
    } catch (LexException& e) {
        popPosition();  // get stack back to initial state
        throw e;        // something went wrong
    }

    int temp = m_pos;
    popPosition();

    return temp;
}
void Lexer::skipNextBlock(char open, char close)
{
    int end = getEndOfNextBlock(open, close);
    m_pos   = end + 1;
}

void Lexer::enterBlock(char open, char close)
{
    skipUntil(open);
    int endofblock = getEndOfNextBlock(open, close);

    ++m_pos;  // skip opening character

    m_blockstack.push(maxint(endofblock, m_pos.getMax()));

    m_pos.setMax(endofblock - 1);  // end of block is one position before closing character
}
void Lexer::exitBlock()
{
    assert(!m_blockstack.empty());
    m_pos = m_blockstack.top();
    m_blockstack.pop();
    ++m_pos;
}

bool Lexer::tryFind(const char* text)
{
    m_blockstack.push(m_pos);
    bool rval = true;
    try {
        skipUntil(text);
    } catch (LexException&) {
        rval  = false;
        m_pos = m_blockstack.top();  // reset back
    }
    m_blockstack.pop();
    return rval;
}
bool Lexer::tryFindAndSkip(const char* text)
{
    m_blockstack.push(m_pos);
    bool rval = true;
    try {
        skipBeyondNext(text);
    } catch (LexException&) {
        rval  = false;
        m_pos = m_blockstack.top();  // reset back
    }
    m_blockstack.pop();
    return rval;
}
void Lexer::pushPosition()
{
    m_blockstack.push(m_pos);
}
void Lexer::popPosition()
{
    m_pos = m_blockstack.top();
    m_blockstack.pop();
}
