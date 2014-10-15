// Copyright (C) 2014 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef EVENTS_H
#define EVENTS_H

#include <deque>

// Send global events with types to separate them
class Events
{
    public:
        template <class T>
        using EventQueue = std::deque<T>;

        // Returns global event queue
        template <class T>
        static EventQueue<T>& get()
        {
            static EventQueue<T> events;
            return events;
        }

        // Sends a global event
        // Events::send(Type(anything));
        template <class T>
        static void send(const T& event)
        {
            get<T>().push_back(event);
        }

        // Sends a global event (forwards arguments)
        // Events::send<Type>(anything);
        template <class T, class... Args>
        static void send(Args&&... args)
        {
            get<T>().emplace_back(std::forward<Args>(args)...);
        }

        // Returns true if there are any events of a certain type
        template <class T>
        static bool exists()
        {
            return !get<T>().empty();
        }

        template <class T>
        static void clear()
        {
            get<T>().clear();
        }
};

#endif
