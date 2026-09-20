#pragma once

#include "BaseScene.h"
#include "DoomMap.h"

class DoomScene : public CBaseScene
{
public:
    friend class DoomLoader;

    DoomScene(void);
    ~DoomScene(void);

    virtual void cullScene();
    virtual void renderScene();
    virtual bool initialize();
    virtual void addEntity(CEntity* newEntity);
    virtual void addEntities(CEntity::ENTITYLIST& list);

protected:
    DoomMap* map;
};
