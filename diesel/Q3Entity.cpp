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

#include "Q3Entity.h"

#include "Engine.h"
#include "TNLStack.h"
#include "Entity.h"
#include "Q3BSPMesh.h"
#include "Q3Scene.h"

#include <math/MathLib.h>

#include <gl/gl.h>

#include <MemoryTracker.h>

int                    Q3Entity::cull_front     = 0;
Q3Entity*              Q3Entity::current_portal = NULL;
Q3Entity::Q3ENTITYLIST Q3Entity::q3entities;
Q3Entity::Q3ENTITYLIST Q3Entity::seen_portals;

using namespace std;

Q3Entity::Q3Entity()
{
    type      = ET_GENERAL;
    classname = "classless";

    worldmodel = NULL;

    spawnflags = 0;
    speed      = 0;

    wait   = 0;
    random = 0;

    count  = 0;
    health = 0;

    dmg = 0;

    angles[0] = angles[1] = angles[2] = 0;

    origin = VECTOR3(0, 0, 0);
    m_transform.setIdentity();

    q3entities.push_back(this);
}

Q3Entity::~Q3Entity()
{
    q3entities.remove(this);

    if (worldmodel)
        worldmodel->unlinkEntity(this);
}

Q3Entity* Q3Entity::findEntity(const std::string& entityname)
{
    Q3ENTITYITERATOR eIt = q3entities.begin();

    for (; eIt != q3entities.end(); ++eIt) {
        if ((*eIt)->getName() == entityname)
            return *eIt;
    }
    return NULL;
}

void Q3Entity::setModel(CEntity* newModel)
{
    model = newModel;
    addChild(model);
}

void Q3Entity::setTarget(Q3Entity* newTarget)
{
    target = newTarget;
}

void Q3Entity::setWorldModel(Q3BSPMesh* model)
{
    worldmodel = model;
    if (getNumChildren())  // don´t link if this entity has no visible models
    {
        worldmodel->linkEntity(this);
    }
}

void Q3Entity::cullEntity(int testplanes)
{
    //	cout<<"Q3Entity::cullEntity()"<<endl;
    switch (type) {
    case ET_ITEM:
        updateItem();  // update Items only if needed
        break;
    case ET_PORTAL:
        // portals may not render themselves again
        if (seen_portals.size() < MAX_NUM_PORTALS && current_portal != this) {
            // portals are rendered to their own stack afterwards to not disturb current bsp-traversion
            seen_portals.push_back(this);
        }
        break;
    }

    if (fog == -1) {
        CEntity::cullEntity(testplanes);
    } else {
        RENDERINFO fogoverride;
        fogoverride.num_features     = 1;
        fogoverride.features[0].name = FEATURE_FOG;
        fogoverride.features[0].data = (void*)const_cast<FOGFEATURE*>(&worldmodel->getFog(fog));
        g_TNLStack->setOverrideRenderInfo(fogoverride);
        CEntity::cullEntity(testplanes);
        g_TNLStack->clearOverrideRenderInfo();
    }
}

void Q3Entity::runTask()
{
    switch (type) {
    case ET_FUNCBOBBING:
        updateFuncBobbing();
        break;
    case ET_FUNCROTATING:
        updateFuncRotating();
        break;
    case ET_FUNCPENDULUM:
        updateFuncPendulum();
        break;
    case ET_FUNCTRAIN:
        updateFuncTrain();
        break;
    }
    setNextThink(CKernel::Instance()->getTime());  // force to run every frame;
}

