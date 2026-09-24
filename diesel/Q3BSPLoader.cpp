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

#include "Q3BSPLoader.h"

#include <cassert>
#include <iostream>

#include <file/FileManager.h>
#include <math/MathLib.h>
#include <math/Vector4.h>

#include "Q3BSPMesh.h"
#include "Q3EntityFactory.h"
#include "QuadricBezierPatch.h"
#include "Shader.h"
#include "Sound.h"
#include "Texture.h"
#include "VBServices.h"

#include <MemoryTracker.h>

#include <crtdbg.h>

ConVar Q3BSPLoader::r_curvefactor("r_curvefactor", "4.0");
ConVar Q3BSPLoader::r_mapOverBrightBits("r_mapOverBrightBits", "2");

using namespace std;

inline VECTOR3 QVector3(const float qv[3])
{
    return VECTOR3(qv[0], qv[2], -qv[1]);
}
inline VECTOR2 QVector2(const float qv[2])
{
    return VECTOR2((float*)&qv);
}
inline BBOX QBBox(const int mins[3], const int maxs[3])
{
    BBOX bbox;
    bbox.min.x = (float)mins[0];
    bbox.min.y = (float)mins[2];
    bbox.min.z = -(float)maxs[1];
    bbox.max.x = (float)maxs[0];
    bbox.max.y = (float)maxs[2];
    bbox.max.z = -(float)mins[1];
    return bbox;
}
inline BBOX QBBox(const float mins[3], const float maxs[3])
{
    BBOX bbox;
    bbox.min.x = mins[0];
    bbox.min.y = mins[2];
    bbox.min.z = -maxs[1];
    bbox.max.x = maxs[0];
    bbox.max.y = maxs[2];
    bbox.max.z = -mins[1];
    return bbox;
}

Q3BSPLoader::Q3BSPLoader()
{
    file = NULL;

    models    = NULL;
    shaders   = NULL;
    lightmaps = NULL;

    worldmodel = NULL;
    scene      = NULL;

    vbuffer = new CVertexBuffer();
    vbuffer->AddRef();
}

Q3BSPLoader::~Q3BSPLoader()
{
    if (file)
        file->close();

    KILLOBJECT(file);
    KILLARRAY(models)
    KILLARRAY(lightmaps)
    KILLARRAY(shaders);

    if (vbuffer)
        vbuffer->Release();
}

Q3Scene* Q3BSPLoader::LoadQ3BSP(std::string Filename)
{
    CFileManager* fman = CFileManager::Instance();

    if (!(file = fman->open(Filename.c_str()))) {
        std::cout << "could not open :\"" << Filename << "\"" << std::endl;
        return NULL;
    }

    std::cout << endl << "...loading map" << std::endl;
    std::cout << "-------------------------------------------\n\n" << std::endl;
    file->readVOID(&header, sizeof(Q3HEADER));

    if (header.version != 46) {
        std::cout << "WARNING: Map version differs from 46: " << header.version << std::endl;
    }

    scene             = new Q3Scene();
    worldmodel        = new Q3BSPMesh();
    scene->worldmodel = worldmodel;

    ReadShaders();
    assert(_CrtCheckMemory());
    //	cout<<"shaders..."<<endl;
    ReadLightmaps();
    assert(_CrtCheckMemory());
    //	cout<<"lightmaps..."<<endl;
    ReadPlanes();
    assert(_CrtCheckMemory());
    //	cout<<"planes..."<<endl;
    ReadVertices();
    assert(_CrtCheckMemory());
    //	cout<<"vertices..."<<endl;
    ReadIndexes();
    //	cout<<"indexes..."<<endl;
    ReadSurfaces();
    //	cout<<"surfaces..."<<endl;
    ReadLeafSurfaces();
    //	cout<<"leafsurfaces..."<<endl;
    ReadNodesAndLeafs();
    //	cout<<"nodes and leafs..."<<endl;
    ReadVisdata();
    //	cout<<"visdata..."<<endl;
    ReadBrushSides();
    //	cout<<"brushsides..."<<endl;
    ReadBrushes();
    //	cout<<"brushes..."<<endl;
    ReadLeafBrushes();
    //	cout<<"leafbrushes..."<<endl;
    ReadEffects();
    //	cout<<"fogs..."<<endl;
    ReadModels();
    //	cout<<"models..."<<endl;
    worldmodel->buildAreas();
    ReadEntities();
    //	cout<<"entities..."<<endl;
    ReadLightgrid();
    //	cout<<"lightgrid..."<<endl;

    file->close();
    KILLOBJECT(file);

    //	cout<<"bsp loaded..."<<endl;
    return scene;
}

