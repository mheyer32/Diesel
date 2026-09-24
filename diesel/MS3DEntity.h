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
#pragma once
#include "Animator.h"
#include "Entity.h"

class MS3DEntity : public CEntity
{
public:
    MS3DEntity(MS3DModel* model = 0);

    inline Animator& getAnimator() { return m_animator; };
    void             setMS3DModel(MS3DModel* model);

protected:
    virtual ~MS3DEntity(void);

    MS3DModel::SMARTPTR m_ms3dmodel;
    Animator            m_animator;
};
