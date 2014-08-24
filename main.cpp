#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minerake");

    sf::Texture texture;
    if (!texture.loadFromFile("box.png"))
    {
        return -1;
    }

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(sf::Vector2f(0.2f, .2f));

    sf::Vector2f startPosition = sprite.getPosition();
    float height = sprite.getGlobalBounds().height;
    float width = sprite.getGlobalBounds().width;

    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        sf::Vector2u windowSize = window.getSize();
        unsigned int windowX = windowSize.x;
        unsigned int windowY = windowSize.y;

        for (float x = startPosition.x; x < windowX; x += width)
        {
            for (float y = startPosition.y; y < windowY; y += height)
            {
                sprite.setPosition(sf::Vector2f(x, y));
                window.draw(sprite);
            }
        }

        window.display();
    }

    return 0;
}