void Q3Entity::RenderPortal(PORTALINFO& portalinfo)
{
    // cout<<"RenderPortal(): "<<endl;

    assert(worldmodel);

    // count saves the faceindex of the portalsurface;
    if (count == -1) {
        //	cout<<"RenderPortal() not a valid faceindex for portalface"<<endl;
        return;
    }

    const Q3BSPMesh::BSPFACE& face = worldmodel->getFace(count);

    Transformation& camtrafo = g_Camera->getTransformation();

    PLANE portalClipPlane(face.vbuffer->vertices[0][face.vbuffer->indices[0]],
                          face.vbuffer->vertices[0][face.vbuffer->indices[1]],
                          face.vbuffer->vertices[0][face.vbuffer->indices[2]]);

    float portalSurfDist = PointDistance(camtrafo.getTranslation(), portalClipPlane);
    // dont draw if behind
    if (portalSurfDist <= 0.0f)
        return;

    float testcos = cosf(RAD(90.0f - g_Camera->FOV * 0.5f));
    // dont draw if facing away
    if ((portalClipPlane.n * camtrafo.getColumnVector(2)) < -testcos)
        return;

    // save old culling state
    int old_cull_front = cull_front;
    int old_lockpvs    = (int)Q3BSPMesh::r_lockpvs;

    MATRIX4 portalCamMat;
    if (!target) {

        // mirror the current camera-system
        (VECTOR3&)portalCamMat.a[3] = mirrorPoint(portalClipPlane, camtrafo.getColumnVector(3));
        (VECTOR3&)portalCamMat.a[0] = mirrorDirection(portalClipPlane, camtrafo.getColumnVector(0));
        (VECTOR3&)portalCamMat.a[1] = mirrorDirection(portalClipPlane, camtrafo.getColumnVector(1));
        (VECTOR3&)portalCamMat.a[2] = mirrorDirection(portalClipPlane, camtrafo.getColumnVector(2));

        // imagine two mirrors that can see each other, the first one mirrors the camera-matrix, front-face culling is
        // needed the second mirror mirrors the mirrored camera again, backface culling is needed
        cull_front ^= 1;

        // bad hack, but for mirrors, the eyecluster must be set manually
        // because the camerapoint for mirrors mostly lies outside the world
        Q3BSPMesh::r_lockpvs = 1;

    } else {
        // target is the point where the camera is placed should be a misc_portal_camera, can also be a func_train
        if (portalSurfDist > 256.0f)
            return;  // Portals with cameras usually have an alphagen portal that makes them oqaque at distance

        const MATRIX4& currentCamMat = camtrafo.getMatrix();

        MATRIX4 portalSurface;
        ConstructMatrix4Z(portalClipPlane.n, portalSurface);
        (VECTOR3&)portalSurface.a[3] = ProjectPointOnPlane(this->origin, portalClipPlane);
        SimpleInverse4(portalSurface);

        portalSurface = portalSurface * currentCamMat;  // portalSurface now expresses the difference transformation
                                                        // between the portal surface-system and the camera-system

        // func_trains will interpolate their position, so use the xform instead of the origin
        MATRIX4        portalCampos;
        const VECTOR3& targetcampos = target->m_transform.getTranslation();
        TranslationMatrix4(portalCampos, targetcampos);

        MATRIX4 portalCamLookat;

        if (!target->target) {
            // the camera doesn´t point at a special entity
            // I never checked this code, so it may be wrong
            /*	RotationMatrix4Y(R,RAD(target->angles[1]));
                Rotate4X(R,RAD(target->angles[0]));
                Rotate4Z(R,RAD(target->angles[2]));*/

            YawPitchRollMatrix4(VECTOR3(RAD(target->angles[0]), RAD(target->angles[1]), RAD(target->angles[2])),
                                portalCamLookat);

            portalClipPlane = PLANE(-(VECTOR3&)portalCamLookat.a[2], targetcampos);
        } else if (model)  // rocket arena encodes moving look-at-targets this way (a portal-entity with associated
                           // model)
        {
            VECTOR3 dir     = model->getTransformation().getTranslation() - targetcampos;
            portalClipPlane = PLANE(dir, targetcampos);
            ConstructMatrix4Z(-dir, portalCamLookat);  // zaxis points out of screen

        } else {
            // the camera is supposed to look at target->target
            VECTOR3 dir     = target->target->m_transform.getTranslation() - targetcampos;
            portalClipPlane = PLANE(dir, targetcampos);
            ConstructMatrix4Z(-dir, portalCamLookat);  // zaxis points out of screen
        }

        float t = g_Time * target->speed / 360.0f;  // speed is roll in deg/s
        t       = t - floorf(t);
        Rotate4Z(portalCamLookat, RAD(15) * sinf(t * 2 * PI));

        portalCamMat = portalCampos * portalCamLookat * portalSurface;

        cull_front = 0;

        // HACK HACK HACK
        worldmodel->eyecluster  = worldmodel->getCluster(targetcampos);
        worldmodel->visdata_eye = worldmodel->visdata + worldmodel->eyecluster * worldmodel->ints_per_cluster;
        worldmodel->markLeaves();
        Q3BSPMesh::r_lockpvs = 1;
    }

    portalinfo.cullfront   = cull_front;
    portalinfo.portalplane = portalClipPlane;

    g_Camera->projectBBox(getBoundingBox(), portalinfo.portalrect);

    // setup new camera
    g_Camera->getTransformation().setMatrix(portalCamMat);
    g_Camera->setupCamera();
    g_Camera->getClipPlane(CLIP_NEAR) = portalClipPlane;

    g_TNLStack->setCamera(*g_Camera);

    // advance FrameCounter so that marked entities, leafs and so on can be rendered again
    g_FrameCounter++;

    Q3Entity* old_portal = current_portal;
    current_portal       = this;  // this prevents the portal from visiting itself directly

    worldmodel->Render();

    current_portal = old_portal;

    // restore old culling-state
    cull_front = old_cull_front;

    Q3BSPMesh::r_lockpvs = old_lockpvs;
}

