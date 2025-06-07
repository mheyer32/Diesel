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
// Kernel.h: Schnittstelle für die Klasse CKernel.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KERNEL_H__D7DAB6BF_CA30_4565_9D3F_8133C1DEF0AF__INCLUDED_)
#define AFX_KERNEL_H__D7DAB6BF_CA30_4565_9D3F_8133C1DEF0AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include <adt/singleton.h>
#include <misc/Timer.h>
#include <list>

class CTask;

/** intended to be used as cooperative micro-kernel,
maybe I´m gonna using it some day :-)
*/

class CKernel : public Singleton<CKernel>
{

    DECLARE_SINGLETON(CKernel)

public:
    enum TASKPRIORTY
    {
        PRECULLING  = -1500,
        CULLING     = -1000,
        PRERENDER   = -500,
        RENDERWORLD = 0,
        POSTRENDER  = 500,
    };

    /** add task to the list of active, callable tasks */
    void addTask(CTask* Task);

    /** remove task from list */
    void removeTask(CTask* Task);

    /** sort tasklist for priorities, usually not called by the user */
    void sortTasks();  // sort Tasklist for priority

    /** get time, probably scaled */
    float getTime() const;

    /** get real time, not scaled */
    float getRealTime() const;

    /** set scaling to time. a value of 0.5 will make things half as fast as normal */
    void setTimeScale(float Scale);

    /** go through the list of active tasks, calling all tasks that whose point of time is
        arrived or already over. This function call is probably placed somewhere in the mainloop
    */
    void runTasks();

private:
    CKernel();
    ~CKernel();

    CTimer timer;
    float  time;
    float  timescale;
    float  scaledtime;

    struct TCB
    {
        CTask* task;
        TCB(CTask* ntask)
        : task(ntask) {};
        bool operator<(const TCB& t2) const;
        bool operator==(const TCB& t2) const;
    };

    typedef std::list<TCB>     TASKLIST;
    typedef TASKLIST::iterator TASKITERATOR;

    TASKLIST tasklist;
};

#endif  // !defined(AFX_KERNEL_H__D7DAB6BF_CA30_4565_9D3F_8133C1DEF0AF__INCLUDED_)
