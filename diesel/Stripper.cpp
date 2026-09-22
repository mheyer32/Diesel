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
#include "Stripper.h"

#include <misc/QSort.h>
#include <defs.h>
#include <stdlib.h>

#include <MemoryTracker.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EDGE
{
    int v1, v2, triangle;  // alles Indizes

    inline bool operator==(const EDGE& e2) const
    {
        if ((v1 == e2.v1) && (v2 == e2.v2))
            return true;
        return false;
    }
    inline bool operator<(const EDGE& e2) const { return (v1 < e2.v1) || ((v1 == e2.v1) && (v2 < e2.v2)); };
};

#define FLAG_INSTRIP (1 << 0)
#define FLAG_ENDSTRIP (1 << 1)  // end or start of strip

#define TRI_INVALID 0xFFFFFFFF

struct TRIADJ
{
    int adjtri[3];  // adjazente Dreiecke an Kante 0-1  1-2  2-0
    int num_neighbours;
    int flags;

    INDEX vref[3];  // Vertex-Referenzen

    TRIADJ()
    {
        adjtri[0] = adjtri[1] = adjtri[2] = 0xFFFFFFFF;
        vref[0] = vref[1] = vref[2] = 0;
        flags = num_neighbours = 0;
    }
    inline bool operator<(const TRIADJ& a2) const { return num_neighbours < a2.num_neighbours; };
    inline bool operator>(const TRIADJ& a2) const { return num_neighbours > a2.num_neighbours; };
};

#define GETEDGE(Tri_number) (((Tri_number) & 0xF0000000) >> 28)
#define GETINDEX(Tri_number) ((Tri_number) & 0x0FFFFFFF)
#define SETEDGE(Tri_number, Edge) (GETINDEX(Tri_number) | (Edge << 28))

inline void rotateIndices(INDEX* indices, int index)  // CCW-Index Rotation of 3 following indices[index] for a triangle
{
    INDEX ti;
    ti                 = indices[index + 2];
    indices[index + 2] = indices[index + 1];
    indices[index + 1] = indices[index];
    indices[index]     = ti;
}

inline void swapIndizes(INDEX* indices, int index1, int index2)
{
    int i1, i2, i3;
    i1                  = indices[index1];
    i2                  = indices[index1 + 1];
    i3                  = indices[index1 + 2];
    indices[index1]     = indices[index2];
    indices[index1 + 1] = indices[index2 + 1];
    indices[index1 + 2] = indices[index2 + 2];
    indices[index2]     = i1;
    indices[index2 + 1] = i2;
    indices[index2 + 2] = i3;
}
// Dreiecke sind def. durch vertices 0 - 1 - 2
// Kante 0  0-1
// Kante 1  2-0
// Kante 2  1-2

// find edgenumber (0,1,2) of given edge in given triangle
inline int findEdgeNum(const TRIADJ& adj, const EDGE& edge)
{
    int num = 0;
    if ((adj.vref[2] == edge.v1) || (adj.vref[2] == edge.v2)) {
        num = 2;
    }
    if ((adj.vref[1] == edge.v1) || (adj.vref[1] == edge.v2)) {
        num |= 1;
    }
    return num - 1;
}

void makeEdges(const INDEX* indices, EDGE* edges, int num_indices)
{
    // Trifaces ist Indexliste für Dreiecke, abc abc abc abc...
    // num_Indizes ist die Anzahl aller Indizes (3 pro Dreieck)
    int e = 0, i = 0;
    int a, b, temp;

    num_indices /= 3;
    for (int t = 0; t < num_indices; ++t) {
        edges[e].triangle = t;
        a                 = indices[i];
        b                 = indices[i + 1];
        if (a > b)  // die Vertex-Ref sind son angeordnet, das immer a<b gilt
        {           //  1-2 und 2-1 werden beide als 1-2 markiert
            temp = a;
            a    = b;
            b    = temp;
        }
        edges[e].v1 = a;
        edges[e].v2 = b;
        e++;

        edges[e].triangle = t;
        a                 = indices[i + 1];
        b                 = indices[i + 2];
        if (a > b) {
            temp = a;
            a    = b;
            b    = temp;
        }
        edges[e].v1 = a;
        edges[e].v2 = b;
        e++;

        edges[e].triangle = t;
        a                 = indices[i + 2];
        b                 = indices[i];  // wieder der erste Vertex-Index des Dreiecks
        if (a > b) {
            temp = a;
            a    = b;
            b    = temp;
        }
        edges[e].v1 = a;
        edges[e].v2 = b;
        e++;
        i += 3;
    }
}