void Q3BSPLoader::ReadShaders()
{
    file->setPosition(header.lumps[LUMP_SHADERS].offset);

    worldmodel->num_surfaceinfos = header.lumps[LUMP_SHADERS].length / sizeof(Q3SHADER);
    worldmodel->surfaceinfos     = new Q3BSPMesh::SURFACEINFO[worldmodel->num_surfaceinfos];

    shaders = new Q3SHADER[worldmodel->num_surfaceinfos];

    file->readVOID(shaders, header.lumps[LUMP_SHADERS].length);

    for (int s = 0; s < worldmodel->num_surfaceinfos; ++s) {
        worldmodel->surfaceinfos[s].shader    = NULL;  // Later when the faces are read in, this field is filled
        worldmodel->surfaceinfos[s].contents  = shaders[s].contents;
        worldmodel->surfaceinfos[s].surfflags = shaders[s].surfflags;
    }
}

void Q3BSPLoader::ReadPlanes()
{
    file->setPosition(header.lumps[LUMP_PLANES].offset);

    worldmodel->num_planes = header.lumps[LUMP_PLANES].length / sizeof(Q3PLANE);
    worldmodel->planes     = new PLANE[worldmodel->num_planes];

    Q3PLANE* q3planes = new Q3PLANE[worldmodel->num_planes];

    file->readVOID(q3planes, header.lumps[LUMP_PLANES].length);

    for (int p = 0; p < worldmodel->num_planes; p++) {
        worldmodel->planes[p] = PLANE(QVector3(q3planes[p].n), q3planes[p].d);
    }

    KILLARRAY(q3planes)
}

void Q3BSPLoader::ReadLightmaps()
{
    file->setPosition(header.lumps[LUMP_LIGHTMAPS].offset);
    int num_lightmaps = header.lumps[LUMP_LIGHTMAPS].length / LIGHTMAPSIZE;

    if (num_lightmaps == 0) {
        lightmaps = NULL;
        std::cout << "WARNING: bsp contains no lightmaps" << std::endl;
        return;
    }

    unsigned char* lmdata = new unsigned char[LIGHTMAPSIZE * num_lightmaps];
    lightmaps             = new CTexture*[num_lightmaps];

    file->readVOID(lmdata, header.lumps[LUMP_LIGHTMAPS].length);

    ConVar* r_OverBrightBits = ConVar::findCVar("r_overBrightBits");

    int intensity = 0;
    if (r_OverBrightBits) {
        intensity = (int)r_mapOverBrightBits - (int)*r_OverBrightBits;
    }

    unsigned int r, g, b, top;

    if (intensity > 0) {
        for (int p = 0; p < num_lightmaps * LIGHTMAPSIZE; p += 3) {
            top = r = lmdata[p] << intensity;
            g       = lmdata[p + 1] << intensity;
            b       = lmdata[p + 2] << intensity;
            if (g > top)
                top = g;
            if (b > top)
                top = b;

            if (top > 255) {
                top = (255 << 8) / top;
                r   = (r * top) >> 8;
                g   = (g * top) >> 8;
                b   = (b * top) >> 8;
            }
            lmdata[p]     = r;
            lmdata[p + 1] = g;
            lmdata[p + 2] = b;
        }
    }

    for (int l = 0; l < num_lightmaps; l++) {
        Image* lightmap = new Image(LIGHTMAP_WIDTH, LIGHTMAP_HEIGHT, 3, GL_RGB8, GL_RGB, lmdata + l * LIGHTMAPSIZE);
        lightmaps[l]    = new CTexture;
        lightmaps[l]->setImage(lightmap, TEX_NOPICMIP | TEX_NOMIPMAP | TEX_CLAMP | TEX_DONTCOMPRESS);
        lightmaps[l]->setName("lightmap");  // FIXME: vielleicht Nummerierung durchfuehren
        delete lightmap;
    }

    KILLARRAY(lmdata);
}

