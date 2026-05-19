#include "Renderer.h"
#include "Scene.h"
#include <iomanip>

Renderer::Renderer(ResourceManager& r, int& g): rm(r), curGridSize(g){ }

void Renderer::Init(){
    sBackground.emplace(rm.getTexture(TextureType::BACKGROUND));
    sTiles.emplace(rm.getTexture(TextureType::GRID));
    sButtons.emplace(rm.getTexture(TextureType::BUTTONS));
    sNum.emplace(rm.getTexture(TextureType::NUM));
    sTimer.emplace(rm.getTexture(TextureType::TIMER));
    sCounter.emplace(rm.getTexture(TextureType::COUNTER));
    sGameOver.emplace(rm.getTexture(TextureType::GAMEOVER));
}

void Renderer::DrawBackground(RenderWindow& window, int index){
	if (index < 0 || index >= rm.getTextureCount(TextureType::BACKGROUND)) {
		std::cerr << "Invalid background index: " << index << std::endl;
		return;
	}
	sBackground->setTexture(rm.getTexture(TextureType::BACKGROUND, index));
	//绘制背景
	sBackground->setPosition({ 0.0, 0.0 });
	window.draw(*sBackground);
}

void Renderer::DrawGrid(RenderWindow& window, const Scene& grid){
    //float scale = 2.0f;
    //sTiles->setScale({scale, scale});
    //curGridSize = GRIDSIZE*2;
    // 计算网格的起始位置，使其居中显示
    gridStartPos.x = (window.getSize().x - grid.GetWidth() * curGridSize) / 2;
	gridStartPos.y = (window.getSize().y - grid.GetHeight() * curGridSize) / 2;
    // 绘制网格
    for (int i = 0; i < grid.GetHeight(); i++) {
        for (int j = 0; j < grid.GetWidth(); j++) {
            sTiles->setTextureRect(IntRect(
                { static_cast<int>(grid.GetCell(i, j)) * GRIDSIZE, 0 },
                { GRIDSIZE, GRIDSIZE }));

			sTiles->setPosition({
                static_cast<float>(gridStartPos.x + j * curGridSize),
                static_cast<float>(gridStartPos.y + i * curGridSize)
            });
			
            window.draw(*sTiles);
        }
    }
}

void Renderer::DrawButton(RenderWindow& window, 
		const Vector2i& LeftCorner, const int& detaX, const std::vector<Button_my>& btns){
	//颜色改回原色
	sButtons->setColor(Color(255, 255, 255, 255));
	
	int x = detaX;
	for(auto& b : btns){
		//读取按钮的纹理区域
		sButtons->setTextureRect(IntRect(
				{ static_cast<int>(b.type) * BTN_WIDTH, 0 }, 
				{ BTN_WIDTH, BTN_HEIGHT })
			);
		//设置按钮的位置坐标
		sButtons->setPosition({ 
			static_cast<float>(x),  
			static_cast<float>(LeftCorner.y) 
		});
	
		if (b.state == ButtonState::Hover)
			sButtons->setColor(Color(255, 255, 255, 128));	//设置阴影的颜色
		else
			sButtons->setColor(Color(255, 255, 255, 255));//颜色改回原色
		window.draw(*sButtons);
		x += detaX + BTN_WIDTH;
	}
}

void Renderer::DrawScore(RenderWindow& window, int score){
	Vector2i LeftCorner;
	LeftCorner.x = window.getSize().x - sCounter->getLocalBounds().size.x * 1.25;
	LeftCorner.y = sCounter->getLocalBounds().size.y * 0.5;
	sCounter->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });	//计数器纹理的贴图位置
	window.draw(*sCounter);

	int NumSize = sNum->getLocalBounds().size.y;
	LeftCorner.x = LeftCorner.x + sCounter->getLocalBounds().size.x - NumSize;
	LeftCorner.y = LeftCorner.y + sCounter->getLocalBounds().size.y * 0.5 - NumSize * 0.5;

	//int mScore = mMineNum - mFlagCalc;
	int mScore = score;
	//绘制个位数的数字
	int a = mScore % 10;
	sNum->setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(*sNum);
	//绘制十位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum->setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(*sNum);
	//绘制百位数的数字
	mScore = mScore / 10;
	a = mScore % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum->setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(*sNum);
}

void Renderer::DrawTimer(RenderWindow& window, float time){
	Vector2i LeftCorner;
	LeftCorner.x = sTimer->getLocalBounds().size.x * 0.25;
	LeftCorner.y = sTimer->getLocalBounds().size.y * 0.5;
	sTimer->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });	//计时器纹理的贴图位置
	window.draw(*sTimer);

	//if (isGameBegin)
	//	mTime = gameClock.getElapsedTime().asSeconds();

	int NumSize = sNum->getLocalBounds().size.y;
	LeftCorner.x = LeftCorner.x + sTimer->getLocalBounds().size.x - NumSize * 1.5;
	LeftCorner.y = LeftCorner.y + sTimer->getLocalBounds().size.y * 0.5 - NumSize * 0.5;

	int mTime = time;
	if (mTime > 999)
		mTime = 999;
	//绘制个位数的数字
	int a = mTime % 10;
	sNum->setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(*sNum);
	//绘制十位数的数字
	mTime = mTime / 10;
	a = mTime % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum->setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(*sNum);
	//绘制百位数的数字
	mTime = mTime / 10;
	a = mTime % 10;
	LeftCorner.x = LeftCorner.x - NumSize;
	sNum->setTextureRect(IntRect({ a * NumSize, 0 }, { NumSize, NumSize }));		//在贴图上取对应数字字符的纹理贴图
	sNum->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });
	window.draw(*sNum);
}

void Renderer::DrawGameEnd(RenderWindow& window, bool win){
	Vector2i LeftCorner;
	int ButtonWidth = 200;
	int ButtonHeight = sGameOver->getLocalBounds().size.y;
	LeftCorner.x = (window.getSize().x - ButtonWidth) / 2;
	LeftCorner.y = (window.getSize().y - ButtonHeight) / 2;

	sGameOver->setPosition({ static_cast<float>(LeftCorner.x), static_cast<float>(LeftCorner.y) });

	if (win)
		sGameOver->setTextureRect(IntRect({ 0 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));
	if (!win)
		sGameOver->setTextureRect(IntRect({ 1 * ButtonWidth, 0 }, { ButtonWidth, ButtonHeight }));

	window.draw(*sGameOver);
}