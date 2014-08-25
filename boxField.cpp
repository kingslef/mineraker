#include "boxField.hpp"

std::ostream & operator<<(std::ostream & out, const BoxField::Box & box)
{
    out << "(" << box.position.x << ", " << box.position.y << ")";
    return out;
}

BoxField::BoxField(const char * box_texture_file,
                   const char * pressed_box_texture_file,
                   unsigned int width,
                   unsigned int height)
{
    if (!box_texture.loadFromFile(box_texture_file)) {
        throw std::invalid_argument("Couldn't open the box texture");
    }

    if (!pressed_box_texture.loadFromFile(pressed_box_texture_file)) {
        throw std::invalid_argument("Couldn't open the pressed box "
                                    "texture");
    }

    box_sprite.setTexture(box_texture);
    pressed_box_sprite.setTexture(pressed_box_texture);

    // FIXME: remove scaling
    box_sprite.setScale(sf::Vector2f(.2f, .2f));
    pressed_box_sprite.setScale(sf::Vector2f(.2f, .2f));

    for (unsigned int i = 0; i < width; i++) {
        std::vector<Box> row;
        for (unsigned int j = 0; j < height; j++) {
            row.push_back(Box(i, j));
        }
        field.push_back(row);
    }
}

void BoxField::draw(sf::RenderWindow & window)
{
    float height = box_sprite.getGlobalBounds().height;
    float width = box_sprite.getGlobalBounds().width;

    for (auto & row : field) {
        for (auto & box : row) {
            sf::Vector2f pos(box.position.x * width, box.position.y * height);
            auto & sprite = (box.pressed ? pressed_box_sprite : box_sprite);
            sprite.setPosition(pos);
            window.draw(sprite);
        }
    }
}

void BoxField::press(const sf::Vector2u & position)
{
    unsigned int height = box_sprite.getGlobalBounds().height;
    unsigned int width = box_sprite.getGlobalBounds().width;

    unsigned int x = position.x / width
        + (position.x % width ? 1 : 0)
        - 1;
    unsigned int y = position.y / height
        + (position.y % height ? 1 : 0)
        - 1;

    std::cout << "box: (" << x << "," << y << ")" << std::endl;

    field[x][y].pressed = !field[x][y].pressed;
}

std::ostream & operator<<(std::ostream & out, const BoxField & boxField)
{
    for (auto & row : boxField.field) {
        for (auto & box : row) {
            out << box << ", ";
        }
        out << std::endl;
    }

    return out;
}