void Q3BSPLoader::ReadVertices()
{
    file->setPosition(header.lumps[LUMP_DRAWVERTS].offset);
    int num_vertices = header.lumps[LUMP_DRAWVERTS].length / sizeof(Q3VERTEX);

    vbuffer->AllocArrays(MT_SYSMEM, VERTEXARRAY | NORMALARRAY | TEXCOORDARRAY | COLORARRAY | LMCOORDARRAY,
                         num_vertices);

    Q3VERTEX* q3vertices = new Q3VERTEX[num_vertices];

    file->readVOID(q3vertices, num_vertices * sizeof(Q3VERTEX));

    ConVar* ob = ConVar::findCVar("r_overBrightBits");

    int overbright = 0;
    if (ob) {
        overbright = (int)r_mapOverBrightBits - (int)*ob;
    }

    for (int v = 0; v < num_vertices; ++v) {

        vbuffer->vertices[0][v] = QVector3(q3vertices[v].position);
        vbuffer->normals[0][v]  = QVector3(q3vertices[v].normal);
        vbuffer->texcoords[v]   = (VECTOR2&)q3vertices[v].texcoords[0];
        vbuffer->lmcoords[v]    = (VECTOR2&)q3vertices[v].texcoords[1];
    }

    unsigned int r, g, b, top;
    if (overbright > 0) {
        for (int v = 0; v < num_vertices; v++) {
            top = r = q3vertices[v].color[0] << overbright;
            g       = q3vertices[v].color[1] << overbright;
            b       = q3vertices[v].color[2] << overbright;
            if (g > top)
                top = g;
            if (b > top)
                top = b;
            if (top > 255) {
                top = (255 << 8) / top;
                r   = (r * top) >> 8;
                g   = (g * top) >> 8;
                b   = (b * top) >> 8;
            }
            vbuffer->colors[v].r     = r;
            vbuffer->colors[v].g     = g;
            vbuffer->colors[v].b     = b;
            vbuffer->colors[v].alpha = q3vertices[v].color[3];
        }
    } else {
        for (int v = 0; v < num_vertices; v++) {
            vbuffer->colors[v] = COLOR(q3vertices[v].color);
        }
    }

    KILLARRAY(q3vertices);
}

