// $Id: MessagingObject.cpp,v 1.3 2003-08-28 00:29:56 skynet Exp $

#include "Messaging/MessagingObject.h"
#include <cassert>
#include <iostream>
#include <ostream>

namespace Msg
{
MessagingObject::MSGOBJECTSET* MessagingObject::s_livingObjects;

MessagingObject::MessagingObject(void)
{
    if (!s_livingObjects)
        s_livingObjects = new MSGOBJECTSET;
    s_livingObjects->insert(this);
}

MessagingObject::~MessagingObject(void)
{
    assert(exists(this));
    s_livingObjects->erase(this);
    if (s_livingObjects->empty()) {
        delete s_livingObjects;
        s_livingObjects = 0;
    }
}

bool MessagingObject::exists(MessagingObject* msgobject)
{
    return s_livingObjects->find(msgobject) != s_livingObjects->end();
}

/* Listener Side **************************************************************************************/
void MessagingObject::registerForMessage(MessagingObject* sender, MESSAGEID msgId, int priority)
{
    if (!exists(sender)) {
        std::cout << "WARNING: MessagingObject::registerForMessage() senderobject does not exist" << std::endl;
        return;
    }
    sender->registerListener(this, msgId, priority);
}

void MessagingObject::unregisterForMessage(MessagingObject* sender, MESSAGEID msgId)
{
    if (!exists(sender)) {
        std::cout << "WARNING: MessagingObject::registerForMessage()  senderobject does not exist" << std::endl;
        return;
    }
    sender->unregisterListener(this, msgId);
}

/* Server Side **************************************************************************************/
void MessagingObject::registerListener(MessagingObject* listener, MESSAGEID msgId, int priority)
{
    assert(exists(listener));

    MSGLISTENERLIST& list = m_msgid2listeners[msgId];

    list.push_back(MSGLISTENERENTRY(listener, priority));

    list.sort();
    // FIXME: need to check if listener got registered twice for the same message ??
}

void MessagingObject::unregisterListener(MessagingObject* listener, MESSAGEID msgId)
{
    MSGIDTOLISTENERMAP::iterator mlIt = m_msgid2listeners.find(msgId);

    // assert(mlIt!=m_msgid2listeners.end());	// a lister that unregisters itself is supposed to be registered
    if (mlIt == m_msgid2listeners.end()) {
        std::cout
            << "MessagingObject::unregisterListener() object tries to unregister message it is not registered for."
            << std::endl;
        return;
    }

    MSGLISTENERLIST&          listeners = mlIt->second;
    MSGLISTENERLIST::iterator lIt       = listeners.begin();
    MSGLISTENERLIST::iterator lItend    = listeners.end();

    for (; lIt != lItend;) {
        MSGLISTENERENTRY& listenerentry = *lIt;
        if (listenerentry.m_msgobject == listener) {
            lIt = listeners.erase(lIt);
            break;  // assume that listener can be only once inside the list
        } else {
            ++lIt;
        }
    }

    if (listeners.empty()) {
        // no listeners for this message are left
        m_msgid2listeners.erase(mlIt);
    }
}

// servers call their listeners through sendMessage();
void MessagingObject::sendMessage(MESSAGEID msgId, const Param& parameters)
{
    // std::cout<<"MessagingObject::sendMessage() in object "<<this<<std::endl;
    MSGIDTOLISTENERMAP::iterator mlIt = m_msgid2listeners.find(msgId);

    if (mlIt == m_msgid2listeners.end()) {
        // this object has no listeners for this message
        return;
    }

    MSGLISTENERLIST&          listeners = mlIt->second;
    MSGLISTENERLIST::iterator lIt       = listeners.begin();
    MSGLISTENERLIST::iterator lItend    = listeners.end();

    for (; lIt != lItend;) {
        const MSGLISTENERENTRY& listenerentry = *lIt;
        if (!exists(listenerentry.m_msgobject)) {
            std::cout << "WARNING: MessagingObject::sendMessage() listener unknown, probably didn´t unregister "
                         "properly. unregistering listener automatically."
                      << std::endl;
            lIt = listeners.erase(lIt);
            continue;
        } else {
            MSGRVAL rval = listenerentry.m_msgobject->handleMessage(this, msgId, parameters);
            if (rval == CONSUMED)
                break;
            ++lIt;
        }
    }

    // if the list of interested listeners is empty, erase it
    if (listeners.empty()) {
        m_msgid2listeners.erase(mlIt);
    }
}

// default MessageHandler does nothing
Msg::MSGRVAL MessagingObject::handleMessage(Msg::MessagingObject* /*sender*/, Msg::MESSAGEID /*msgId*/,
                                            const Msg::Param& /*parameters*/)
{
    assert(0);
    std::cout << "WARNING: empty MessagingObject::handleMessage() was called, check if this is intended!!" << std::endl;
    return Msg::PASS_ON;
}

Msg::MSGRVAL MessagingObject::sendMessage(Msg::MessagingObject* listener, Msg::MESSAGEID msgId,
                                          const Msg::Param& parameters)
{
    assert(exists(listener));
    return listener->handleMessage(this, msgId, parameters);
}

void MessageCallback::registerForMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId, int priority)
{
    MessagingObject::registerForMessage(sender, msgId, priority);
}

void MessageCallback::unregisterForMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId)
{
    MessagingObject::unregisterForMessage(sender, msgId);
}

Msg::MSGRVAL MessageCallback::handleMessage(Msg::MessagingObject* sender, Msg::MESSAGEID msgId,
                                            const Msg::Param& parameters)
{
    return m_callbackfunc(sender, msgId, parameters);
}

}  // namespace Msg
