#include "Frustum.h"

Frustum::Frustum(int size)
: m_planes(size)
{
    if (size == 0)
        m_planes.reserve(6);
    else
        m_planes.resize(size);
}

Frustum::Frustum(PLANE* plns, int num_plns)
: m_planes(plns, plns + num_plns)
{
}

bool Frustum::BoxInFrustum(const BBOX& bbox) const
{
    std::vector<PLANE>::size_type size = m_planes.size();
    for (std::vector<PLANE>::size_type p = 0; p < size; ++p) {
        if (BoxBehind(bbox, m_planes[p]))
            return false;
    }
    return true;
}

int Frustum::BoxOnFrustumSide(const BBOX& bbox) const
{
    int                           allsides = 0;
    std::vector<PLANE>::size_type size     = m_planes.size();
    for (std::vector<PLANE>::size_type p = 0; p < size; ++p) {
        int side = BoxOnPlaneSide(bbox, m_planes[p]);
        allsides |= side;
        if (side == BACKSIDE)
            return side;
        if (allsides == SPANNING)
            return allsides;
    }
    return allsides;
}

bool Frustum::PointInFrustum(const VECTOR3& point) const
{
    std::vector<PLANE>::size_type size = m_planes.size();
    for (std::vector<PLANE>::size_type p = 0; p < size; ++p) {
        if (PointOnPlaneSide(point, m_planes[p]) == BACKSIDE)
            return false;
    }
    return true;
}
/*
 bool Frustum::SphereInFrustum(const SPHERE &sphere) const
{
    std::vector<PLANE>::size_type size=m_planes.size();
    for (std::vector<PLANE>::size_type p=0; p<size; ++p)
    {
        if (PointDistance(sphere.position,m_planes[p])<-sphere.radius) return false;
    }
    return true;
}

 bool Frustum::EllipsoidInFrustum(const ELLIPSOID &ellipsoid) const
{
    std::vector<PLANE>::size_type size=m_planes.size();
    for (std::vector<PLANE>::size_type p=0; p<size; ++p)
    {
        if (PointDistance(ellipsoid.position,m_planes[p])<-getEllipsoidRadius(ellipsoid,m_planes[p].n)) return false;
    }
    return true;
}
*/
void Frustum::setFromMatrix(const MATRIX4& M)
{
    m_planes.resize(6);  // ensure enouh space

    // still Room for optimization, but not that needed
    m_planes[CLIP_LEFT]   = PLANE(M.m30 + M.m00, M.m31 + M.m01, M.m32 + M.m02, -(M.m33 + M.m03));
    m_planes[CLIP_RIGHT]  = PLANE(M.m30 - M.m00, M.m31 - M.m01, M.m32 - M.m02, -(M.m33 - M.m03));
    m_planes[CLIP_BOTTOM] = PLANE(M.m30 + M.m10, M.m31 + M.m11, M.m32 + M.m12, -(M.m33 + M.m13));
    m_planes[CLIP_TOP]    = PLANE(M.m30 - M.m10, M.m31 - M.m11, M.m32 - M.m12, -(M.m33 - M.m13));
    m_planes[CLIP_NEAR]   = PLANE(M.m30 + M.m20, M.m31 + M.m21, M.m32 + M.m22, -(M.m33 + M.m23));
    m_planes[CLIP_FAR]    = PLANE(M.m30 - M.m20, M.m31 - M.m21, M.m32 - M.m22, -(M.m33 - M.m23));
}