void Q3BSPLoader::ReadSurfaces()
{
    file->setPosition(header.lumps[LUMP_SURFACES].offset);
    worldmodel->num_faces = header.lumps[LUMP_SURFACES].length / sizeof(Q3SURFACE);

    Q3SURFACE* q3faces = new Q3SURFACE[worldmodel->num_faces];
    worldmodel->faces  = new Q3BSPMesh::BSPFACE[worldmodel->num_faces];

    file->readVOID(q3faces, header.lumps[LUMP_SURFACES].length);

    for (int f = 0; f < worldmodel->num_faces; f++) {
        worldmodel->faces[f].normal = QVector3(q3faces[f].normal);
        worldmodel->faces[f].origin = QVector3(q3faces[f].lightmap_origin);
        worldmodel->faces[f].tang_s = QVector3(q3faces[f].lightmap_vecs[0]);
        worldmodel->faces[f].tang_t = QVector3(q3faces[f].lightmap_vecs[1]);
        worldmodel->faces[f].type   = q3faces[f].type;
        worldmodel->faces[f].fognum = q3faces[f].fognum;

        bool genlightmap;
        int  shadernum = q3faces[f].shadernum;

        Q3BSPMesh::SURFACEINFO& surface = worldmodel->surfaceinfos[shadernum];

        // has shader already been loaded ?
        if (!(surface.shader)) {
            // if the face has a valid lightmapnum, the shader to be generated must
            // have a lightmap-pass
            genlightmap = q3faces[f].lightmapnum >= 0;

            // try to find the shader, or generate it
            assert(shaders[shadernum].shader);
            surface.shader = CShader::findOrLoadShader(shaders[shadernum].shader, genlightmap);

            if (surface.shader) {
                // if the shader had to be generated from a texture and has no lightmap pass
                // then use rgbgen_vertex

                if ((surface.shader->getFlags() & SHADER_TAGGED) && !genlightmap) {
                    surface.shader->getPass(0).setRGBGen(RGBGEN_VERTEX);
                    surface.shader->validate();
                }
            } else {
                cout << "WARNING: shader '" << shaders[shadernum].shader
                     << "' could not be found or generated, using default shader" << endl;
                surface.shader = CShader::DefaultShader;
            }
        }

        // create the new face
        CVertexBuffer* facebuffer = worldmodel->faces[f].vbuffer = new CVertexBuffer;
        facebuffer->setShader(surface.shader);

        if ((lightmaps != NULL) && (q3faces[f].lightmapnum >= 0)) {
            facebuffer->setLightmap(lightmaps[q3faces[f].lightmapnum]);
        }

        switch (q3faces[f].type) {
        case MST_PATCH: {
            evaluatePatch(q3faces[f], worldmodel->faces[f]);
            VBServices::reverseIndices(*facebuffer);
            break;
        }
        case MST_FLARE:
            generateFlare(q3faces[f], *facebuffer);
            break;

        case MST_PLANAR:
        case MST_TRIANGLE_SOUP:

            DWORD Arrays       = facebuffer->getShader()->getUsedArrays();
            int   num_vertices = q3faces[f].num_vertices;
            int   firstvertex  = q3faces[f].firstvertex;

            facebuffer->AllocArrays(Arrays & ~INDEXARRAY, num_vertices);

            facebuffer->setVertices(vbuffer->vertices[0] + firstvertex, num_vertices);
            if (Arrays & TEXCOORDARRAY) {
                facebuffer->setTexCoords(vbuffer->texcoords + firstvertex, num_vertices);
            }
            if (Arrays & COLORARRAY) {
                facebuffer->setColors(vbuffer->colors + firstvertex, num_vertices);
            }
            if (Arrays & LMCOORDARRAY) {
                facebuffer->setLMCoords(vbuffer->lmcoords + firstvertex, num_vertices);
            }
            if (Arrays & NORMALARRAY) {
                facebuffer->setNormals(vbuffer->normals[0] + firstvertex, num_vertices);
            }

            if (q3faces[f].num_indexes && (Arrays & INDEXARRAY)) {
                facebuffer->AllocArrays(INDEXARRAY, 0, q3faces[f].num_indexes);
                facebuffer->setIndices(vbuffer->indices + q3faces[f].firstindex, q3faces[f].num_indexes);
                VBServices::reverseIndices(*facebuffer);
            }
            //		facebuffer->OptimizeVBuffer();
            // FIXME: vorher auf SHADERFLAG_DEFORMV prüfen
            CShader* shader = facebuffer->getShader();
            for (int d = 0; d < shader->getNumVertexDeform(); ++d) {
                if (shader->getVertexDeform(d) == VDEF_AUTOSPRITE) {
                    VBServices::preTransformBillboard(*facebuffer);
                    break;
                } else if (shader->getVertexDeform(d) == VDEF_AUTOSPRITE2) {
                    VBServices::preTransformAxisBillboard(*facebuffer);
                    break;
                }
            }
        }
    }

    KILLARRAY(q3faces)
}

void Q3BSPLoader::ReadIndexes()
{
    file->setPosition(header.lumps[LUMP_DRAWINDEXES].offset);
    int num_meshverts = header.lumps[LUMP_DRAWINDEXES].length / sizeof(int);

    vbuffer->AllocArrays(INDEXARRAY, 0, num_meshverts);

    int* q3indices = new int[num_meshverts];
    file->readVOID(q3indices, header.lumps[LUMP_DRAWINDEXES].length);

    for (int i = 0; i < num_meshverts; ++i)
        vbuffer->indices[i] = q3indices[i];

    KILLARRAY(q3indices)
}
void Q3BSPLoader::ReadLeafSurfaces()
{
    file->setPosition(header.lumps[LUMP_LEAFSURFACES].offset);
    worldmodel->num_leaffaces = header.lumps[LUMP_LEAFSURFACES].length / sizeof(int);
    worldmodel->leaffaces     = new int[worldmodel->num_leaffaces];

    file->readVOID(worldmodel->leaffaces, header.lumps[LUMP_LEAFSURFACES].length);
}

