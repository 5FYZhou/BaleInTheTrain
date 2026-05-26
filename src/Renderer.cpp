#include "Renderer.h"
#include "Scene.h"
#include <iomanip>

Renderer::Renderer(ResourceManager& r): rm(r){ }

void Renderer::Init(){
    sBackground.emplace(rm.getTexture(TextureType::BACKGROUND));
    sTiles.emplace(rm.getTexture(TextureType::GRID));
    sButtons.emplace(rm.getTexture(TextureType::BUTTONS));
    sNum.emplace(rm.getTexture(TextureType::NUM));
    sTimer.emplace(rm.getTexture(TextureType::TIMER));
    sCounter.emplace(rm.getTexture(TextureType::COUNTER));
    sGameOver.emplace(rm.getTexture(TextureType::GAMEOVER));
    sPlayer.emplace(rm.getTexture(TextureType::Player));
    sCard.emplace(rm.getTexture(TextureType::Card));
}

void Renderer::DrawPlayer(RenderWindow& window, sf::Vector2f pos){
    sPlayer->setTexture(rm.getTexture(TextureType::Player));
	sPlayer->setPosition(pos);
	window.draw(*sPlayer);
}

void Renderer::DrawCard(RenderWindow& window, std::vector<sf::Vector2f> poss){
    sCard->setTexture(rm.getTexture(TextureType::Card));
    for(auto& pos : poss){
	    sCard->setPosition(pos);
	    window.draw(*sCard);
    }
}