void sortForAdjacency(EDGE* edges, int num_edges)
{
    quicksort<512>(edges, num_edges, 50);
}

void buildTriAdj(const INDEX* indices, TRIADJ* adj, int num_indices)
{
    // Edge-List bauen
    EDGE* edges = new EDGE[num_indices];  //	num_tris= num_indices/3

    makeEdges(indices, edges, num_indices);

    sortForAdjacency(edges, num_indices);

    // Vertex-Referenzen aus Indexliste übernehmen
    int i = 0;
    for (int t = 0; t < num_indices / 3; t++) {
        adj[t].vref[0] = indices[i];
        adj[t].vref[1] = indices[i + 1];
        adj[t].vref[2] = indices[i + 2];
        i += 3;
    }

    // vorsortierte Edge-List ist nötig!!,
    // durch Sortierung stehen zwei gleiche Kanten
    // hintereinander in der Edge-List -> benachbarte Dreiecke
    int e = 0, tri1, tri2, num1, num2;
    while (e < num_indices - 1) {
        if (edges[e] == edges[e + 1]) {
            tri1 = edges[e].triangle;  // get both triangles connected by edge e (and e+1)
            tri2 = edges[e + 1].triangle;

            num1 = findEdgeNum(adj[tri1], edges[e]);  // find this edge in first triangle
            num2 = findEdgeNum(adj[tri2], edges[e]);  // and in second

            adj[tri1].adjtri[num1] =
                SETEDGE(tri2, num2);  // set the neighbour triangle for tri1
                                      // and edgenum (in tri2) over which one comes from tri1 to tri2
            adj[tri1].num_neighbours++;

            adj[tri2].adjtri[num2] = SETEDGE(tri1, num1);
            adj[tri2].num_neighbours++;
            e += 2;
        } else
            e++;
    }

    KILLARRAY(edges);
}

// ermittelt die entstehende striplänge ausgehend von Dreieck tri1 aus AdjenzensListe triadj
// in Richtung e1; die Indizes (innerhalb von triadj) der gestrippten Dreicke verbunden
// mit der eingehenden Kante (in den oberen 8 Bit) (siehe EDGE) landen in stripbuffer

int getStriplen(TRIADJ* triadj, int tri1, int e1, int* stripbuffer)
{
    int  striplen   = 0, tri2, e2;
    bool dothestrip = true;
    bool mode       = true;

    switch (e1)  // ausgehende Kante suchen
    {
    case 0:
        e2 = 1;
        break;
    case 1:
        e2 = 2;
        break;
    case 2:
        e2 = 0;
        break;
    }
    tri1 = SETEDGE(GETINDEX(tri1), e2);

    while (1) {
        tri2 = GETINDEX(tri1);
        if (triadj[tri2].flags & FLAG_INSTRIP)
            break;

        // altes Dreieck eintragen
        triadj[tri2].flags |= FLAG_INSTRIP;
        stripbuffer[striplen++] = tri1;

        // Nachfolgendes Dreieck holen
        tri1 = triadj[tri2].adjtri[e1];

        // legaler Nachbar?
        if (tri1 == 0xFFFFFFFF)
            break;

        e1 = GETEDGE(tri1);  // über diese Kante komme ich herein
        switch (e1)          // ausgehende Kante suchen
        {
        case 0:
            e1 = mode ? 1 : 2;
            break;
        case 1:
            e1 = mode ? 2 : 0;
            break;
        case 2:
            e1 = mode ? 0 : 1;
            break;
        }
        mode = !mode;
    }

    for (tri1 = 0; tri1 < striplen; tri1++) {
        triadj[GETINDEX(stripbuffer[tri1])].flags &= ~FLAG_INSTRIP;
    }
    return striplen;
}

// indices  point to array of indices to be written
// index is first free index ( will be updated)
// triadj	array of triangle adjacencies
// triangle	index in triadj of starttriangle
// edge		edge of starttriangle (way to go)

// Dreiecke sind def. durch vertices 0 - 1 - 2
// Kante 0  0-1
// Kante 1  2-0
// Kante 2  1-2