void Q3BSPLoader::ReadNodesAndLeafs()
{

    worldmodel->num_nodes = header.lumps[LUMP_NODES].length / sizeof(Q3NODE);
    worldmodel->nodes     = new Q3BSPMesh::BSPNODE[worldmodel->num_nodes];

    Q3NODE* q3nodes = new Q3NODE[worldmodel->num_nodes];

    file->setPosition(header.lumps[LUMP_NODES].offset);
    file->readVOID(q3nodes, worldmodel->num_nodes * sizeof(Q3NODE));

    for (int n = 0; n < worldmodel->num_nodes; ++n) {
        worldmodel->nodes[n].plane = q3nodes[n].plane;
        worldmodel->nodes[n].front = q3nodes[n].children[0];
        worldmodel->nodes[n].back  = q3nodes[n].children[1];
        worldmodel->nodes[n].bbox  = QBBox(q3nodes[n].mins, q3nodes[n].maxs);
    }
    KILLARRAY(q3nodes)

    file->setPosition(header.lumps[LUMP_LEAFS].offset);

    worldmodel->num_leafs = header.lumps[LUMP_LEAFS].length / sizeof(Q3LEAF);
    worldmodel->leafs     = new Q3BSPMesh::BSPLEAF[worldmodel->num_leafs];

    Q3LEAF* q3leafs = new Q3LEAF[worldmodel->num_leafs];

    file->readVOID(q3leafs, worldmodel->num_leafs * sizeof(Q3LEAF));

    for (int l = 0; l < worldmodel->num_leafs; ++l) {
        worldmodel->leafs[l].bbox            = QBBox(q3leafs[l].mins, q3leafs[l].maxs);
        worldmodel->leafs[l].cluster         = q3leafs[l].cluster;
        worldmodel->leafs[l].area            = q3leafs[l].area;
        worldmodel->leafs[l].startface       = q3leafs[l].firstleafsurface;
        worldmodel->leafs[l].num_faces       = q3leafs[l].num_leafsurfaces;
        worldmodel->leafs[l].startleafbrush  = q3leafs[l].firstleafbrush;
        worldmodel->leafs[l].num_leafbrushes = q3leafs[l].num_leafbrushes;
    }

    KILLARRAY(q3leafs)
}

void Q3BSPLoader::ReadModels()
{

    file->setPosition(header.lumps[LUMP_MODELS].offset);
    int num_models = header.lumps[LUMP_MODELS].length / sizeof(Q3MODEL);

    Q3MODEL* models = new Q3MODEL[num_models];
    file->readVOID(models, num_models * sizeof(Q3MODEL));

    worldmodel->num_models = num_models;
    worldmodel->models     = new Q3BSPMesh::BSPMODEL[num_models];
    for (int m = 0; m < num_models; ++m) {
        worldmodel->models[m].firstbrush  = models[m].firstbrush;
        worldmodel->models[m].num_brushes = models[m].num_brushes;
        worldmodel->models[m].bbox        = QBBox(models[m].mins, models[m].maxs);
    }

    worldmodel->bbox = QBBox(models[0].mins, models[0].maxs);

    char name[64] = "*";  // solid models have name *x
    // für Kollisionserkennung braucht es eine meshklasse, die brushes unterstützt
    for (int m = 1; m < num_models; m++) {
        _itoa(m, name + 1, 10);

        CMesh* model = new CMesh;
        model->setName(name);

        meshlist.push_back(model);

        for (int f = 0; f < models[m].num_surfaces; f++) {
            int surface = models[m].firstsurface + f;
            if (worldmodel->faces[surface].vbuffer) {
                if (!worldmodel->faces[surface].vbuffer->num_vertices) {
                    std::cout << "WARNING: Q3BSPLoader::ReadModels() model with empty vbuffer!" << std::endl;
                    continue;
                }
                model->addVertexBuffer(worldmodel->faces[surface].vbuffer);
            }
        }
        model->setBoundingBox(QBBox(models[m].mins, models[m].maxs));
    }

    KILLARRAY(models)
}

void Q3BSPLoader::ReadEntities()
{
    file->setPosition(header.lumps[LUMP_ENTITIES].offset);
    int length = header.lumps[LUMP_ENTITIES].length;

    char* entdesc = new char[length + 1];
    file->readVOID(entdesc, length);
    entdesc[length] = 0;

    clearMapSounds();

    Q3EntityFactory factory;

    factory.processEntities(entdesc, scene);

    delete[] entdesc;
}

