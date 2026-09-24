/*
This file is part of Diesel
(c) 2002-2026 by Mathias Heyer
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
header "pre_include_hpp" {
	#pragma warning(disable: 4267 4101)
    // gets inserted before antlr generated includes in the header file
}
header "post_include_hpp" {
    // gets inserted after antlr generated includes in the header file
	 // outside any generated namespace specifications
}

header "pre_include_cpp" {
    // gets inserted after the antlr generated includes in the cpp file
}

header "post_include_cpp" {
    // gets inserted after the antlr generated includes in the cpp file


}

header {
	// gets inserted after generated namespace specifications in the header
	// file. But outside the generated class.
	#include <iostream>
    using namespace std;
}

options {
   language="Cpp";
  //  namespace="something";      // encapsulate code in this namespace
  //namespaceStd="std";         // cosmetic option to get rid of long defines
                                  // in generated code
 //namespaceAntlr="antlr";     // cosmetic option to get rid of long defines
                                // in generated code
  // genHashLines = true;        // generated #line's or turn it off.

}



class DoomProcLexer extends Lexer;

options
        {
        k = 3;
        exportVocab = DoomProc;
        testLiterals = false;
        caseSensitiveLiterals = false;
		charVocabulary = '\u0000'..'\u00FF';
		}

tokens
{	
	"model";
	"interareaportals";
	"nodes";
    "shadowmodels";
}

COMMA           : ',' ;
LPAREN          : '(' ;
RPAREN          : ')' ;
LCURLY          : '{' ;
RCURLY          : '}' ;

WHITESPACE
				: ('\t' | '\f' | ' ' | '\n' | '\r')   { $setType(antlr::Token::SKIP); }
				;
STRING
				: '"'!
					(~( '"' | '\r' | '\n' ))*
                  '"'!
				;
COMMENT
				:  "/*"
                ( { LA(2) != '/' }? '*'
                | ("\r\n") => "\r\n"               
                | ( '\r' | '\n')     
                | ~( '*'| '\r' | '\n' )
                )*
                "*/"  {/*printf("COMMENT %s\n\n",getText().c_str());*/ $setType(antlr::Token::SKIP); }
				;
CPPCOMMENT    
				: "//"( ~('\n'|'\r') )*  { /*printf("COMMENT %s\n\n",getText().c_str());*/$setType(antlr::Token::SKIP); }
				;
		
protected
DIGIT			: '0'..'9'
				;

protected
FLOAT			:	
				;

protected
INT				: 
				;
NUMBER			:( ('-')?(DIGIT)*('.')) =>  ('-')?(DIGIT)*('.')(DIGIT)+ {$setType(FLOAT);}
				| ('-')?( DIGIT )+ {$setType(INT);}
				;
protected        
ALPHA	
				: ('a'..'z') | ('A'..'Z')
				;

protected
ALPHANUM		: (ALPHA) | (DIGIT)
				;

IDENTIFIER
 			options 
			{
				testLiterals = true; 
			}
      		: ( ( ALPHA ) | '_' ) ( ( ALPHANUM ) | '_' )*
				;
