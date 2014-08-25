#include "boxField.hpp"

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

    std::cout << "creating boxField" << std::endl;
    BoxField boxField("box.png", "box_pressed.png",
                      window.getSize().x / sprite.getGlobalBounds().width,
                      window.getSize().y / sprite.getGlobalBounds().height);
    std::cout << boxField << std::endl;

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

        boxField.draw(window);

        window.display();
    }

    return 0;
}
