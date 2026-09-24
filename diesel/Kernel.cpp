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
// Kernel.cpp: Implementierung der Klasse CKernel.
//
//////////////////////////////////////////////////////////////////////

#include "Kernel.h"
#include "Task.h"
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

// needed for sorting the list
bool CKernel::TCB::operator<(const CKernel::TCB& t2) const
{
    return task->getPriority() < t2.task->getPriority();
}

// needed for correctly removing tasks
bool CKernel::TCB::operator==(const CKernel::TCB& t2) const
{
    return task == t2.task;
}

CKernel::CKernel()
{
    timer.start();
    timescale = 1.0f;
    time      = 0.0;
}

CKernel::~CKernel()
{
}

void CKernel::runTasks()
{
    timer.stop();
    time       = timer.getElapsedSecs();
    scaledtime = time * timescale;

    // FIXME: what if a task removes itself while the list is traversed ?
    TASKITERATOR tIt = tasklist.begin();

    for (; tIt != tasklist.end(); ++tIt) {
        if ((tIt->task->getNextThink() >= 0.0f) && (scaledtime >= tIt->task->getNextThink())) {
            tIt->task->setNextThink(0.0f);
            tIt->task->runTask();
        }
    }
}

void CKernel::setTimeScale(float Scale)
{
}

float CKernel::getTime() const
{
    return scaledtime;
}

float CKernel::getRealTime() const
{
    return time;
}

void CKernel::addTask(CTask* Task)
{
    tasklist.push_back(Task);
    sortTasks();
}

void CKernel::removeTask(CTask* Task)
{
    tasklist.remove(Task);
}

void CKernel::sortTasks()
{
    tasklist.sort();
}