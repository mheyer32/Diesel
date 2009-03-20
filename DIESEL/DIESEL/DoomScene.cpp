#include "doomscene.h"
#include "Engine.h"
#include "TNLStack.h"

DoomScene::DoomScene(void)
{
}

DoomScene::~DoomScene(void)
{
	delete map;
}

void DoomScene::cullScene()
{
	map->cullMap();
}

void DoomScene::renderScene()
{
	g_TNLStack->execute();
	g_TNLStack->clearStack();
}

bool DoomScene::initialize()
{
	return true;
}

void DoomScene::addEntity(CEntity *newEntity)
{
}

void DoomScene::addEntities(CEntity::ENTITYLIST &list)
{

}