int doStrip(INDEX* indices, int& index, TRIADJ* triadj, int triangle, int edge)  // returns number of stripped tris
{
    bool mode     = false;
    int  striplen = 0;

    switch (edge)  // tue so, als kämen wir aus einem vorherigen dreieck (erleichtert untere schleife)
    {
    case 0:
        edge = 1;
        break;
    case 1:
        edge = 2;
        break;
    case 2:
        edge = 0;
        break;
    }

    int lasttri = triangle;

    TRIADJ* adj = &triadj[triangle];
    triadj[triangle].flags |= FLAG_ENDSTRIP;

    while (1) {
        // dieses Dreieck als benutzt kennzeichnen
        adj->flags |= FLAG_INSTRIP;
        int openedge;  // edge of current tri which will not be used  in strip
        switch (edge) {
        case 0:
            indices[index]     = adj->vref[0];
            indices[index + 1] = adj->vref[1];
            indices[index + 2] = adj->vref[2];
            if (mode) {
                edge     = 1;
                openedge = 2;
            } else {
                edge     = 2;
                openedge = 1;
            }

            break;
        case 1:
            indices[index]     = adj->vref[2];
            indices[index + 1] = adj->vref[0];
            indices[index + 2] = adj->vref[1];
            if (mode) {
                edge     = 2;
                openedge = 0;
            } else {
                edge     = 0;
                openedge = 2;
            }
            break;
        case 2:
            indices[index]     = adj->vref[1];
            indices[index + 1] = adj->vref[2];
            indices[index + 2] = adj->vref[0];
            if (mode) {
                edge     = 0;
                openedge = 1;
            } else {
                edge     = 1;
                openedge = 0;
            }
            break;
        }

        int neighbour = adj->adjtri[openedge];
        if (neighbour != TRI_INVALID) {
            triadj[GETINDEX(neighbour)].num_neighbours -= 1;
            triadj[GETINDEX(neighbour)].adjtri[GETEDGE(neighbour)] = TRI_INVALID;
        }

        mode = !mode;
        index += 3;

        striplen++;

        lasttri = triangle;
        // in vorgesehener Richtung zum nächsten Strip-Nachbar
        triangle = adj->adjtri[edge];

        // legaler Nachbar?
        if (triangle == TRI_INVALID)
            break;

        edge     = GETEDGE(triangle);
        triangle = GETINDEX(triangle);
        adj      = &triadj[triangle];

        // nachbar schon teil eines strips?
        if (adj->flags & FLAG_INSTRIP)
            break;
    }

    triadj[lasttri].flags |= FLAG_ENDSTRIP;

    return striplen;
}

void ArrangeForStrips(INDEX* indices, int num_indices, int options)
{
    int num_triangles = num_indices / 3;

    TRIADJ* triadj = new TRIADJ[num_triangles];

    buildTriAdj(indices, triadj, num_indices);

    int* sorted = new int[num_triangles];

    for (int t = 0; t < num_triangles; ++t) {
        sorted[t] = t;
    }

    int starttri        = rand() % num_triangles;
    int num_usedindices = 0;

    do {
        // find best stripdirection
        int stripbuffer[3000];  // holds indices of strip-ordered adjacend triangles
        int striplen = 0;
        int bestlen  = 0;
        int besttri;
        int startedge = 0;

        for (int testedge = 0; testedge < 3; ++testedge) {
            striplen = getStriplen(triadj, starttri, testedge, stripbuffer);
            if (striplen > bestlen) {
                besttri = stripbuffer[striplen - 1];  // ich merke mir nur das letzte Dreieck des Strips
                bestlen = striplen;
            }
        }
        starttri  = GETINDEX(besttri);
        startedge = GETEDGE(besttri);

        doStrip(indices, num_usedindices, triadj, starttri, startedge);

        if (num_usedindices == num_indices)
            break;

        // find "best" suited next tri. This is the point were I could need a better heuristic
        besttri = -1;
        bestlen = 999;
        for (starttri = 0; starttri < num_triangles; ++starttri) {
            if ((!(triadj[starttri].flags & FLAG_INSTRIP)) && triadj[starttri].num_neighbours <= bestlen) {
                bestlen = triadj[starttri].num_neighbours;
                besttri = starttri;
            }
        }

        starttri = besttri;
    } while (starttri != -1);

    KILLARRAY(triadj);
    KILLARRAY(sorted);
}
