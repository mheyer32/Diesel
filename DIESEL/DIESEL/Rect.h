#pragma once

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <math/vector2.h>

#define RECT_NOINTERSECTION 0
#define RECT_CLIPPED 1
#define RECT_CONTAINED 2

struct RECTANGLE
{
    VECTOR2 min;
    VECTOR2 max;

    RECTANGLE()
    : min(VECTOR2(999999, 999999))
    , max(VECTOR2(-999999, -999999)) {};
    RECTANGLE(const VECTOR2& nmin, const VECTOR2& nmax)
    : min(nmin)
    , max(nmax) {};

    inline RECTANGLE& operator|=(const RECTANGLE& rect2);
    inline RECTANGLE& operator|=(const VECTOR2& v);
};

inline void resetRectangle(RECTANGLE& r)
{
    r.min = VECTOR2(999999, 999999);
    r.max = VECTOR2(-999999, -999999);
}

inline RECTANGLE& RECTANGLE::operator|=(const VECTOR2& v)
{
    if (v.x < min.x)
        min.x = v.x;
    if (v.x > max.x)
        max.x = v.x;
    if (v.y < min.y)
        min.y = v.y;
    if (v.y > max.y)
        max.y = v.y;

    return *this;
}

inline RECTANGLE& RECTANGLE::operator|=(const RECTANGLE& rect2)
{
    if (rect2.min.x < min.x)
        min.x = rect2.min.x;
    if (rect2.min.y < min.y)
        min.y = rect2.min.y;
    if (rect2.max.x > max.x)
        max.x = rect2.max.x;
    if (rect2.max.y > max.y)
        max.y = rect2.max.y;

    return *this;
}

inline bool intersectRectangle(const RECTANGLE& rect1, const RECTANGLE& rect2)
{
    if (rect1.max.x < rect2.min.x)
        return false;
    if (rect1.min.x > rect2.max.x)
        return false;
    if (rect1.max.y < rect2.min.y)
        return false;
    if (rect1.min.y > rect2.max.y)
        return false;
    return true;
}

inline int clipRectangle(RECTANGLE& rectangle, const RECTANGLE& cliprect)
{
    if (!intersectRectangle(rectangle, cliprect)) {
        return RECT_NOINTERSECTION;
    }
    int clipped = RECT_CONTAINED;
    if (rectangle.min.x < cliprect.min.x) {
        rectangle.min.x = cliprect.min.x;
        clipped         = RECT_CLIPPED;
    }
    if (rectangle.max.x > cliprect.max.x) {
        clipped         = RECT_CLIPPED;
        rectangle.max.x = cliprect.max.x;
    }
    if (rectangle.min.y < cliprect.min.y) {
        clipped         = RECT_CLIPPED;
        rectangle.min.y = cliprect.min.y;
    }
    if (rectangle.max.y > cliprect.max.y) {
        clipped         = RECT_CLIPPED;
        rectangle.max.y = cliprect.max.y;
    }

    return clipped;
}