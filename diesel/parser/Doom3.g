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



class DoomLexer extends Lexer;

options
        {
        k = 3;
        exportVocab = Doom;
        testLiterals = false;
        caseSensitiveLiterals = false;
		charVocabulary = '\u0000'..'\u00FF';
        }
{

}

/* Operators: */

COMMA           : ',' ;

LPAREN          : '(' ;
RPAREN          : ')' ;
LBRACKET        : '[' ;
RBRACKET        : ']' ;
LCURLY          : '{' ;
RCURLY          : '}' ;

EQUAL           : "==" ;
NOTEQUAL        :  "!=" ;
LTE             : "<=" ;
LESS            : "<" ;
GTE             : ">=" ;
GREATER         : ">" ;

	
DIV             : '/' ;
PLUS            : '+' ;
MINUS           : '-' ;
STAR            : '*' ;
MOD             : '%' ;

LAND            : "&&" ;
LNOT            : '!' ;
LOR             : "||" ;


WHITESPACE
				:      ('\t' | '\f' | ' ' | '\n' | '\r')   { $setType(antlr::Token::SKIP); }
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
DIGIT
				: '0'..'9'
				;
				
protected        
ALPHA	
				: ('a'..'z') | ('A'..'Z')
				;

protected
ALPHANUM		: (ALPHA) | (DIGIT)
				;
protected
FLOAT			:;

protected
INT				:;

NUMBER
				: ( (DIGIT)*('.')) =>  (DIGIT)* ('.') (DIGIT)+ {$setType(FLOAT);/*printf("FLOAT %s\n\n",getText().c_str());*/}
				  | ( DIGIT )+      {$setType(INT);/*printf("INT %s\n\n",getText().c_str());*/}
				;

protected
FNAMECHAR
				: (ALPHANUM|'_'|'.')
				;
protected		
FCHAR	
				: (	'.' )
	    		;
protected
DELIM	
				: '/' | '\\'
				;
				
protected				
FILENAME
				: ((ALPHA)|'_')(FNAMECHAR|DELIM)*
				;
    
protected
IDENTIFIER
				: ( ( ALPHA ) | '_' ) ( ( ALPHANUM ) | '_' )*
				;    

FILE_ID_NUMBER
 	  options 
      {
         testLiterals = true; 
      }
                :
 			   	|((IDENTIFIER)(DELIM)) => (FILENAME) {$setType(FILENAME);/*printf("FILE %s\n\n",getText().c_str());*/}
			    | ((IDENTIFIER)(FCHAR)) => (FILENAME)  {$setType(FILENAME);/*printf("FILE %s\n\n",getText().c_str());*/}
			    | IDENTIFIER {$setType(IDENTIFIER);/*printf("IDENTIFIER %s\n\n",getText().c_str());*/}
				;


//===============================================================================================================

class DoomParser extends Parser;

options
{
	buildAST=true;
}

vec2				: LPAREN! number number RPAREN!
					;
vec3				: LPAREN! number number number RPAREN!
					;	
vec4				: LPAREN! number number number number RPAREN!
					;			
vec5				: LPAREN! number number number number number RPAREN!
					;
matrix3x2			: LPAREN! vec3 vec3 RPAREN!
					;
filename			: (FILENAME)|(IDENTIFIER)
					;
real				: (MINUS)?(FLOAT)
					;
integer				: (MINUS)?(INT)
					;
number				: (MINUS)? (INT|FLOAT)
					;
		
					
/*======================================
MAPFILE
=======================================*/

mapfile			:  	 mapheader!
					 (entity)*
					;
mapheader		:	(anytext)*
					;
anytext			:	((number)|(IDENTIFIER)|(FILENAME))
					;
entity			:	 {std::cout<<"ENTITY:"<<std::endl;}
					LCURLY! (keyvalue)* (primitive)* RCURLY!
					;
keyvalue		:	key:STRING value:STRING {std::cout<<"key: "<<key->getText()<<" value: "<<value->getText()<<std::endl;}
					;
primitive		:	LCURLY! primitivedef RCURLY!
					;
primitivedef	:	brushdef3 | patchdef2 | patchdef3
					;
brushdef3		:	"brushDef3" 
					LCURLY!
					 (brushside)+
					RCURLY!
					;
brushside		:	plane matrix3x2 texturename INT INT INT
					;
plane			:	vec4
					;
texturename		:	filename
					;
patchdef2		:	"patchDef2"
					LCURLY!
						texturename
						LPAREN! INT INT INT INT INT RPAREN!
						LPAREN!
							(ctrlpointrow)+
						RPAREN!
					RCURLY!
					;
