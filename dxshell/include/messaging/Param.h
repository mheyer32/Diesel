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
// $Id: Param.h,v 1.2 2003-07-04 18:12:30 skynet Exp $

#ifndef PARAM_H_DA2718098ECA4ED3BF28B90621BB9846_INCLUDED
#define PARAM_H_DA2718098ECA4ED3BF28B90621BB9846_INCLUDED

#include <typeinfo>

namespace Msg
{

class Param
{
public:
    Param() {}

    /// declare it virtual just to make this a polymorphic class
    virtual ~Param() {}
};

/////////////////////////////////////////////////////////////////////////

template <typename P1>
class Param1 : public Param
{
public:
    Param1(P1 par1)
    : m_p1(par1)
    {
    }
    P1   p1() const { return m_p1; }
    void p1(P1 par1) { m_p1 = par1; }

protected:
    P1 m_p1;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename P1, typename P2>
class Param2 : public Param1<P1>
{
public:
    Param2(P1 par1, P2 par2)
    : Param1<P1>(par1)
    , m_p2(par2)
    {
    }
    P2   p2() const { return m_p2; }
    void p2(P2 par2) { m_p2 = par2; }

protected:
    P2 m_p2;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename P1, typename P2, typename P3>
class Param3 : public Param2<P1, P2>
{
public:
    Param3(P1 par1, P2 par2, P3 par3)
    : Param2<P1, P2>(par1, par2)
    , m_p3(par3)
    {
    }
    P3   p3() const { return m_p3; }
    void p3(P3 par3) { m_p3 = par3; }

protected:
    P3 m_p3;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename P1, typename P2, typename P3, typename P4>
class Param4 : public Param3<P1, P2, P3>
{
public:
    Param4(P1 par1, P2 par2, P3 par3, P4 par4)
    : Param3<P1, P2, P3>(par1, par2, par3)
    , m_p4(par4)
    {
    }
    P4   p4() const { return m_p4; }
    void p4(P4 par4) { m_p4 = par4; }

protected:
    P4 m_p4;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

template <typename P1, typename P2, typename P3, typename P4, typename P5>
class Param5 : public Param4<P1, P2, P3, P4>
{
public:
    Param5(P1 par1, P2 par2, P3 par3, P4 par4, P5 par5)
    : Param4<P1, P2, P3, P4>(par1, par2, par3, par4)
    , m_p5(par5)
    {
    }
    P5   p5() const { return m_p5; }
    void p5(P5 par5) { m_p5 = par5; }

protected:
    P5 m_p5;
};

/////////////////////////////////////////////////////////////////////////

template <typename P1>
inline Param1<P1> const& prm1_cast(Param const& p)
{
#if GCC_VERSION == 30200
    assert(std::string(typeid(p).name()) == std::string(typeid(Param1<P1>).name()));
#else
    assert(typeid(p) == typeid(Param1<P1>));
#endif

    return static_cast<Param1<P1> const&>(p);
}

template <typename P1, typename P2>
inline Param2<P1, P2> const& prm2_cast(Param const& p)
{
#if GCC_VERSION == 30200
    assert(std::string(typeid(p).name()) == std::string(typeid(Param2<P1, P2>).name()));
#else
    assert(typeid(p) == typeid(Param2<P1, P2>));
#endif

    return static_cast<Param2<P1, P2> const&>(p);
}

template <typename P1, typename P2, typename P3>
inline Param3<P1, P2, P3> const& prm3_cast(Param const& p)
{
#if GCC_VERSION == 30200
    assert(std::string(typeid(p).name()) == std::string(typeid(Param3<P1, P2, P3>).name()));
#else
    assert(typeid(p) == typeid(Param3<P1, P2, P3>));
#endif

    return static_cast<Param3<P1, P2, P3> const&>(p);
}

template <typename P1, typename P2, typename P3, typename P4>
inline Param4<P1, P2, P3, P4> const& prm4_cast(Param const& p)
{
#if GCC_VERSION == 30200
    assert(std::string(typeid(p).name()) == std::string(typeid(Param4<P1, P2, P3, P4>).name()));
#else
    assert(typeid(p) == typeid(Param4<P1, P2, P3, P4>));
#endif

    return static_cast<Param4<P1, P2, P3, P4> const&>(p);
}

template <typename P1, typename P2, typename P3, typename P4, typename P5>
inline Param5<P1, P2, P3, P4, P5> const& prm5_cast(Param const& p)
{
#if GCC_VERSION == 30200
    assert(std::string(typeid(p).name()) == std::string(typeid(Param5<P1, P2, P3, P4, P5>).name()));
#else
    assert(typeid(p) == typeid(Param5<P1, P2, P3, P4, P5>));
#endif

    return static_cast<Param5<P1, P2, P3, P4, P5> const&>(p);
}

}  // namespace Msg
#endif
