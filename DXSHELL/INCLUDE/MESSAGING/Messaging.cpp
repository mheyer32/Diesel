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
