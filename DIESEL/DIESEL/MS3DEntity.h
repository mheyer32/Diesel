#pragma once
#include "Animator.h"
#include "entity.h"

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
