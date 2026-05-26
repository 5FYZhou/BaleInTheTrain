#include <SFML/Graphics.hpp>

class Player{
private:
    sf::Vector2f pos;
    float speed;
public:
    Player(){
        pos.x = 10;  pos.y = 100;
        speed = 100;
    }

    void Move(int dir){
        pos.x += speed * dir;
    }

    sf::Vector2f GetPos(){ return pos; }
};