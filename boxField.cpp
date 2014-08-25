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
