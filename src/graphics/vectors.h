#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>
#include <SFML/System/Vector2.hpp>

/*
Contains commonly used vector math for SFML's 2D vectors.
*/

namespace vectors
{

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

}

#endif
