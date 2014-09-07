#include "boxField.hpp"

#include <random>

std::ostream & operator<<(std::ostream & out, const BoxField::Box & box)
{
    out << "(" << box.position.x << ", " << box.position.y << ")";
    return out;
}

BoxField::BoxField(const char * box_texture_file,
                   const char * pressed_box_texture_file,
                   const char * mine_texture_file,
                   unsigned int mines,
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

    if (!mine_texture.loadFromFile(mine_texture_file)) {
        throw std::invalid_argument("Couldn't open the mine texture");
    }

    box_sprite.setTexture(box_texture);
    pressed_box_sprite.setTexture(pressed_box_texture);
    mine_sprite.setTexture(mine_texture);

    // FIXME: remove scaling
    box_sprite.setScale(sf::Vector2f(.2f, .2f));
    pressed_box_sprite.setScale(sf::Vector2f(.2f, .2f));
    mine_sprite.setScale(sf::Vector2f(.2f, .2f));

    std::default_random_engine generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0,1);

    for (unsigned int i = 0; i < width; i++) {
        std::vector<Box> row;
        for (unsigned int j = 0; j < height; j++) {
            bool is_mine = mines ? distribution(generator) : false;
            if (is_mine) {
                std::cout << "mine in (" << i << "," << j << ")" << std::endl;
                mines--;
            }

            row.push_back(Box(i, j, is_mine));
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
            sf::Sprite sprite;
            if (box.mine && box.pressed) {
                sprite = mine_sprite;
            } else if (box.pressed) {
                sprite = pressed_box_sprite;
            } else {
                sprite = box_sprite;
            }

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
