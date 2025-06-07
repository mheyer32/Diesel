/* $ANTLR 2.7.0: "Doom3.g" -> "DoomLexer.cpp"$ */

// gets inserted after the antlr generated includes in the cpp file

#include "DoomLexer.hpp"
#include "antlr/CharBuffer.hpp"
#include "antlr/CharStreamException.hpp"
#include "antlr/CharStreamIOException.hpp"
#include "antlr/NoViableAltForCharException.hpp"
#include "antlr/TokenStreamException.hpp"
#include "antlr/TokenStreamIOException.hpp"
#include "antlr/TokenStreamRecognitionException.hpp"

// gets inserted after generated namespace specifications in the header
// file. But outside the generated class.
#include <iostream>
using namespace std;

DoomLexer::DoomLexer(ANTLR_USE_NAMESPACE(std) istream& in)
: ANTLR_USE_NAMESPACE(antlr) CharScanner(new ANTLR_USE_NAMESPACE(antlr) CharBuffer(in))
{
    setCaseSensitive(true);
    initLiterals();
}

DoomLexer::DoomLexer(ANTLR_USE_NAMESPACE(antlr) InputBuffer& ib)
: ANTLR_USE_NAMESPACE(antlr) CharScanner(ib)
{
    setCaseSensitive(true);
    initLiterals();
}

DoomLexer::DoomLexer(const ANTLR_USE_NAMESPACE(antlr) LexerSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr) CharScanner(state)
{
    setCaseSensitive(true);
    initLiterals();
}

void DoomLexer::initLiterals()
{
    literals["surfaceparm"]            = 77;
    literals["color"]                  = 125;
    literals["skybox"]                 = 144;
    literals["snaptable"]              = 50;
    literals["slick"]                  = 84;
    literals["linear"]                 = 155;
    literals["aa"]                     = 74;
    literals["guisurf"]                = 56;
    literals["maskcolor"]              = 151;
    literals["cubemap"]                = 106;
    literals["green"]                  = 132;
    literals["#define"]                = 52;
    literals["rgba"]                   = 130;
    literals["gl_dst_color"]           = 119;
    literals["nodes"]                  = 46;
    literals["gl_src_color"]           = 117;
    literals["colormap"]               = 76;
    literals["alpha"]                  = 134;
    literals["inversevertexcolor"]     = 128;
    literals["remoterendermap"]        = 109;
    literals["expand"]                 = 94;
    literals["diffusemap"]             = 99;
    literals["noselfshadow"]           = 61;
    literals["maskalpha"]              = 152;
    literals["shadowmodel"]            = 47;
    literals["nopicmip"]               = 154;
    literals["nodrop"]                 = 79;
    literals["nonsolid"]               = 81;
    literals["clamptable"]             = 49;
    literals["rotate"]                 = 136;
    literals["scroll"]                 = 138;
    literals["maskdepth"]              = 153;
    literals["gl_dst_alpha"]           = 123;
    literals["qer_trans"]              = 54;
    literals["heightmap"]              = 101;
    literals["tube"]                   = 92;
    literals["gl_src_alpha"]           = 121;
    literals["deform"]                 = 89;
    literals["add"]                    = 113;
    literals["trace"]                  = 75;
    literals["addnormals"]             = 102;
    literals["patchDef3"]              = 43;
    literals["eyeball"]                = 91;
    literals["blood"]                  = 88;
    literals["renderbump"]             = 72;
    literals["qer_nocarve"]            = 55;
    literals["forceshadows"]           = 62;
    literals["translucent"]            = 57;
    literals["patchDef2"]              = 42;
    literals["model"]                  = 44;
    literals["monsterclip"]            = 82;
    literals["noisemap"]               = 111;
    literals["brushDef3"]              = 41;
    literals["mirrorrendermap"]        = 110;
    literals["centerscale"]            = 140;
    literals["map"]                    = 105;
    literals["flarepic"]               = 70;
    literals["red"]                    = 131;
    literals["cameracubemap"]          = 108;
    literals["gl_zero"]                = 116;
    literals["nofragment"]             = 63;
    literals["bumpmap"]                = 98;
    literals["highquality"]            = 157;
    literals["ladder"]                 = 86;
    literals["twosided"]               = 60;
    literals["if"]                     = 104;
    literals["vertexcolor"]            = 127;
    literals["lightfalloffimage"]      = 69;
    literals["trans"]                  = 87;
    literals["polygonoffet"]           = 58;
    literals["gl_one_minus_dst_color"] = 120;
    literals["normal"]                 = 142;
    literals["nearest"]                = 156;
    literals["blendlight"]             = 67;
    literals["qer_editorimage"]        = 53;
    literals["rgb"]                    = 129;
    literals["clamp"]                  = 149;
    literals["specularmap"]            = 100;
    literals["makeintensity"]          = 103;
    literals["gl_one_minus_src_color"] = 118;
    literals["blue"]                   = 133;
    literals["blend"]                  = 112;
    literals["scale"]                  = 139;
    literals["shear"]                  = 137;
    literals["size"]                   = 73;
    literals["texgen"]                 = 141;
    literals["gl_one"]                 = 115;
    literals["gl_one_minus_dst_alpha"] = 124;
    literals["false"]                  = 97;
    literals["nodraw"]                 = 78;
    literals["alphazeroclamp"]         = 148;
    literals["foglight"]               = 66;
    literals["move"]                   = 95;
    literals["table"]                  = 48;
    literals["flare"]                  = 93;
    literals["sprite"]                 = 90;
    literals["playerclip"]             = 83;
    literals["description"]            = 71;
    literals["zeroclamp"]              = 147;
    literals["interareaportals"]       = 45;
    literals["noshadows"]              = 59;
    literals["gl_one_minus_src_alpha"] = 122;
    literals["noimpact"]               = 80;
    literals["soundtable"]             = 51;
    literals["translate"]              = 135;
    literals["ambientlight"]           = 68;
    literals["reflect"]                = 143;
    literals["sort"]                   = 65;
    literals["filter"]                 = 114;
    literals["discrete"]               = 85;
    literals["ignorealphatest"]        = 146;
    literals["shadowdraw"]             = 150;
    literals["colored"]                = 126;
    literals["true"]                   = 96;
    literals["videomap"]               = 107;
    literals["alphatest"]              = 145;
    literals["forceopaque"]            = 64;
}
bool DoomLexer::getCaseSensitiveLiterals() const
{
    return false;
}

