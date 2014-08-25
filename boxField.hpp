#ifndef BOXFIELD_HPP
#define BOXFIELD_HPP

#include <SFML/Graphics.hpp>

#include <stdexcept>
#include <vector>
#include <iostream>

class BoxField {

public:
    BoxField(const char * box_texture_file,
             const char * pressed_box_texture_file,
             unsigned int width,
             unsigned int height);

    friend std::ostream & operator<<(std::ostream & out, const BoxField & field);

    class Box {

    public:
        Box(const sf::Vector2u & pos) : position(pos) {}
        Box(unsigned int x, unsigned int y) : position(sf::Vector2u(x, y)) {}

        friend std::ostream & operator<<(std::ostream & out, const Box & box);

    private:
        sf::Vector2u position;
        bool pressed = false;
        bool mine = false;
    };

private:

    sf::Sprite box_sprite;
    sf::Sprite pressed_box_sprite;

    sf::Texture box_texture;
    sf::Texture pressed_box_texture;

    std::vector<std::vector<BoxField::Box>> field;
};

#endif
