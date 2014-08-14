// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef EVENTS_H
#define EVENTS_H

#include <deque>
#include <map>
#include <string>

/*
Send specific events with keys and types to separate them
An automatically assigned ID might be better...
Instead of using inheritance, maybe a getId() would be better.
This way the key is always of type int, AND the user (programmer)
    doesn't need to specify it.
Only downside to this is that it wouldn't integrate well with an entity system
    that uses unique int IDs as well, since you'd want to use those for sending
    events related to specific entities.
Actually, for specific entity events, you'd probably just have event queues on
    each entity, and you could send them events directly. They would also
    receive them internally without having to know their ID or anything.
Well, the entities would need event queue components in this case.

Rename Events -> Messages
Rename Broadcasts -> Events

OR

Rename Broadcasts -> Notifier

But, both are still events, just that these can be sorted differently.
    It just gets messy combining them into 1 class...

Actually, don't do anything until I actually use this to see how it is...
*/
class Events
{
    public:
        // Hardcoding this for now, since it makes it much simpler to use
        using Key = std::string;

        // Note: T is the event type
        template <class T>
        using EventQueue = std::deque<T>;

        template <class T>
        using EventMap = std::map<Key, EventQueue<T> >;

        // Returns a unique ID to use for different events
        template <class T>
        static int getId()
        {
            static int id = 0;
            return id++;
        }

        // Returns the entire map storing all event queues for specified key type
        template <class T>
        static EventMap<T>& get()
        {
            static EventMap<T> events;
            return events;
        }

        // Returns specific event queue (specified by a key of any type)
        template <class T>
        static EventQueue<T>& get(const Key& key)
        {
            return get<T>()[key];
        }

        // Sends a specific event
        template <class T>
        static void send(const Key& key, const T& event)
        {
            get<T>(key).push_back(event);
        }

        // Sends a specific event (forwards arguments)
        template <class T, class... Args>
        static void send(const Key& key, Args&&... args)
        {
            get<T>(key).emplace_back(std::forward<Args>(args)...);
        }

        template <class T>
        static void clear(const Key& key)
        {
            get<T>(key).clear();
        }
};

#endif
