// Copyright (C) 2014-2015 Eric Hebert (ayebear)
// This code is licensed under GPLv3, see LICENSE.txt for details.

#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>
#include <SFML/System/Vector2.hpp>

/*
Contains commonly used vector math for SFML's 2D vectors.
*/

namespace vectors
{

const double PI = 3.141592653589793238463;

// Calculates the length of a vector
template <typename T>
float length(const sf::Vector2<T>& vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

// Calculates the distance between two vectors.
template <typename T>
float distance(const sf::Vector2<T>& vecStart, const sf::Vector2<T>& vecEnd)
{
    return length(vecEnd - vecStart);
}

// Makes the magnitude of a vector 1 (makes it a unit vector).
template <typename T>
T normalize(sf::Vector2<T>& vec)
{
	auto len = length(vec);
	if (len != 0)
	{
        vec.x /= len;
        vec.y /= len;
	}
	return len;
}

// Returns the unit vector of a vector.
template <typename T>
sf::Vector2f normalized(sf::Vector2<T> vec)
{
	normalize(vec);
	return vec;
}

template <typename T1, typename T2>
sf::Vector2<T1> cast(const sf::Vector2<T2>& vec)
{
    return sf::Vector2<T1>(static_cast<T1>(vec.x), static_cast<T1>(vec.y));
}

template <typename T>
double getAngle(const sf::Vector2<T>& vec)
{
    return (atan2(vec.y, vec.x) * (180.0 / PI));
}

}

#endif
