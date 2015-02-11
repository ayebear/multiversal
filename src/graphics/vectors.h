#ifndef VECTORS_H
#define VECTORS_H

#include <cmath>

/*
Contains commonly used vector math.
*/

namespace vectors
{

template <typename T>
float length(const sf::Vector2<T>& vec)
{
    return sqrt(vec.x * vec.x + vec.y * vec.y);
}

template <typename T>
float distance(const sf::Vector2<T>& vecStart, const sf::Vector2<T>& vecEnd)
{
    return length(vecEnd - vecStart);
}

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

template <typename T>
sf::Vector2f normalized(sf::Vector2<T> vec)
{
	normalize(vec);
	return vec;
}

}

#endif