void Q3BSPLoader::ReadVisdata()
{
    file->setPosition(header.lumps[LUMP_VISIBILITY].offset);
    int length = header.lumps[LUMP_VISIBILITY].length;

    if (length == 0) {
        std::cout << "WARNING: bsp contains no PVS data" << std::endl;
        return;
    }

    worldmodel->num_clusters = (int)file->readDWORD();
    int clustersize          = (int)file->readDWORD();

    assert(!(clustersize & 3));

    // clustersize is always 4-byte aligned
    clustersize >>= 2;

    if (worldmodel->num_clusters < 65535)  // FIXME: sinnlose abfrage?
    {
        worldmodel->visdata = new unsigned int[worldmodel->num_clusters * clustersize];
        file->readVOID(worldmodel->visdata, worldmodel->num_clusters * clustersize * sizeof(unsigned int));
        worldmodel->ints_per_cluster = clustersize;
    }
}

void Q3BSPLoader::evaluatePatch(const Q3BSPLoader::Q3SURFACE& face, Q3BSPMesh::BSPFACE& bspface)
{
    int num_cpx = face.patchwidth;
    int num_cpy = face.patchheight;

    int patches_x = num_cpx / 2;
    int patches_y = num_cpy / 2;

    VECTOR3* cps   = &(vbuffer->vertices[0][face.firstvertex]);
    VECTOR3* norms = &(vbuffer->normals[0][face.firstvertex]);
    VECTOR2* texc  = &(vbuffer->texcoords[face.firstvertex]);
    VECTOR2* lmc   = &(vbuffer->lmcoords[face.firstvertex]);
    COLOR*   col   = &(vbuffer->colors[face.firstvertex]);

    int flatx = 0;
    int flaty = 0;

    for (int cp_y = 0; cp_y < num_cpy - 1; cp_y += 2) {
        for (int cp_x = 0; cp_x < num_cpx - 1; cp_x += 2) {
            // Krümmung der Fläche in s und t Richtung abschätzen
            int index;
            index = cp_y * num_cpx + cp_x;
            flatx = __max(flatx, Bezier::flatnessTest(r_curvefactor, cps[index], cps[index + 1], cps[index + 2]));
            flatx = __max(flatx, Bezier::flatnessTest(r_curvefactor, cps[index + num_cpx], cps[index + num_cpx + 1],
                                                      cps[index + num_cpx + 2]));
            flatx = __max(flatx, Bezier::flatnessTest(r_curvefactor, cps[index + 2 * num_cpx],
                                                      cps[index + 2 * num_cpx + 1], cps[index + 2 * num_cpx + 2]));

            flaty = __max(flaty, Bezier::flatnessTest(r_curvefactor, cps[index], cps[index + num_cpx],
                                                      cps[index + 2 * num_cpx]));
            flaty = __max(flaty, Bezier::flatnessTest(r_curvefactor, cps[index + 1], cps[index + num_cpx + 1],
                                                      cps[index + 2 * num_cpx + 1]));
            flaty = __max(flaty, Bezier::flatnessTest(r_curvefactor, cps[index + 2], cps[index + num_cpx + 2],
                                                      cps[index + 2 * num_cpx + 2]));
        }
    }

    int tess_y = (1 << flaty);
    int tess_x = (1 << flatx);

    int vb_verts_x = patches_x * tess_x + 1;
    int vb_verts_y = patches_y * tess_y + 1;

    bspface.patchdim[0] = vb_verts_x;
    bspface.patchdim[1] = vb_verts_y;

    CVertexBuffer& vb     = *bspface.vbuffer;
    int            arrays = vb.getShader()->getUsedArrays() & ~INDEXARRAY;
    vb.AllocArrays(arrays, vb_verts_x * vb_verts_y);

    vb.num_vertices = vb.getMaxNumVertices();

    Bezier::evalQuadricBezierPatch(cps, num_cpx, num_cpy, vb.vertices[0], tess_x, tess_y);
    if (arrays & TEXCOORDARRAY) {
        Bezier::evalQuadricBezierPatch(texc, num_cpx, num_cpy, vb.texcoords, tess_x, tess_y);
    }
    if (arrays & LMCOORDARRAY) {
        Bezier::evalQuadricBezierPatch(lmc, num_cpx, num_cpy, vb.lmcoords, tess_x, tess_y);
    }
    if (arrays & COLORARRAY) {
        Bezier::evalQuadricBezierPatch(col, num_cpx, num_cpy, vb.colors, tess_x, tess_y);
    }

    if (arrays & NORMALARRAY) {
        // this one is using quake´s delivered normals and interpolates them
        Bezier::evalQuadricBezierPatch(norms, num_cpx, num_cpy, vb.normals[0], tess_x, tess_y);
        // this would analytically determine the normals (more expensive)
        // Bezier::evalQuadricBezierPatchNormals(cps,num_cpx,num_cpy,vb.normals[0],tess_x,tess_y);
    }

    VBServices::createGridIndices(vb, vb_verts_x, vb_verts_y);
}

