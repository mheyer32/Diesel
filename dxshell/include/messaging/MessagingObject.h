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
// $Id: MessagingObject.h,v 1.4 2003-08-28 11:23:08 skynet Exp $

#pragma once

#ifndef MESSAGINGOBJ_H_93CA5ACC22A645D2B467FF025A4FA399_INCLUDED
#define MESSAGINGOBJ_H_93CA5ACC22A645D2B467FF025A4FA399_INCLUDED

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>

#include <cassert>
#include <list>
#include <map>
#include <set>
#include <typeinfo>

#include "Param.h"

// MSGx macros will produce typdefs that define a type names "msgname_TYPE" that reflect the type of the
// message´s parameter object
#define MSG0(msgname)                                                                                                  \
    namespace Msg                                                                                                      \
    {                                                                                                                  \
    typedef Param msgname##_PARAM;                                                                                     \
    }
#define MSG1(msgname, paramtype1)                                                                                      \
    namespace Msg                                                                                                      \
    {                                                                                                                  \
    typedef Param1<paramtype1> msgname##_PARAM;                                                                        \
    }
#define MSG2(msgname, paramtype1, paramtype2)                                                                          \
    namespace Msg                                                                                                      \
    {                                                                                                                  \
    typedef Param2<paramtype1, paramtype2> msgname##_PARAM;                                                            \
    }
#define MSG3(msgname, paramtype1, paramtype2, paramtype3)                                                              \
    namespace Msg                                                                                                      \
    {                                                                                                                  \
    typedef Param3<paramtype1, paramtype2, paramtype3> msgname##_PARAM;                                                \
    }
#define MSG4(msgname, paramtype1, paramtype2, paramtype3, paramtype4)                                                  \
    namespace Msg                                                                                                      \
    {                                                                                                                  \
    typedef Param4<paramtype1, paramtype2, paramtype3, paramtype4> msgname##_PARAM;                                    \
    }

#include "Messages.h"

#undef MSG0
#undef MSG1
#undef MSG2
#undef MSG3
#undef MSG4

// macro to check a Param-object against the parameter type of the given message
#define __ASSERT_PARAMTYPE(msgname, parameterobject) assert(typeid(parameterobject) == typeid(msgname##_PARAM));

// use this inside your handleMessage() function to cast the given parameter object down
// to its real type.
// parameterobject is the const reference you´ll get through the function call
// localobject is the name of a reference that will be created locally which has the right type for
// the message
#define MSG_PARAMCAST(msgname, parameterobject, localobject)                                                           \
    __ASSERT_PARAMTYPE(msgname, parameterobject)                                                                       \
    const msgname##_PARAM& localobject = static_cast<const msgname##_PARAM&>(parameterobject);

namespace Msg
{
// redefine the macros to produce "msgname," entries used inside the enum declaration
#define MSG0(msgname) msgname,
#define MSG1(msgname, paramtype1) msgname,
#define MSG2(msgname, paramtype1, paramtype2) msgname,
#define MSG3(msgname, paramtype1, paramtype2, paramtype3) msgname,
#define MSG4(msgname, paramtype1, paramtype2, paramtype3, paramtype4) msgname,

enum MESSAGEID
{
    MIN_ID = WM_APP,
#include "Messages.h"
    INVALID_MSGID = 0xFFFFFFFF  // in order to close the list and force the messageIds to at least ints
};

#undef MSG0
#undef MSG1
#undef MSG2
#undef MSG3
#undef MSG4

/** the to-be-implemented handleMessage() method must return one of these values */
enum MSGRVAL
{
    PASS_ON,  ///< return this to pass the message to the next handler
    CONSUMED  ///< return this to stop traversing the handlers
};

/** MessagingObject is a class that enables derived classes to send and receive messages
 **/
class MessagingObject
{
protected:
    MessagingObject(void);
    virtual ~MessagingObject(void);

    /** Listeners get called through handleMessage().
     *	Implement this in your derived class
     **/
    virtual Msg::MSGRVAL handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId,
                                       const Msg::Param& parameters = Msg::Param());

    /** Use this method to register a listener at a message server object.
     **/
    void registerForMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId, int priority = 0);

    /** Use this method to unregister a listener at a msg server.
        It is allowed to unregister messages that you are not registered for.
        So if your object dynamically registers and unregistered for messages,
        just unregister for all of them inside you destructor in order to be sure.
    **/
    void unregisterForMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId);

    /** A server object passes a messages to all its listeners that registered for this messageId
     **/
    void sendMessage(Msg::MESSAGEID msgId, const Msg::Param& parameters = Param());

    /** A server object passes a message to one certain listener
     **/
    Msg::MSGRVAL sendMessage(Msg::MessagingObject* listener, Msg::MESSAGEID msgId,
                             const Msg::Param& parameters = Param());

    /** used to check the existance of a certain MessagingObject
     **/
    static bool exists(Msg::MessagingObject* msgobject);

private:
    /** Used to decorate the msgobject with a priority.
    *** the < operator is implemented, solists of MSGLISTENERENTRY can be sorted for priority
    **/
    struct MSGLISTENERENTRY
    {
        MessagingObject* m_msgobject;
        int              m_priority;

        ///	Attention! Is actually an operator> to get a reversed list after sorting!
        MSGLISTENERENTRY(MessagingObject* msgobject, int priority = 0)
        : m_msgobject(msgobject)
        , m_priority(priority) {};
        bool operator<(const MSGLISTENERENTRY& msghandler) const
        {
            // CAUTION: this operator does the opposite, which will force the list
            // to be sorted backwards, biggest priority first
            return (m_priority > msghandler.m_priority);
        }
    };

    typedef std::list<MSGLISTENERENTRY>          MSGLISTENERLIST;
    typedef std::map<MESSAGEID, MSGLISTENERLIST> MSGIDTOLISTENERMAP;
    typedef std::set<MessagingObject*>           MSGOBJECTSET;

    /** register the given listener for msgID
     **/
    void registerListener(MessagingObject* listener, MESSAGEID msgId, int priority);

    /** unregister the given listener
     **/
    void unregisterListener(MessagingObject* listener, MESSAGEID msgId);

    MSGIDTOLISTENERMAP
    m_msgid2listeners;  ///< serverobjects use this map to store the listeners interested for a certain message
    static MSGOBJECTSET* s_livingObjects;  ///< MessagingObject objects will automatically put themselves into that map
                                           ///< at construction time and
                                           /// remove themselves inside their destructor.
                                           /// Used to verify the existance of MessagingObject objects.
};

class MessageCallback : MessagingObject
{
public:
    typedef Msg::MSGRVAL (*MSGCALLBACKFUNC)(Msg::MessagingObject* sender, Msg::MESSAGEID msgId,
                                            const Msg::Param& parameters);

    MessageCallback(MSGCALLBACKFUNC func)
    : m_callbackfunc(func) {};
    /** Use this method to register a listener at a message server object.
     **/
    void registerForMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId, int priority = 0);

    /** Use this method to unregister a listener at a msg server.
    It is allowed to unregister messages that you are not registered for.
    So if your object dynamically registers and unregistered for messages,
    just unregister for all of them inside you destructor in order to be sure.
    **/
    void unregisterForMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId);

protected:
    virtual Msg::MSGRVAL handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId,
                                       const Msg::Param& parameters = Msg::Param());

    MSGCALLBACKFUNC m_callbackfunc;
};

}  // namespace Msg

#endif
