#include "boxField.hpp"

#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Minerake",
                            sf::Style::Titlebar | sf::Style::Close);

    BoxField boxField(20, window.getSize().x, window.getSize().y);

    window.setVerticalSyncEnabled(true);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "the left button was pressed on ("
                              << event.mouseButton.x << ","
                              << event.mouseButton.y << ")" << std::endl;

                    boxField.press(sf::Vector2u(event.mouseButton.x,
                                                event.mouseButton.y));
                }
                else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    if (boxField.isOver()) {
                        boxField.reset();
                    } else {
                        boxField.mark(sf::Vector2u(event.mouseButton.x,
                                                   event.mouseButton.y));
                    }
                }
                break;
            default:
                break;
            }
        }

        window.clear();

        boxField.draw(window);

        window.display();
    }

    return 0;
}
