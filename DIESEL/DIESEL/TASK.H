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
// Task.h: Schnittstelle für die Klasse CTask.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASK_H__90FC34D6_F346_4F94_AC25_7A960A6F9875__INCLUDED_)
#define AFX_TASK_H__90FC34D6_F346_4F94_AC25_7A960A6F9875__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Kernel.h"

class CTask  
{
public:

	CTask(int Priority=CKernel::PRERENDER);
	virtual ~CTask();

	/** implement in derived classes */
	virtual void runTask()=0;

	/** set priority of task, see Kernel.h for some coarse values */
	void setPriority(int Priority);
	inline int getPriority() const // make inline so Kernel can sort as fast as possible
	{
		return priority;
	}

	/** whenever runTask() gets called by the kernel, you are obliged to set a new point in time
		you expect to be called again. otherwise runTask() won´t be called again. A negative value will 
		also prevent another call to runTask() until a new valid time is given*/
	void setNextThink(float time);

	/** when will the task be run again, time is an absolute point in time */
	float getNextThink() const;

private:
	
	CTask& operator = (const CTask &task2); // copying a task is not defined

	int priority;
	float nextthink;
};

#endif // !defined(AFX_TASK_H__90FC34D6_F346_4F94_AC25_7A960A6F9875__INCLUDED_)