ctrlpointrow		:	LPAREN! (controlpoint)+ RPAREN!
					;
controlpoint		: vec5
					;
patchdef3			:	"patchDef3"
						LCURLY!
							texturename
							LPAREN! INT INT INT INT INT INT INT RPAREN!
							LPAREN!
								(ctrlpointrow)+
							RPAREN!
						RCURLY!
					;

/*=============================================================================
PROCFILE
===============================================================================*/
procfile		:	{cout<<"parsing procfile"<<endl;}
					IDENTIFIER! (procfileparts)+
					{cout<<"parsed procfile"<<endl;}
				;

procfileparts	:	 model
					|interareaportals
					|bsp
					|shadowmodel
				;
/* models =======================================*/
model		:	{cout<<"parsing model"<<endl;}
				"model" LCURLY! modelname integer
				surfaces
				RCURLY!
				{cout<<"parsed a model!!"<<endl;}
				;

modelname	:	{cout<<"parsing modelname"<<endl;}
				name:STRING
				{cout<<"\nMODEL: "<<name->getText()<<endl<<endl;}
				;

/* surfaces =======================================*/

surfaces	:	( surface )*
				;

surface		:	{cout<<"parsing surface"<<endl;}
				LCURLY! surfacematerial integer integer 
					vertices
					indices
				RCURLY!
				{cout<<"parsed surface"<<endl;}
				;
				
surfacematerial : STRING
				;

vertices	:	(vertex)+
				;

vertex		:	LPAREN! number number number number number number number number  RPAREN!
				;

indices		:	(index)+
				;

index		:	integer
				;

/*portals =======================================*/

interareaportals : {cout<<"parsing interareaportals"<<endl;}
					"interareaportals"
					LCURLY! integer integer
						(portal)+
					RCURLY!
					{cout<<"parsed interareaportals\n"<<endl;}
					;
portal			:	{printf("parsing portal\n");}
					integer integer integer points
					{printf("parsed portal\n");}
				;

points			:	(vec3)+
				;
/*Bsp =======================================*/

bsp				:	{cout<<"parsing nodes"<<endl;}
					"nodes" LCURLY! integer
					nodes
					RCURLY!
					{cout<<"parsed nodes"<<endl;}
				;
				
nodes			:	(node)*
				;

node			:	plane positivechild negativechild
				;

positivechild	: integer
				;

negativechild	: integer
				;

/*shadowmodels =======================================*/

shadowmodel		:	{cout<<"parsing shadowmodel"<<endl;}
					"shadowmodel" LCURLY! modelname 
						integer integer integer integer integer
						points
						indices
					RCURLY!
					{cout<<"parsed shadowmodel"<<endl;}
				;

/*======================================
MTRFILE
=======================================*/				
			
mtrfile			:	(mtrfileitem)*
				;
mtrfileitem		:  tabledecl | material | define
				;
tabledecl		:	tabletype tablename LCURLY! listoffloats RCURLY!
				;
tabletype		:	"table"
					| "clamptable"
					| "snaptable"
					| "soundtable"
					;
tablename		:	tab:IDENTIFIER {cout<<"TABLE: "<<tab->getText()<<endl;}
					;
listoffloats	:	number (COMMA! number)*
					;	
define			:	"#define" IDENTIFIER anytext {/*muss noch verbessert werden*/}
					;
material		:	materialname 
					LCURLY!
							(matstatement)*
					RCURLY!
					;