void Q3Entity::updateFuncBobbing()
{
    float t = g_Time;
    t       = t / speed + angles[1];
    t -= floorf(t);
    t = sinf(2.0f * PI * t) * angles[2];
    switch (spawnflags) {
    case 1:
        m_transform.setTranslation(origin + VECTOR3(-t, 0, 0));
        break;
    case 2:
        m_transform.setTranslation(origin + VECTOR3(0, 0, t));
        break;
    default:
        m_transform.setTranslation(origin + VECTOR3(0, t, 0));
        break;
    }

    worldmodel->linkEntity(this);
}

void Q3Entity::updateFuncRotating()
{
    float t = g_Time;
    t       = t * RAD(speed);
    t += RAD(angles[1]);

    if (spawnflags & 4) {
        m_transform.setAngles(VECTOR3(-t, 0, 0));
    } else if (spawnflags & 8) {
        m_transform.setAngles(VECTOR3(0, 0, t));
    } else {
        m_transform.setAngles(VECTOR3(0, t, 0));
    }
}

void Q3Entity::updateFuncPendulum()
{
    float t = g_Time * 0.15f;
    t       = t - floorf(t) + wait;
    m_transform.setAngles(VECTOR3(RAD(sinf(2.0f * PI * t) * speed), RAD(angles[1]), 0));

    // translation remains set
}

void Q3Entity::updateFuncTrain()
{
    if (g_Time >= wait)  // arrived at next corner
    {
        if (!target)
            return;
        origin = target->origin;
        target = target->target;
        if (target) {
            if (target->speed > 0) {
                speed = target->speed;
            } else {
                speed = angles[0];
            }
            float dist = Distance(origin, target->origin);
            wait       = g_Time + dist / speed;  // time of arrival at new pathcorner
            random     = g_Time;                 // starttime;
        }
    }
    VECTOR3 dist = NullVector3;
    if (target) {
        dist = target->origin - origin;
        Normalize(dist);
    }
    VECTOR3 translation = origin + dist * (g_Time - random) * speed;
    m_transform.setTranslation(translation);

    worldmodel->linkEntity(this);  // relink this entity, because it may have moved out of the current leaves
}

