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
// Messaging.cpp : Defines the entry point for the console application.
//

#include "TestKlasse.h"

int main()
{
    TestKlasse o1;
    {
        TestKlasse o2;

        o1.registerForMessage(&o2, Msg::HLI_MOVE_OBJECT, -100);
        o2.registerForMessage(&o1, Msg::HLI_MOVE_OBJECT, 200);

        o2.sendMessage(Msg::HLI_MOVE_OBJECT, Msg::HLI_MOVE_OBJECT_PARAM(1, 2, 3));
        o1.sendMessage(Msg::HLI_MOVE_OBJECT);

        //	o1.unregisterForMessage(&o2,HLI_MOVE_OBJECT);
        //		o2.unregisterForMessage(&o1,HLI_MOVE_OBJECT);
    }
    o1.sendMessage(Msg::HLI_MOVE_OBJECT);

    return 0;
}