materialname	:	file:FILENAME {#materialname=#file; cout<<"\nMATERIAL: "<<file->getText()<<endl;}
				    | id:IDENTIFIER {#materialname=#id; cout<<"\nMATERIAL: "<<id->getText()<<endl;}
					;
matstatement		: globalstatement | stagestatement
					;
globalstatement		: editorattribute! | surfaceattribute 
					;
editorattribute		:	 "qer_editorimage" FILENAME
						| "qer_trans" number
						| "qer_nocarve"
					;
surfaceattribute	:	  guisurf
						| surfaceparm
						| translucent
						| description
						| polygonoffset
						| noshadows
						| twosided
						| flarepic
						| forceshadows {/*invisible.mtr*/}
						| noselfshadow {/*kentest.mtr*/}
						| nofragment {/*senetemp.mtr*/}
						| sort {/*decals.mtr*/}
						| foglight {/*fogs.mtr*/}
						| blendlight {/*fogs.mtr*/}
						| ambientlight {/*lights.mtr*/}
						| lightfalloffimage {/*fogs.mtr*/}
						| forceopaque {/*gfx.mtr*/}
						| renderbump  {/*characters.mtr*/}
						| deform   {/*characters.mtr, decals.mtr, shaderdemos.mtr: deform sprite, deform eyeball,deform flare ( 16 + parm4 ),deform	expand 4*sintable[time*0.5]deform	move	10*sintable[time*0.5] // *sound
*/}	
						| flare
						| IDENTIFIER	{/* macro??*/}

						;
/*---------------------------------------------------------------------------------*/
guisurf				:	"guisurf"! (guisurface)?
					;
guisurface			:   FILENAME /* | IDENTIFIER*/
					;
translucent			:	"translucent"!;
polygonoffset		:	"polygonoffet"!;
noshadows			:	"noshadows"!;
twosided			:	"twosided"!;
noselfshadow		:	"noselfshadow"!;
forceshadows		:	"forceshadows"!;
nofragment			:	"nofragment"!;
forceopaque			:	"forceopaque"! {/*mglw. sort = opaque erzwingen */};	
sort				:	"sort"! (number)?;
foglight			:	"foglight"!;
blendlight			:	"blendlight"!;
ambientlight		:	"ambientlight"!;
lightfalloffimage	:	"lightfalloffimage"! texture;
flarepic			:	"flarepic"! texture;
description			:	"description"! desc:STRING {cout<<"Description:"<<desc->getText()<<endl;}
					;
renderbump			:	"renderbump"! (rboption)* FILENAME FILENAME
					;
rboption			:	MINUS rboption2
					;
rboption2			:	"size" integer integer
						| "aa" integer
						| "trace" number
						| "colormap"
						;
/*---------------------------------------------------------------------------------*/
surfaceparm		:	("surfaceparm")? spparam
					;
spparam				:	  nodraw {/*invisible.mtr  teilweise mit surfaceparam davor*/}
						| nodrop {/*invisible.mtr*/}
						| noimpact {/*invisible.mtr*/}
						| nonsolid {/*invisible.mtr*/}
						| monsterclip {/*invisible.mtr*/}
						| playerclip {/*invisible.mtr*/}
						| slick {/*invisible.mtr*/}
						| discrete {/*invisible.mtr*/}
						| ladder
						| trans
						| blood
						;
nodraw				: "nodraw"!;
nodrop				: "nodrop"!;
noimpact			: "noimpact"!;
nonsolid			: "nonsolid"!;
monsterclip			: "monsterclip"!;
playerclip			: "playerclip"!;
slick				: "slick"!;
discrete			: "discrete"!;
ladder				: "ladder"!;
trans				: "trans"!;
blood				: "blood"!;
/*---------------------------------------------------------------------------------*/
deform				: "deform"! deformfunc
					;
deformfunc			:	  "sprite"
						| "eyeball"
						| "tube"
						| "flare" expr /* NICHT VERWECHSELN mit alleinestehendem flar */
						| "expand" expr
						| "move" expr
						;
flare				: "flare"! vec3 vec2 vec2 vec2
					;
/*---------------------------------------------------------------------------------*/
					
				// the mother of all expressions
expr				: logicalOrExpression
					;

logicalOrExpression	:	logicalAndExpression (LOR^ logicalAndExpression)*
					;
logicalAndExpression :	equalityExpression ( LAND^ equalityExpression )*
					;
equalityExpression	:	relationalExpression (( NOTEQUAL^ |EQUAL^)	relationalExpression )*
					;
relationalExpression :	additiveExpression(	(LESS^ | GREATER^ | LTE^ | GTE^ ) additiveExpression)*
					;
additiveExpression	:	multiplicativeExpression	(( PLUS^ | MINUS^) multiplicativeExpression)*
					;
multiplicativeExpression	:	unaryExpression ((STAR^ | DIV^ | MOD^ ) unaryExpression	)*
					;
unaryExpression		:	(MINUS^ | PLUS^) unaryExpression
					| unaryExpressionNotPlusMinus
					;
unaryExpressionNotPlusMinus : (LNOT^) unaryExpression
						| postfixExpression
					;
postfixExpression	:  FLOAT
					| INT
					| "true"
					| "false"
					| (IDENTIFIER LBRACKET) => tableaccess
					| id:IDENTIFIER 	{cout<<"Identifier in EXPR: "<<id->getText()<<endl;/*parm0-7, sound, global usw*/}
					| LPAREN! expr RPAREN!

					;
/*--------------------------------------------------------------------------------*/						
tableaccess			:	 table:IDENTIFIER {cout<<"\nACCESS TO TABLE: "<<table->getText()<<endl;} 
						LBRACKET! expr RBRACKET!
						;						
/*---------------------------------------------------------------------------------*/

stagestatement		:	"bumpmap" texture
						| "diffusemap" texture
						| "specularmap" texture
						| customstage
						;
texture				: 	 heightmap
						| addnormals
						| makeintensity
						| texturefile
						| IDENTIFIER /*built-in textures*/
						;
heightmap			:	"heightmap"! LPAREN! texture COMMA! number RPAREN!
						;
addnormals			:	"addnormals"! LPAREN! texture COMMA! texture RPAREN!
						;
makeintensity		:	"makeintensity"! LPAREN! texture RPAREN!
						;
texturefile			:	FILENAME | STRING
						;												
customstage			:	LCURLY! 
							(condition)?
							stageattributes
						RCURLY!
						;
condition			:	"if" LPAREN! expr RPAREN!
						;

stageattributes		:	(stageattribute)*
						;
stageattribute		:	  map	
						| cubemap	
						| videomap	
						| remoterendermap
						| cameracubemap
						| mirrorrendermap
						| noisemap
						
						| blend
						
						| color
						| colored	/* take rgba from entity shaderparms */
						| vertexcolor
						| inversevertexcolor
						| rgb
						| rgba
						| red
						| green 
						| blue 
						| alpha	
				
						
						| translate 
						| rotate 
						| shear
						| scroll 
						| scale
						| centerscale

						| texgen
						
						| alphatest
						| ignorealphatest
						
						| zeroclamp /* make sure it doesn't bleed over the edges */
						| alphazeroclamp /* make sure it doesn't bleed over the edges*/
						| clamp

						| shadowdraw /*// this stage will be drawn on the shadows, still modulated by the lightFalloffImage
										stenciltest aus ?
									*/

						| maskcolor /* keine color-writes, aber alphawrites!*/
						| maskdepth /* keine zwrites? */ 
						| nopicmip 
						| linear
						| nearest
						| highquality
						;
						
map					:	"map"! texture;
cubemap				:	"cubemap"! texturefile;
videomap			:	"videomap"! texturefile;
cameracubemap		:	"cameracubemap"! texturefile;
remoterendermap		:	"remoterendermap"! INT INT ;
mirrorrendermap		:	"mirrorrendermap"! INT INT;
noisemap			:	"noisemap"! expr;

blend				:   "blend"! blendparam;
blendparam			:	  "add" | "filter" | "blend" | "diffusemap" | "bumpmap" | "specularmap" 
						| glblend COMMA! glblend
						;
glblend				:	"gl_one" | "gl_zero"
						| "gl_src_color" | "gl_one_minus_src_color" 
						| "gl_dst_color"  | "gl_one_minus_dst_color"
						| "gl_src_alpha" | "gl_one_minus_src_alpha"
						| "gl_dst_alpha" | "gl_one_minus_dst_alpha"
						;
						
color				:	"color"! expr COMMA! expr COMMA! expr COMMA! expr;
colored				:	"colored"!;
vertexcolor			:	"vertexcolor"!;
inversevertexcolor	:	"inversevertexcolor"!;
rgb					:	"rgb"! expr;
rgba				:	"rgba"! expr;
red					:	"red"! expr;
green				:	"green"! expr;
blue				:	"blue"! expr;
alpha				:	"alpha"! expr;

translate			:	"translate"! expr COMMA! expr;
rotate				:	"rotate"!	expr;
shear				:	"shear"!expr COMMA! expr;
scroll				:	"scroll"! 	expr COMMA! expr;
scale				:	"scale"	expr COMMA! expr;
centerscale			:	"centerscale" expr COMMA! expr;

texgen				:	"texgen"! texgenoption;
texgenoption		:	"normal" | "reflect" | "skybox";

alphatest			:	"alphatest"! expr;
ignorealphatest		:	"ignorealphatest"!;

zeroclamp			:	"zeroclamp"!;
alphazeroclamp		:	"alphazeroclamp"!;
clamp				:	"clamp"!;

shadowdraw			:	"shadowdraw"!;
maskcolor			:	"maskcolor"!;
maskalpha			:	"maskalpha"!;
maskdepth			:	"maskdepth"!;
nopicmip			:	"nopicmip"!;
linear				:	"linear"!;
nearest				:	"nearest"!;
highquality			:	"highquality"!;
				