#define DEFBOBHEIGHT 7.0f
#define DEFBOBSPEED 1.0f
#define DEFROT 180.0f
#define DEFSCALE 1.0f

void Q3Entity::updateItem()
{
    float angle;
    if (itemtype == IT_HEALTH) {
        angle = g_Time + angles[0];
    } else {
        angle = (g_Time + angles[0]) * 0.5f;  // weapons and normal items rotate slower
    }

    angle -= floorf(angle);
    // the first model of multipart-items should always rotate
    getChild(0)->getTransformation().setAngles(VECTOR3(0, angle * 2.0f * PI, 0));

    // rotate outer rings
    if (itemtype == IT_POWERUP && getChild(1)) {
        angle = g_Time + angles[0];
        angle -= floorf(angle);
        getChild(1)->getTransformation().setAngles(VECTOR3(0, -angle * 2.0f * PI, 0));
    }

    float t = (g_Time + angles[0]) / DEFBOBSPEED;
    t -= floorf(t);
    VECTOR3 pos = origin;

    if (itemtype == IT_WEAPON) {
        m_transform.setScale(1.5f);
        pos.y += 8.0f;
    }
    pos.y += 4 + cosf(2.0f * PI * t) * 4;
    m_transform.setTranslation(pos);
}

void Q3Entity::setPortalFace()
{
    assert(worldmodel);

    int                 leafnum;
    int                 facenum;
    Q3BSPMesh::BSPFACE* face;
    Q3BSPMesh::BSPLEAF* leaf;

    count = -1;

    // get the leafs the portalentity touches
    BBOX bbox(origin - VECTOR3(64, 64, 64), origin + VECTOR3(64, 64, 64));
    worldmodel->getTouchedLeafs(bbox, leafs);

    Q3BSPMesh::INDEXITERATOR lIt = leafs.begin();

    resetBBox(bbox);
    // now examine each face of each leaf and look if it´s a portal-surface
    // in rare cases this is still the wrong method
    // cout<<"leafs:";
    for (; lIt != leafs.end(); ++lIt) {
        leafnum = *lIt;
        // cout<<leafnum<<" ";
        leaf = &(worldmodel->getLeaf(leafnum));

        leaf->entities.push_back(this);

        CShader* shader;
        for (int f = 0; f < leaf->num_faces; ++f) {
            facenum = worldmodel->getFaceNum(leafnum, f);
            face    = &worldmodel->getFace(facenum);

            shader = face->vbuffer->getShader();
            if (shader->getFlags() & SHADER_PORTAL) {
                // remember the face-index
                count = facenum;

                CVertexBuffer* vb = worldmodel->getFace(count).vbuffer;
                for (int v = 0; v < vb->num_vertices; v++) {
                    bbox |= vb->vertices[0][v];
                }
                VECTOR3& vec = vb->vertices[0][0];
                bbox |= BBOX(vec + VECTOR3(-5, -5, -5),
                             vec + VECTOR3(5, 5, 5));  // ensure that the bbox has some size in all directions
            }
        }
    }
    if (count == -1) {
        std::cout << "could not find portal surface near portal entity" << std::endl;
        return;
    }

    bbox -= origin;
    // the entity must be linked into the worldmodel properly, so give it a
    // pseudo-mesh that contains nothing but a bbox
    // this way the entity has "size", though no "body"
    CMesh* pseudomesh = new CMesh;
    pseudomesh->setName("portal_surface_mesh");
    pseudomesh->setBoundingBox(bbox);
    addMesh(pseudomesh);

    worldmodel->linkEntity(this);  // need to relink the entity, because its size has changed
}

void Q3Entity::assignEntity(CEntity* entity)
{
    // FIXME why the hell did I do it this way?
    classname = "classless";
    type      = ET_GENERAL;

    m_transform = entity->getTransformation();
    entity->getTransformation().setIdentity();

    setModel(entity);
}
