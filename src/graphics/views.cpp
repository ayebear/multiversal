#include "views.h"

sf::Vector2f getViewPos(const sf::Vector2f& center, const sf::Vector2f& size)
{
    return sf::Vector2f(center.x - (size.x / 2.0f), center.y - (size.y / 2.0f));
}

sf::Vector2f getViewPos(const sf::View& view)
{
    return getViewPos(view.getCenter(), view.getSize());
}

sf::FloatRect getViewRect(const sf::View& view)
{
    const auto& size = view.getSize();
    auto pos = getViewPos(view.getCenter(), size);
    return sf::FloatRect(pos.x, pos.y, size.x, size.y);
}
