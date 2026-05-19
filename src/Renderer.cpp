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
}
