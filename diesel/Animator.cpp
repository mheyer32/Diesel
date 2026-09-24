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
#include "Animator.h"
#include <cassert>

Animator::Animator(CMesh* mesh, MS3DModel* model)
{
    if (model)
        setMS3DModel(model);
    if (mesh)
        setMesh(mesh);
}

Animator::~Animator(void)
{
}

void Animator::setMS3DModel(MS3DModel* model)
{
    assert(model);
    m_model = model;
}

void Animator::setMesh(CMesh* mesh)
{
    assert(mesh);
    m_mesh = mesh;
}

void Animator::removeAnimation(MS3DAnimation* anim)
{
    m_animations.remove_if(_predicate(anim));
}

void Animator::removeAllAnimations()
{
    m_animations.clear();
}

void Animator::animate(float time)
{
    assert(m_model && m_mesh);

    MS3DModel::FRAMEINFOS frameinfos;

    std::list<ANIMINFO>::iterator anIt = m_animations.begin();

    for (; anIt != m_animations.end();) {
        const ANIMINFO& animinfo = *anIt;

        float animframe = time - animinfo.starttime;
        animframe *= animinfo.fps;

        MS3DModel::FRAMEINFO frameinfo;
        frameinfo.anim = animinfo.animation;

        if (animframe < 0.0f) {
            ++anIt;  // do not start animation yet
            continue;
        }

        if (animinfo.mode == ONCE) {
            if (animframe > animinfo.animation->getNumFrames()) {
                // animation is over, remove it from list
                anIt = m_animations.erase(anIt);
                continue;
            } else {
                frameinfo.time = animframe;
            }
        } else {
            frameinfo.time = fmodf(animframe, animinfo.animation->getNumFrames());
        }

        frameinfos.push_back(frameinfo);

        ++anIt;
    }

    m_model->evaluateRuntimePosture(m_mesh, frameinfos);
}

void Animator::runTask()
{
    float time = CKernel::Instance()->getTime();
    animate(time);
    setNextThink(CKernel::Instance()->getTime() + 1.0f / 50.0f);
}

void Animator::addAnimation(MS3DAnimation* anim, ANIMMODE mode, float starttime, float fps)
{
    assert(anim);
    m_animations.push_back(ANIMINFO(anim, starttime, mode, fps));
}