#ifndef INC_DoomLexer_hpp_
#define INC_DoomLexer_hpp_


	#pragma warning(disable: 4267 4101)
    // gets inserted before antlr generated includes in the header file

#include "antlr/config.hpp"
/* $ANTLR 2.7.0: "Doom3.g" -> "DoomLexer.hpp"$ */
#include "antlr/CommonToken.hpp"
#include "antlr/InputBuffer.hpp"
#include "antlr/BitSet.hpp"
#include "DoomTokenTypes.hpp"
#include "antlr/CharScanner.hpp"

    // gets inserted after antlr generated includes in the header file
	 // outside any generated namespace specifications

class DoomLexer : public ANTLR_USE_NAMESPACE(antlr)CharScanner, public DoomTokenTypes
 {


private:
	void initLiterals();
public:
	bool getCaseSensitiveLiterals() const;
public:
	DoomLexer(ANTLR_USE_NAMESPACE(std)istream& in);
	DoomLexer(ANTLR_USE_NAMESPACE(antlr)InputBuffer& ib);
	DoomLexer(const ANTLR_USE_NAMESPACE(antlr)LexerSharedInputState& state);
	ANTLR_USE_NAMESPACE(antlr)RefToken nextToken();
	public: void mCOMMA(bool _createToken);
	public: void mLPAREN(bool _createToken);
	public: void mRPAREN(bool _createToken);
	public: void mLBRACKET(bool _createToken);
	public: void mRBRACKET(bool _createToken);
	public: void mLCURLY(bool _createToken);
	public: void mRCURLY(bool _createToken);
	public: void mEQUAL(bool _createToken);
	public: void mNOTEQUAL(bool _createToken);
	public: void mLTE(bool _createToken);
	public: void mLESS(bool _createToken);
	public: void mGTE(bool _createToken);
	public: void mGREATER(bool _createToken);
	public: void mDIV(bool _createToken);
	public: void mPLUS(bool _createToken);
	public: void mMINUS(bool _createToken);
	public: void mSTAR(bool _createToken);
	public: void mMOD(bool _createToken);
	public: void mLAND(bool _createToken);
	public: void mLNOT(bool _createToken);
	public: void mLOR(bool _createToken);
	public: void mWHITESPACE(bool _createToken);
	public: void mSTRING(bool _createToken);
	public: void mCOMMENT(bool _createToken);
	public: void mCPPCOMMENT(bool _createToken);
	protected: void mDIGIT(bool _createToken);
	protected: void mALPHA(bool _createToken);
	protected: void mALPHANUM(bool _createToken);
	protected: void mFLOAT(bool _createToken);
	protected: void mINT(bool _createToken);
	public: void mNUMBER(bool _createToken);
	protected: void mFNAMECHAR(bool _createToken);
	protected: void mFCHAR(bool _createToken);
	protected: void mDELIM(bool _createToken);
	protected: void mFILENAME(bool _createToken);
	protected: void mIDENTIFIER(bool _createToken);
	public: void mFILE_ID_NUMBER(bool _createToken);
private:
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
};

#endif /*INC_DoomLexer_hpp_*/
