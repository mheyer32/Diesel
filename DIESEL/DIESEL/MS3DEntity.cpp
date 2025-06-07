#include ".\ms3dentity.h"
#include "Kernel.h"

MS3DEntity::MS3DEntity(MS3DModel* model)
{
    if (model) {
        setMS3DModel(model);
        setName(m_ms3dmodel->getName());
    }
}

MS3DEntity::~MS3DEntity(void)
{
}

void MS3DEntity::setMS3DModel(MS3DModel* model)
{
    CKernel::Instance()->removeTask(&m_animator);

    m_ms3dmodel = model;
    m_meshes.clear();
    m_meshes.push_back(m_ms3dmodel->createMesh());

    m_animator.removeAllAnimations();
    m_animator.setMesh(m_meshes[0]);
    m_animator.setMS3DModel(m_ms3dmodel);

    CKernel::Instance()->addTask(&m_animator);
}