void Q3BSPLoader::generateFlare(const Q3BSPLoader::Q3SURFACE& face, CVertexBuffer& vb)
{
    VECTOR3 right, up, normal, org;

    org = QVector3(face.lightmap_origin);

    // FIXME: actually those flares could be ways better handled if they were real seperate sprite-objects
    // they even could get a logic to softly fade in and out
    right = VECTOR3(40, 0, 0);
    up    = VECTOR3(0, 40, 0);

    vb.AllocArrays(VERTEXARRAY | TEXCOORDARRAY | INDEXARRAY, 4, 6);  // NORMALARRAY

    vb.mode = GL_TRIANGLES;

    vb.num_vertices = 4;
    vb.num_indices  = 6;

    vb.vertices[0][2] = org - right + up;
    vb.vertices[0][0] = org - right - up;
    vb.vertices[0][1] = org + right - up;
    vb.vertices[0][3] = org + right + up;

    vb.texcoords[2] = VECTOR2(0, 0);
    vb.texcoords[0] = VECTOR2(0, 1);
    vb.texcoords[1] = VECTOR2(1, 1);
    vb.texcoords[3] = VECTOR2(1, 0);

    vb.indices[0] = 0;
    vb.indices[1] = 1;
    vb.indices[2] = 2;
    vb.indices[3] = 2;
    vb.indices[4] = 1;
    vb.indices[5] = 3;

    CShader* shader = vb.getShader();

    assert(shader);

    // normally the shader of the vertexbuffer is already set
    // but I have to modify it a bit to get the right blending and automatic billboarding
    /*	shader->setSort(SORT_NEAREST-SORT_ONE-SORT_ONE); // noch hinter fog
        shader->addVertexDeformAutosprite();

        CShaderPass &pass=shader->getPass(0);
        pass.setDepthTest(false);
        pass.setBlendFunc(GL_ONE,GL_ONE);
        pass.setRGBGen(RGBGEN_FLARE); // distance-dependend vertex-color

        shader->validate();*/
}

void Q3BSPLoader::ReadLightgrid()
{
    file->setPosition(header.lumps[LUMP_LIGHTGRID].offset);
    worldmodel->num_lightvols = header.lumps[LUMP_LIGHTGRID].length / sizeof(Q3LIGHTVOL);

    if (!worldmodel->num_lightvols) {
        cout << "WARNING: bsp contains no lightgrid" << endl;
        return;
    }

    worldmodel->num_lightvols_x = (unsigned int)(floorf(worldmodel->bbox.max.x / worldmodel->gridsize.x) -
                                                 ceilf(worldmodel->bbox.min.x / worldmodel->gridsize.x) + 1);
    worldmodel->num_lightvols_y = (unsigned int)(floorf(worldmodel->bbox.max.y / worldmodel->gridsize.y) -
                                                 ceilf(worldmodel->bbox.min.y / worldmodel->gridsize.y) + 1);
    worldmodel->num_lightvols_z = (unsigned int)(floorf(worldmodel->bbox.max.z / worldmodel->gridsize.z) -
                                                 ceilf(worldmodel->bbox.min.z / worldmodel->gridsize.z) + 1);

    unsigned int realnum_lightvols =
        worldmodel->num_lightvols_x * worldmodel->num_lightvols_y * worldmodel->num_lightvols_z;
    if (realnum_lightvols != worldmodel->num_lightvols) {
        std::cout << "Q3BSPLoader::ReadLightVols():\n   ^3 calculated number of lightvols differs from bsp file"
                  << std::endl;
        return;
    }

    worldmodel->lightvols = new Q3BSPMesh::LIGHTVOL[worldmodel->num_lightvols];
    file->readVOID(worldmodel->lightvols, header.lumps[LUMP_LIGHTGRID].length);
}

