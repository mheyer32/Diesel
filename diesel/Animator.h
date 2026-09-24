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
#include "Task.h"

#include "MS3DAnimation.h"
#include "MS3DModel.h"
#include "Mesh.h"

class Animator : public CTask
{
public:
    enum ANIMMODE
    {
        REPEATING,
        ONCE
    };

    Animator(CMesh* mesh = 0, MS3DModel* data = 0);
    virtual ~Animator(void);

    virtual void runTask();

    void animate(float time);
    void addAnimation(MS3DAnimation* anim, ANIMMODE mode = REPEATING, float starttime = 0.0f, float fps = 24.0f);
    void removeAnimation(MS3DAnimation* anim);
    void removeAllAnimations();

    void setMS3DModel(MS3DModel* model);
    void setMesh(CMesh* mesh);

protected:
    struct ANIMINFO
    {
        MS3DAnimation::SMARTPTR animation;
        float                   starttime;
        float                   fps;
        ANIMMODE                mode;

        ANIMINFO(MS3DAnimation* a = 0, float s = 0.0f, ANIMMODE m = REPEATING, float f = 24.0f)
        : animation(a)
        , starttime(s)
        , fps(f)
        , mode(m) {};
    };
    struct _predicate
    {
        _predicate(MS3DAnimation* anim)
        : m_anim(anim) {};
        bool           operator()(const ANIMINFO& animinfo) const { return animinfo.animation.getPointer() == m_anim; }
        MS3DAnimation* m_anim;
    };

    CMesh::SMARTPTR     m_mesh;
    MS3DModel::SMARTPTR m_model;

    std::list<ANIMINFO> m_animations;
};
