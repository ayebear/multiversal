// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef COMPONENTSTRINGS_H
#define COMPONENTSTRINGS_H

namespace ocs { class ObjectManager; }

#define BIND_COMP_STR(name) objects.bindStringToComponent<Components::name>(#name)

void bindComponentStrings(ocs::ObjectManager& objects);

#endif