void Q3BSPLoader::ReadLeafBrushes()
{
    file->setPosition(header.lumps[LUMP_LEAFBRUSHES].offset);
    worldmodel->num_leafbrushes = header.lumps[LUMP_LEAFBRUSHES].length / sizeof(int);
    worldmodel->leafbrushes     = new int[worldmodel->num_leafbrushes];
    file->readVOID(worldmodel->leafbrushes, header.lumps[LUMP_LEAFBRUSHES].length);
}

void Q3BSPLoader::ReadBrushSides()
{
    file->setPosition(header.lumps[LUMP_BRUSHSIDES].offset);
    int length = header.lumps[LUMP_BRUSHSIDES].length;

    worldmodel->num_brushsides = length / sizeof(Q3BRUSHSIDE);
    worldmodel->brushsides     = new Q3BSPMesh::BRUSHSIDE[worldmodel->num_brushsides];

    file->readVOID(worldmodel->brushsides, length);
}

void Q3BSPLoader::ReadBrushes()
{
    file->setPosition(header.lumps[LUMP_BRUSHES].offset);
    int length = header.lumps[LUMP_BRUSHES].length;

    worldmodel->num_brushes = length / sizeof(Q3BRUSH);
    worldmodel->brushes     = new Q3BSPMesh::BRUSH[worldmodel->num_brushes];

    Q3BRUSH* q3brushes = new Q3BRUSH[worldmodel->num_brushes];
    file->readVOID(q3brushes, length);

    for (int b = 0; b < worldmodel->num_brushes; b++) {
        worldmodel->brushes[b].startbrushside = q3brushes[b].firstside;
        worldmodel->brushes[b].num_brushsides = q3brushes[b].num_sides;
        worldmodel->brushes[b].surfaceinfo    = q3brushes[b].shadernum;
    }

    KILLARRAY(q3brushes)
}

void Q3BSPLoader::ReadEffects()
{
    file->setPosition(header.lumps[LUMP_FOGS].offset);
    int length = header.lumps[LUMP_FOGS].length;

    worldmodel->num_fogs = length / sizeof(Q3FOG);

    if (!worldmodel->num_fogs)
        return;  // its completely normal that a map may have no fog

    Q3FOG* q3fogs = new Q3FOG[worldmodel->num_fogs];
    file->readVOID(q3fogs, length);

    // effects=new Q3BSPMesh::EFFECT[num_effects];

    worldmodel->fogfeatures = new FOGFEATURE[worldmodel->num_fogs];
    FOGFEATURE* fogfeatures = worldmodel->fogfeatures;

    for (int i = 0; i < worldmodel->num_fogs; ++i) {
        /* the EFFECT structure is no longer used for fog
        effects[i].shader=CShader::FindOrLoadShader(q3effects[i].shadername);
        if (!effects[i].shader) effects[i].shader=CShader::DefaultShader;
        effects[i].shader->AddRef();
        effects[i].brush=q3effects[i].firstbrush;
        effects[i].sideofbrush=q3effects[i].num_brushes;*/

        fogfeatures[i].shader = CShader::findOrLoadShader(q3fogs[i].shadername);
        if (!fogfeatures[i].shader) {
            cout << "WARNING: fog-shader '" << q3fogs[i].shadername << "' not found" << endl;
            fogfeatures[i].shader = CShader::DefaultShader;
        }
        fogfeatures[i].plane =
            worldmodel->planes
                [worldmodel->brushsides[worldmodel->brushes[q3fogs[i].brushnum].startbrushside + q3fogs[i].visibleside]
                     .plane];
        fogfeatures[i].brushnum = q3fogs[i].brushnum;
    }
    KILLARRAY(q3fogs);
}
