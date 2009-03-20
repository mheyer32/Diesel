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
// BaseScene.h: Schnittstelle für die Klasse CBaseScene.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASESCENE_H__B13DE18D_2755_4896_BD0A_FCF0CC38B5C6__INCLUDED_)
#define AFX_BASESCENE_H__B13DE18D_2755_4896_BD0A_FCF0CC38B5C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Entity.h"

class CBaseScene  
{
public:
	CBaseScene();
	virtual ~CBaseScene();

	virtual void cullScene()=0;
	virtual void renderScene()=0;
	virtual bool initialize()=0;
	virtual void addEntity(CEntity *newEntity)=0;
	virtual void addEntities(CEntity::ENTITYLIST &list)=0;

};

#endif // !defined(AFX_BASESCENE_H__B13DE18D_2755_4896_BD0A_FCF0CC38B5C6__INCLUDED_)