ANTLR_USE_NAMESPACE(antlr) RefToken DoomLexer::nextToken()
{
    ANTLR_USE_NAMESPACE(antlr) RefToken theRetToken;
    for (;;) {
        ANTLR_USE_NAMESPACE(antlr) RefToken theRetToken;
        int                                 _ttype = ANTLR_USE_NAMESPACE(antlr) Token::INVALID_TYPE;
        resetText();
        try {      // for char stream error handling
            try {  // for lexical error handling
                switch (LA(1)) {
                case static_cast<unsigned char>(','): {
                    mCOMMA(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('('): {
                    mLPAREN(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>(')'): {
                    mRPAREN(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('['): {
                    mLBRACKET(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>(']'): {
                    mRBRACKET(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('{'): {
                    mLCURLY(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('}'): {
                    mRCURLY(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('='): {
                    mEQUAL(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('+'): {
                    mPLUS(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('-'): {
                    mMINUS(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('*'): {
                    mSTAR(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('%'): {
                    mMOD(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('&'): {
                    mLAND(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('|'): {
                    mLOR(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('\t'):
                case static_cast<unsigned char>('\n'):
                case static_cast<unsigned char>('\14'):
                case static_cast<unsigned char>('\r'):
                case static_cast<unsigned char>(' '): {
                    mWHITESPACE(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('"'): {
                    mSTRING(true);
                    theRetToken = _returnToken;
                    break;
                }
                case static_cast<unsigned char>('.'):
                case static_cast<unsigned char>('0'):
                case static_cast<unsigned char>('1'):
                case static_cast<unsigned char>('2'):
                case static_cast<unsigned char>('3'):
                case static_cast<unsigned char>('4'):
                case static_cast<unsigned char>('5'):
                case static_cast<unsigned char>('6'):
                case static_cast<unsigned char>('7'):
                case static_cast<unsigned char>('8'):
                case static_cast<unsigned char>('9'): {
                    mNUMBER(true);
                    theRetToken = _returnToken;
                    break;
                }
                default:
                    if ((LA(1) == static_cast<unsigned char>('!')) && (LA(2) == static_cast<unsigned char>('='))) {
                        mNOTEQUAL(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('<')) &&
                               (LA(2) == static_cast<unsigned char>('='))) {
                        mLTE(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('>')) &&
                               (LA(2) == static_cast<unsigned char>('='))) {
                        mGTE(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('/')) &&
                               (LA(2) == static_cast<unsigned char>('*'))) {
                        mCOMMENT(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('/')) &&
                               (LA(2) == static_cast<unsigned char>('/'))) {
                        mCPPCOMMENT(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('<')) && (true)) {
                        mLESS(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('>')) && (true)) {
                        mGREATER(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('/')) && (true)) {
                        mDIV(true);
                        theRetToken = _returnToken;
                    } else if ((LA(1) == static_cast<unsigned char>('!')) && (true)) {
                        mLNOT(true);
                        theRetToken = _returnToken;
                    } else {
                        mFILE_ID_NUMBER(true);
                        theRetToken = _returnToken;
                    }
                }
                if (!_returnToken)
                    goto tryAgain;  // found SKIP token
                _ttype = _returnToken->getType();
                _returnToken->setType(_ttype);
                return _returnToken;
            } catch (ANTLR_USE_NAMESPACE(antlr) RecognitionException& e) {
                throw ANTLR_USE_NAMESPACE(antlr) TokenStreamRecognitionException(e);
            }
        } catch (ANTLR_USE_NAMESPACE(antlr) CharStreamIOException& csie) {
            throw ANTLR_USE_NAMESPACE(antlr) TokenStreamIOException(csie.io);
        } catch (ANTLR_USE_NAMESPACE(antlr) CharStreamException& cse) {
            throw ANTLR_USE_NAMESPACE(antlr) TokenStreamException(cse.getMessage());
        }
    tryAgain:;
    }
}

void DoomLexer::mCOMMA(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = COMMA;
    int _saveIndex;

    match(static_cast<unsigned char>(','));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLPAREN(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LPAREN;
    int _saveIndex;

    match(static_cast<unsigned char>('('));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mRPAREN(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = RPAREN;
    int _saveIndex;

    match(static_cast<unsigned char>(')'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLBRACKET(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LBRACKET;
    int _saveIndex;

    match(static_cast<unsigned char>('['));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mRBRACKET(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = RBRACKET;
    int _saveIndex;

    match(static_cast<unsigned char>(']'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLCURLY(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LCURLY;
    int _saveIndex;

    match(static_cast<unsigned char>('{'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mRCURLY(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = RCURLY;
    int _saveIndex;

    match(static_cast<unsigned char>('}'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mEQUAL(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = EQUAL;
    int _saveIndex;

    match("==");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mNOTEQUAL(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = NOTEQUAL;
    int _saveIndex;

    match("!=");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLTE(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LTE;
    int _saveIndex;

    match("<=");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLESS(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LESS;
    int _saveIndex;

    match("<");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mGTE(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = GTE;
    int _saveIndex;

    match(">=");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mGREATER(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = GREATER;
    int _saveIndex;

    match(">");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mDIV(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = DIV;
    int _saveIndex;

    match(static_cast<unsigned char>('/'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mPLUS(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = PLUS;
    int _saveIndex;

    match(static_cast<unsigned char>('+'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mMINUS(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = MINUS;
    int _saveIndex;

    match(static_cast<unsigned char>('-'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mSTAR(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = STAR;
    int _saveIndex;

    match(static_cast<unsigned char>('*'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mMOD(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = MOD;
    int _saveIndex;

    match(static_cast<unsigned char>('%'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLAND(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LAND;
    int _saveIndex;

    match("&&");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLNOT(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LNOT;
    int _saveIndex;

    match(static_cast<unsigned char>('!'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mLOR(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = LOR;
    int _saveIndex;

    match("||");
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mWHITESPACE(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = WHITESPACE;
    int _saveIndex;

    {
        switch (LA(1)) {
        case static_cast<unsigned char>('\t'): {
            match(static_cast<unsigned char>('\t'));
            break;
        }
        case static_cast<unsigned char>('\14'): {
            match(static_cast<unsigned char>('\14'));
            break;
        }
        case static_cast<unsigned char>(' '): {
            match(static_cast<unsigned char>(' '));
            break;
        }
        case static_cast<unsigned char>('\n'): {
            match(static_cast<unsigned char>('\n'));
            break;
        }
        case static_cast<unsigned char>('\r'): {
            match(static_cast<unsigned char>('\r'));
            break;
        }
        default: {
            throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
        }
        }
    }
    if (inputState->guessing == 0) {
        _ttype = antlr::Token::SKIP;
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mSTRING(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = STRING;
    int _saveIndex;

    _saveIndex = text.length();
    match(static_cast<unsigned char>('"'));
    text.erase(_saveIndex);
    {
        for (;;) {
            if ((_tokenSet_0.member(LA(1)))) {
                {
                    match(_tokenSet_0);
                }
            } else {
                goto _loop27;
            }
        }
    _loop27:;
    }
    _saveIndex = text.length();
    match(static_cast<unsigned char>('"'));
    text.erase(_saveIndex);
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mCOMMENT(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = COMMENT;
    int _saveIndex;

    match("/*");
    {
        for (;;) {
            if (((LA(1) == static_cast<unsigned char>('*')) &&
                 ((LA(2) >= static_cast<unsigned char>('\0') && LA(2) <= static_cast<unsigned char>('\377'))) &&
                 ((LA(3) >= static_cast<unsigned char>('\0') && LA(3) <= static_cast<unsigned char>('\377')))) &&
                (LA(2) != '/')) {
                match(static_cast<unsigned char>('*'));
            } else {
                bool synPredMatched31 = false;
                if (((LA(1) == static_cast<unsigned char>('\r')) && (LA(2) == static_cast<unsigned char>('\n')) &&
                     ((LA(3) >= static_cast<unsigned char>('\0') && LA(3) <= static_cast<unsigned char>('\377'))))) {
                    int _m31         = mark();
                    synPredMatched31 = true;
                    inputState->guessing++;
                    try {
                        {
                            match("\r\n");
                        }
                    } catch (ANTLR_USE_NAMESPACE(antlr) RecognitionException& pe) {
                        synPredMatched31 = false;
                    }
                    rewind(_m31);
                    inputState->guessing--;
                }
                if (synPredMatched31) {
                    match("\r\n");
                } else if ((LA(1) == static_cast<unsigned char>('\n') || LA(1) == static_cast<unsigned char>('\r')) &&
                           ((LA(2) >= static_cast<unsigned char>('\0') &&
                             LA(2) <= static_cast<unsigned char>('\377'))) &&
                           ((LA(3) >= static_cast<unsigned char>('\0') &&
                             LA(3) <= static_cast<unsigned char>('\377')))) {
                    {
                        switch (LA(1)) {
                        case static_cast<unsigned char>('\r'): {
                            match(static_cast<unsigned char>('\r'));
                            break;
                        }
                        case static_cast<unsigned char>('\n'): {
                            match(static_cast<unsigned char>('\n'));
                            break;
                        }
                        default: {
                            throw ANTLR_USE_NAMESPACE(antlr)
                                NoViableAltForCharException(LA(1), getFilename(), getLine());
                        }
                        }
                    }
                } else if ((_tokenSet_1.member(LA(1)))) {
                    {
                        match(_tokenSet_1);
                    }
                } else {
                    goto _loop34;
                }
            }
        }
    _loop34:;
    }
    match("*/");
    if (inputState->guessing == 0) {
        /*printf("COMMENT %s\n\n",getText().c_str());*/ _ttype = antlr::Token::SKIP;
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mCPPCOMMENT(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = CPPCOMMENT;
    int _saveIndex;

    match("//");
    {
        for (;;) {
            if ((_tokenSet_2.member(LA(1)))) {
                {
                    match(_tokenSet_2);
                }
            } else {
                goto _loop38;
            }
        }
    _loop38:;
    }
    if (inputState->guessing == 0) {
        /*printf("COMMENT %s\n\n",getText().c_str());*/ _ttype = antlr::Token::SKIP;
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mDIGIT(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = DIGIT;
    int _saveIndex;

    matchRange(static_cast<unsigned char>('0'), static_cast<unsigned char>('9'));
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mALPHA(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = ALPHA;
    int _saveIndex;

    switch (LA(1)) {
    case static_cast<unsigned char>('a'):
    case static_cast<unsigned char>('b'):
    case static_cast<unsigned char>('c'):
    case static_cast<unsigned char>('d'):
    case static_cast<unsigned char>('e'):
    case static_cast<unsigned char>('f'):
    case static_cast<unsigned char>('g'):
    case static_cast<unsigned char>('h'):
    case static_cast<unsigned char>('i'):
    case static_cast<unsigned char>('j'):
    case static_cast<unsigned char>('k'):
    case static_cast<unsigned char>('l'):
    case static_cast<unsigned char>('m'):
    case static_cast<unsigned char>('n'):
    case static_cast<unsigned char>('o'):
    case static_cast<unsigned char>('p'):
    case static_cast<unsigned char>('q'):
    case static_cast<unsigned char>('r'):
    case static_cast<unsigned char>('s'):
    case static_cast<unsigned char>('t'):
    case static_cast<unsigned char>('u'):
    case static_cast<unsigned char>('v'):
    case static_cast<unsigned char>('w'):
    case static_cast<unsigned char>('x'):
    case static_cast<unsigned char>('y'):
    case static_cast<unsigned char>('z'): {
        {
            matchRange(static_cast<unsigned char>('a'), static_cast<unsigned char>('z'));
        }
        break;
    }
    case static_cast<unsigned char>('A'):
    case static_cast<unsigned char>('B'):
    case static_cast<unsigned char>('C'):
    case static_cast<unsigned char>('D'):
    case static_cast<unsigned char>('E'):
    case static_cast<unsigned char>('F'):
    case static_cast<unsigned char>('G'):
    case static_cast<unsigned char>('H'):
    case static_cast<unsigned char>('I'):
    case static_cast<unsigned char>('J'):
    case static_cast<unsigned char>('K'):
    case static_cast<unsigned char>('L'):
    case static_cast<unsigned char>('M'):
    case static_cast<unsigned char>('N'):
    case static_cast<unsigned char>('O'):
    case static_cast<unsigned char>('P'):
    case static_cast<unsigned char>('Q'):
    case static_cast<unsigned char>('R'):
    case static_cast<unsigned char>('S'):
    case static_cast<unsigned char>('T'):
    case static_cast<unsigned char>('U'):
    case static_cast<unsigned char>('V'):
    case static_cast<unsigned char>('W'):
    case static_cast<unsigned char>('X'):
    case static_cast<unsigned char>('Y'):
    case static_cast<unsigned char>('Z'): {
        {
            matchRange(static_cast<unsigned char>('A'), static_cast<unsigned char>('Z'));
        }
        break;
    }
    default: {
        throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
    }
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mALPHANUM(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = ALPHANUM;
    int _saveIndex;

    switch (LA(1)) {
    case static_cast<unsigned char>('A'):
    case static_cast<unsigned char>('B'):
    case static_cast<unsigned char>('C'):
    case static_cast<unsigned char>('D'):
    case static_cast<unsigned char>('E'):
    case static_cast<unsigned char>('F'):
    case static_cast<unsigned char>('G'):
    case static_cast<unsigned char>('H'):
    case static_cast<unsigned char>('I'):
    case static_cast<unsigned char>('J'):
    case static_cast<unsigned char>('K'):
    case static_cast<unsigned char>('L'):
    case static_cast<unsigned char>('M'):
    case static_cast<unsigned char>('N'):
    case static_cast<unsigned char>('O'):
    case static_cast<unsigned char>('P'):
    case static_cast<unsigned char>('Q'):
    case static_cast<unsigned char>('R'):
    case static_cast<unsigned char>('S'):
    case static_cast<unsigned char>('T'):
    case static_cast<unsigned char>('U'):
    case static_cast<unsigned char>('V'):
    case static_cast<unsigned char>('W'):
    case static_cast<unsigned char>('X'):
    case static_cast<unsigned char>('Y'):
    case static_cast<unsigned char>('Z'):
    case static_cast<unsigned char>('a'):
    case static_cast<unsigned char>('b'):
    case static_cast<unsigned char>('c'):
    case static_cast<unsigned char>('d'):
    case static_cast<unsigned char>('e'):
    case static_cast<unsigned char>('f'):
    case static_cast<unsigned char>('g'):
    case static_cast<unsigned char>('h'):
    case static_cast<unsigned char>('i'):
    case static_cast<unsigned char>('j'):
    case static_cast<unsigned char>('k'):
    case static_cast<unsigned char>('l'):
    case static_cast<unsigned char>('m'):
    case static_cast<unsigned char>('n'):
    case static_cast<unsigned char>('o'):
    case static_cast<unsigned char>('p'):
    case static_cast<unsigned char>('q'):
    case static_cast<unsigned char>('r'):
    case static_cast<unsigned char>('s'):
    case static_cast<unsigned char>('t'):
    case static_cast<unsigned char>('u'):
    case static_cast<unsigned char>('v'):
    case static_cast<unsigned char>('w'):
    case static_cast<unsigned char>('x'):
    case static_cast<unsigned char>('y'):
    case static_cast<unsigned char>('z'): {
        {
            mALPHA(false);
        }
        break;
    }
    case static_cast<unsigned char>('0'):
    case static_cast<unsigned char>('1'):
    case static_cast<unsigned char>('2'):
    case static_cast<unsigned char>('3'):
    case static_cast<unsigned char>('4'):
    case static_cast<unsigned char>('5'):
    case static_cast<unsigned char>('6'):
    case static_cast<unsigned char>('7'):
    case static_cast<unsigned char>('8'):
    case static_cast<unsigned char>('9'): {
        {
            mDIGIT(false);
        }
        break;
    }
    default: {
        throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
    }
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mFLOAT(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = FLOAT;
    int _saveIndex;

    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mINT(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = INT;
    int _saveIndex;

    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mNUMBER(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = NUMBER;
    int _saveIndex;

    bool synPredMatched53 = false;
    if (((_tokenSet_3.member(LA(1))) && (_tokenSet_3.member(LA(2))) && (true))) {
        int _m53         = mark();
        synPredMatched53 = true;
        inputState->guessing++;
        try {
            {
                {
                    for (;;) {
                        if (((LA(1) >= static_cast<unsigned char>('0') && LA(1) <= static_cast<unsigned char>('9')))) {
                            mDIGIT(false);
                        } else {
                            goto _loop51;
                        }
                    }
                _loop51:;
                }
                {
                    match(static_cast<unsigned char>('.'));
                }
            }
        } catch (ANTLR_USE_NAMESPACE(antlr) RecognitionException& pe) {
            synPredMatched53 = false;
        }
        rewind(_m53);
        inputState->guessing--;
    }
    if (synPredMatched53) {
        {
            for (;;) {
                if (((LA(1) >= static_cast<unsigned char>('0') && LA(1) <= static_cast<unsigned char>('9')))) {
                    mDIGIT(false);
                } else {
                    goto _loop55;
                }
            }
        _loop55:;
        }
        {
            match(static_cast<unsigned char>('.'));
        }
        {
            int _cnt58 = 0;
            for (;;) {
                if (((LA(1) >= static_cast<unsigned char>('0') && LA(1) <= static_cast<unsigned char>('9')))) {
                    mDIGIT(false);
                } else {
                    if (_cnt58 >= 1) {
                        goto _loop58;
                    } else {
                        throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
                    }
                }

                _cnt58++;
            }
        _loop58:;
        }
        if (inputState->guessing == 0) {
            _ttype = FLOAT; /*printf("FLOAT %s\n\n",getText().c_str());*/
        }
    } else if (((LA(1) >= static_cast<unsigned char>('0') && LA(1) <= static_cast<unsigned char>('9'))) && (true) &&
               (true)) {
        {
            int _cnt60 = 0;
            for (;;) {
                if (((LA(1) >= static_cast<unsigned char>('0') && LA(1) <= static_cast<unsigned char>('9')))) {
                    mDIGIT(false);
                } else {
                    if (_cnt60 >= 1) {
                        goto _loop60;
                    } else {
                        throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
                    }
                }

                _cnt60++;
            }
        _loop60:;
        }
        if (inputState->guessing == 0) {
            _ttype = INT; /*printf("INT %s\n\n",getText().c_str());*/
        }
    } else {
        throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
    }

    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mFNAMECHAR(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = FNAMECHAR;
    int _saveIndex;

    {
        switch (LA(1)) {
        case static_cast<unsigned char>('0'):
        case static_cast<unsigned char>('1'):
        case static_cast<unsigned char>('2'):
        case static_cast<unsigned char>('3'):
        case static_cast<unsigned char>('4'):
        case static_cast<unsigned char>('5'):
        case static_cast<unsigned char>('6'):
        case static_cast<unsigned char>('7'):
        case static_cast<unsigned char>('8'):
        case static_cast<unsigned char>('9'):
        case static_cast<unsigned char>('A'):
        case static_cast<unsigned char>('B'):
        case static_cast<unsigned char>('C'):
        case static_cast<unsigned char>('D'):
        case static_cast<unsigned char>('E'):
        case static_cast<unsigned char>('F'):
        case static_cast<unsigned char>('G'):
        case static_cast<unsigned char>('H'):
        case static_cast<unsigned char>('I'):
        case static_cast<unsigned char>('J'):
        case static_cast<unsigned char>('K'):
        case static_cast<unsigned char>('L'):
        case static_cast<unsigned char>('M'):
        case static_cast<unsigned char>('N'):
        case static_cast<unsigned char>('O'):
        case static_cast<unsigned char>('P'):
        case static_cast<unsigned char>('Q'):
        case static_cast<unsigned char>('R'):
        case static_cast<unsigned char>('S'):
        case static_cast<unsigned char>('T'):
        case static_cast<unsigned char>('U'):
        case static_cast<unsigned char>('V'):
        case static_cast<unsigned char>('W'):
        case static_cast<unsigned char>('X'):
        case static_cast<unsigned char>('Y'):
        case static_cast<unsigned char>('Z'):
        case static_cast<unsigned char>('a'):
        case static_cast<unsigned char>('b'):
        case static_cast<unsigned char>('c'):
        case static_cast<unsigned char>('d'):
        case static_cast<unsigned char>('e'):
        case static_cast<unsigned char>('f'):
        case static_cast<unsigned char>('g'):
        case static_cast<unsigned char>('h'):
        case static_cast<unsigned char>('i'):
        case static_cast<unsigned char>('j'):
        case static_cast<unsigned char>('k'):
        case static_cast<unsigned char>('l'):
        case static_cast<unsigned char>('m'):
        case static_cast<unsigned char>('n'):
        case static_cast<unsigned char>('o'):
        case static_cast<unsigned char>('p'):
        case static_cast<unsigned char>('q'):
        case static_cast<unsigned char>('r'):
        case static_cast<unsigned char>('s'):
        case static_cast<unsigned char>('t'):
        case static_cast<unsigned char>('u'):
        case static_cast<unsigned char>('v'):
        case static_cast<unsigned char>('w'):
        case static_cast<unsigned char>('x'):
        case static_cast<unsigned char>('y'):
        case static_cast<unsigned char>('z'): {
            mALPHANUM(false);
            break;
        }
        case static_cast<unsigned char>('_'): {
            match(static_cast<unsigned char>('_'));
            break;
        }
        case static_cast<unsigned char>('.'): {
            match(static_cast<unsigned char>('.'));
            break;
        }
        default: {
            throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
        }
        }
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mFCHAR(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = FCHAR;
    int _saveIndex;

    {
        match(static_cast<unsigned char>('.'));
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mDELIM(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = DELIM;
    int _saveIndex;

    switch (LA(1)) {
    case static_cast<unsigned char>('/'): {
        match(static_cast<unsigned char>('/'));
        break;
    }
    case static_cast<unsigned char>('\\'): {
        match(static_cast<unsigned char>('\\'));
        break;
    }
    default: {
        throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
    }
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mFILENAME(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = FILENAME;
    int _saveIndex;

    {
        switch (LA(1)) {
        case static_cast<unsigned char>('A'):
        case static_cast<unsigned char>('B'):
        case static_cast<unsigned char>('C'):
        case static_cast<unsigned char>('D'):
        case static_cast<unsigned char>('E'):
        case static_cast<unsigned char>('F'):
        case static_cast<unsigned char>('G'):
        case static_cast<unsigned char>('H'):
        case static_cast<unsigned char>('I'):
        case static_cast<unsigned char>('J'):
        case static_cast<unsigned char>('K'):
        case static_cast<unsigned char>('L'):
        case static_cast<unsigned char>('M'):
        case static_cast<unsigned char>('N'):
        case static_cast<unsigned char>('O'):
        case static_cast<unsigned char>('P'):
        case static_cast<unsigned char>('Q'):
        case static_cast<unsigned char>('R'):
        case static_cast<unsigned char>('S'):
        case static_cast<unsigned char>('T'):
        case static_cast<unsigned char>('U'):
        case static_cast<unsigned char>('V'):
        case static_cast<unsigned char>('W'):
        case static_cast<unsigned char>('X'):
        case static_cast<unsigned char>('Y'):
        case static_cast<unsigned char>('Z'):
        case static_cast<unsigned char>('a'):
        case static_cast<unsigned char>('b'):
        case static_cast<unsigned char>('c'):
        case static_cast<unsigned char>('d'):
        case static_cast<unsigned char>('e'):
        case static_cast<unsigned char>('f'):
        case static_cast<unsigned char>('g'):
        case static_cast<unsigned char>('h'):
        case static_cast<unsigned char>('i'):
        case static_cast<unsigned char>('j'):
        case static_cast<unsigned char>('k'):
        case static_cast<unsigned char>('l'):
        case static_cast<unsigned char>('m'):
        case static_cast<unsigned char>('n'):
        case static_cast<unsigned char>('o'):
        case static_cast<unsigned char>('p'):
        case static_cast<unsigned char>('q'):
        case static_cast<unsigned char>('r'):
        case static_cast<unsigned char>('s'):
        case static_cast<unsigned char>('t'):
        case static_cast<unsigned char>('u'):
        case static_cast<unsigned char>('v'):
        case static_cast<unsigned char>('w'):
        case static_cast<unsigned char>('x'):
        case static_cast<unsigned char>('y'):
        case static_cast<unsigned char>('z'): {
            {
                mALPHA(false);
            }
            break;
        }
        case static_cast<unsigned char>('_'): {
            match(static_cast<unsigned char>('_'));
            break;
        }
        default: {
            throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
        }
        }
    }
    {
        for (;;) {
            switch (LA(1)) {
            case static_cast<unsigned char>('.'):
            case static_cast<unsigned char>('0'):
            case static_cast<unsigned char>('1'):
            case static_cast<unsigned char>('2'):
            case static_cast<unsigned char>('3'):
            case static_cast<unsigned char>('4'):
            case static_cast<unsigned char>('5'):
            case static_cast<unsigned char>('6'):
            case static_cast<unsigned char>('7'):
            case static_cast<unsigned char>('8'):
            case static_cast<unsigned char>('9'):
            case static_cast<unsigned char>('A'):
            case static_cast<unsigned char>('B'):
            case static_cast<unsigned char>('C'):
            case static_cast<unsigned char>('D'):
            case static_cast<unsigned char>('E'):
            case static_cast<unsigned char>('F'):
            case static_cast<unsigned char>('G'):
            case static_cast<unsigned char>('H'):
            case static_cast<unsigned char>('I'):
            case static_cast<unsigned char>('J'):
            case static_cast<unsigned char>('K'):
            case static_cast<unsigned char>('L'):
            case static_cast<unsigned char>('M'):
            case static_cast<unsigned char>('N'):
            case static_cast<unsigned char>('O'):
            case static_cast<unsigned char>('P'):
            case static_cast<unsigned char>('Q'):
            case static_cast<unsigned char>('R'):
            case static_cast<unsigned char>('S'):
            case static_cast<unsigned char>('T'):
            case static_cast<unsigned char>('U'):
            case static_cast<unsigned char>('V'):
            case static_cast<unsigned char>('W'):
            case static_cast<unsigned char>('X'):
            case static_cast<unsigned char>('Y'):
            case static_cast<unsigned char>('Z'):
            case static_cast<unsigned char>('_'):
            case static_cast<unsigned char>('a'):
            case static_cast<unsigned char>('b'):
            case static_cast<unsigned char>('c'):
            case static_cast<unsigned char>('d'):
            case static_cast<unsigned char>('e'):
            case static_cast<unsigned char>('f'):
            case static_cast<unsigned char>('g'):
            case static_cast<unsigned char>('h'):
            case static_cast<unsigned char>('i'):
            case static_cast<unsigned char>('j'):
            case static_cast<unsigned char>('k'):
            case static_cast<unsigned char>('l'):
            case static_cast<unsigned char>('m'):
            case static_cast<unsigned char>('n'):
            case static_cast<unsigned char>('o'):
            case static_cast<unsigned char>('p'):
            case static_cast<unsigned char>('q'):
            case static_cast<unsigned char>('r'):
            case static_cast<unsigned char>('s'):
            case static_cast<unsigned char>('t'):
            case static_cast<unsigned char>('u'):
            case static_cast<unsigned char>('v'):
            case static_cast<unsigned char>('w'):
            case static_cast<unsigned char>('x'):
            case static_cast<unsigned char>('y'):
            case static_cast<unsigned char>('z'): {
                mFNAMECHAR(false);
                break;
            }
            case static_cast<unsigned char>('/'):
            case static_cast<unsigned char>('\\'): {
                mDELIM(false);
                break;
            }
            default: {
                goto _loop70;
            }
            }
        }
    _loop70:;
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mIDENTIFIER(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = IDENTIFIER;
    int _saveIndex;

    {
        switch (LA(1)) {
        case static_cast<unsigned char>('A'):
        case static_cast<unsigned char>('B'):
        case static_cast<unsigned char>('C'):
        case static_cast<unsigned char>('D'):
        case static_cast<unsigned char>('E'):
        case static_cast<unsigned char>('F'):
        case static_cast<unsigned char>('G'):
        case static_cast<unsigned char>('H'):
        case static_cast<unsigned char>('I'):
        case static_cast<unsigned char>('J'):
        case static_cast<unsigned char>('K'):
        case static_cast<unsigned char>('L'):
        case static_cast<unsigned char>('M'):
        case static_cast<unsigned char>('N'):
        case static_cast<unsigned char>('O'):
        case static_cast<unsigned char>('P'):
        case static_cast<unsigned char>('Q'):
        case static_cast<unsigned char>('R'):
        case static_cast<unsigned char>('S'):
        case static_cast<unsigned char>('T'):
        case static_cast<unsigned char>('U'):
        case static_cast<unsigned char>('V'):
        case static_cast<unsigned char>('W'):
        case static_cast<unsigned char>('X'):
        case static_cast<unsigned char>('Y'):
        case static_cast<unsigned char>('Z'):
        case static_cast<unsigned char>('a'):
        case static_cast<unsigned char>('b'):
        case static_cast<unsigned char>('c'):
        case static_cast<unsigned char>('d'):
        case static_cast<unsigned char>('e'):
        case static_cast<unsigned char>('f'):
        case static_cast<unsigned char>('g'):
        case static_cast<unsigned char>('h'):
        case static_cast<unsigned char>('i'):
        case static_cast<unsigned char>('j'):
        case static_cast<unsigned char>('k'):
        case static_cast<unsigned char>('l'):
        case static_cast<unsigned char>('m'):
        case static_cast<unsigned char>('n'):
        case static_cast<unsigned char>('o'):
        case static_cast<unsigned char>('p'):
        case static_cast<unsigned char>('q'):
        case static_cast<unsigned char>('r'):
        case static_cast<unsigned char>('s'):
        case static_cast<unsigned char>('t'):
        case static_cast<unsigned char>('u'):
        case static_cast<unsigned char>('v'):
        case static_cast<unsigned char>('w'):
        case static_cast<unsigned char>('x'):
        case static_cast<unsigned char>('y'):
        case static_cast<unsigned char>('z'): {
            {
                mALPHA(false);
            }
            break;
        }
        case static_cast<unsigned char>('_'): {
            match(static_cast<unsigned char>('_'));
            break;
        }
        default: {
            throw ANTLR_USE_NAMESPACE(antlr) NoViableAltForCharException(LA(1), getFilename(), getLine());
        }
        }
    }
    {
        for (;;) {
            switch (LA(1)) {
            case static_cast<unsigned char>('0'):
            case static_cast<unsigned char>('1'):
            case static_cast<unsigned char>('2'):
            case static_cast<unsigned char>('3'):
            case static_cast<unsigned char>('4'):
            case static_cast<unsigned char>('5'):
            case static_cast<unsigned char>('6'):
            case static_cast<unsigned char>('7'):
            case static_cast<unsigned char>('8'):
            case static_cast<unsigned char>('9'):
            case static_cast<unsigned char>('A'):
            case static_cast<unsigned char>('B'):
            case static_cast<unsigned char>('C'):
            case static_cast<unsigned char>('D'):
            case static_cast<unsigned char>('E'):
            case static_cast<unsigned char>('F'):
            case static_cast<unsigned char>('G'):
            case static_cast<unsigned char>('H'):
            case static_cast<unsigned char>('I'):
            case static_cast<unsigned char>('J'):
            case static_cast<unsigned char>('K'):
            case static_cast<unsigned char>('L'):
            case static_cast<unsigned char>('M'):
            case static_cast<unsigned char>('N'):
            case static_cast<unsigned char>('O'):
            case static_cast<unsigned char>('P'):
            case static_cast<unsigned char>('Q'):
            case static_cast<unsigned char>('R'):
            case static_cast<unsigned char>('S'):
            case static_cast<unsigned char>('T'):
            case static_cast<unsigned char>('U'):
            case static_cast<unsigned char>('V'):
            case static_cast<unsigned char>('W'):
            case static_cast<unsigned char>('X'):
            case static_cast<unsigned char>('Y'):
            case static_cast<unsigned char>('Z'):
            case static_cast<unsigned char>('a'):
            case static_cast<unsigned char>('b'):
            case static_cast<unsigned char>('c'):
            case static_cast<unsigned char>('d'):
            case static_cast<unsigned char>('e'):
            case static_cast<unsigned char>('f'):
            case static_cast<unsigned char>('g'):
            case static_cast<unsigned char>('h'):
            case static_cast<unsigned char>('i'):
            case static_cast<unsigned char>('j'):
            case static_cast<unsigned char>('k'):
            case static_cast<unsigned char>('l'):
            case static_cast<unsigned char>('m'):
            case static_cast<unsigned char>('n'):
            case static_cast<unsigned char>('o'):
            case static_cast<unsigned char>('p'):
            case static_cast<unsigned char>('q'):
            case static_cast<unsigned char>('r'):
            case static_cast<unsigned char>('s'):
            case static_cast<unsigned char>('t'):
            case static_cast<unsigned char>('u'):
            case static_cast<unsigned char>('v'):
            case static_cast<unsigned char>('w'):
            case static_cast<unsigned char>('x'):
            case static_cast<unsigned char>('y'):
            case static_cast<unsigned char>('z'): {
                {
                    mALPHANUM(false);
                }
                break;
            }
            case static_cast<unsigned char>('_'): {
                match(static_cast<unsigned char>('_'));
                break;
            }
            default: {
                goto _loop76;
            }
            }
        }
    _loop76:;
    }
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

void DoomLexer::mFILE_ID_NUMBER(bool _createToken)
{
    int                                 _ttype;
    ANTLR_USE_NAMESPACE(antlr) RefToken _token;
    int                                 _begin = text.length();
    _ttype                                     = FILE_ID_NUMBER;
    int _saveIndex;

    bool synPredMatched81 = false;
    if (((_tokenSet_4.member(LA(1))) && (true) && (true))) {
        int _m81         = mark();
        synPredMatched81 = true;
        inputState->guessing++;
        try {
            {
                {
                    mIDENTIFIER(false);
                }
                {
                    mDELIM(false);
                }
            }
        } catch (ANTLR_USE_NAMESPACE(antlr) RecognitionException& pe) {
            synPredMatched81 = false;
        }
        rewind(_m81);
        inputState->guessing--;
    }
    if (synPredMatched81) {
        {
            mFILENAME(false);
        }
        if (inputState->guessing == 0) {
            _ttype = FILENAME; /*printf("FILE %s\n\n",getText().c_str());*/
        }
    } else {
        bool synPredMatched86 = false;
        if (((_tokenSet_4.member(LA(1))) && (true) && (true))) {
            int _m86         = mark();
            synPredMatched86 = true;
            inputState->guessing++;
            try {
                {
                    {
                        mIDENTIFIER(false);
                    }
                    {
                        mFCHAR(false);
                    }
                }
            } catch (ANTLR_USE_NAMESPACE(antlr) RecognitionException& pe) {
                synPredMatched86 = false;
            }
            rewind(_m86);
            inputState->guessing--;
        }
        if (synPredMatched86) {
            {
                mFILENAME(false);
            }
            if (inputState->guessing == 0) {
                _ttype = FILENAME; /*printf("FILE %s\n\n",getText().c_str());*/
            }
        } else if ((_tokenSet_4.member(LA(1))) && (true) && (true)) {
            mIDENTIFIER(false);
            if (inputState->guessing == 0) {
                _ttype = IDENTIFIER; /*printf("IDENTIFIER %s\n\n",getText().c_str());*/
            }
        } else {
        }
    }
    _ttype = testLiteralsTable(_ttype);
    if (_createToken && _token == ANTLR_USE_NAMESPACE(antlr) nullToken &&
        _ttype != ANTLR_USE_NAMESPACE(antlr) Token::SKIP) {
        _token = makeToken(_ttype);
        _token->setText(text.substr(_begin, text.length() - _begin));
    }
    _returnToken = _token;
}

const unsigned long DoomLexer::_tokenSet_0_data_[] = {4294958079UL, 4294967291UL, 4294967295UL, 4294967295UL,
                                                      4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL,
                                                      0UL,          0UL,          0UL,          0UL,
                                                      0UL,          0UL,          0UL,          0UL};
const ANTLR_USE_NAMESPACE(antlr) BitSet DoomLexer::_tokenSet_0(_tokenSet_0_data_, 16);
const unsigned long DoomLexer::_tokenSet_1_data_[] = {4294958079UL, 4294966271UL, 4294967295UL, 4294967295UL,
                                                      4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL,
                                                      0UL,          0UL,          0UL,          0UL,
                                                      0UL,          0UL,          0UL,          0UL};
const ANTLR_USE_NAMESPACE(antlr) BitSet DoomLexer::_tokenSet_1(_tokenSet_1_data_, 16);
const unsigned long DoomLexer::_tokenSet_2_data_[] = {4294958079UL, 4294967295UL, 4294967295UL, 4294967295UL,
                                                      4294967295UL, 4294967295UL, 4294967295UL, 4294967295UL,
                                                      0UL,          0UL,          0UL,          0UL,
                                                      0UL,          0UL,          0UL,          0UL};
const ANTLR_USE_NAMESPACE(antlr) BitSet DoomLexer::_tokenSet_2(_tokenSet_2_data_, 16);
const unsigned long DoomLexer::_tokenSet_3_data_[] = {0UL, 67059712UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL, 0UL};
const ANTLR_USE_NAMESPACE(antlr) BitSet DoomLexer::_tokenSet_3(_tokenSet_3_data_, 10);
const unsigned long DoomLexer::_tokenSet_4_data_[] = {0UL, 0UL, 2281701374UL, 134217726UL, 0UL,
                                                      0UL, 0UL, 0UL,          0UL,         0UL};
const ANTLR_USE_NAMESPACE(antlr) BitSet DoomLexer::_tokenSet_4(_tokenSet_4_data_, 10);
