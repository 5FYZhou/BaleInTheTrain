#include "BattleRender.h"

void BattleRender::Init()
{
    layout.drawPilePos = {120.f, 930.f};

    layout.discardPilePos = {1800.f, 930.f};

    layout.actionPointPos = {350.f, 930.f};

    layout.handArea.position = {450.f, 820.f};
    layout.handArea.size     = {1000.f, 220.f};

    layout.cardWidth  = 160.f;
    layout.cardHeight = 220.f;

    layout.cardHoverOffset = 40.f;
}

void BattleRender::DrawHandCards(sf::RenderWindow& window,const BattleState& state)
{
    const auto& hand = state.handCards;

    if(hand.empty())
        return;

    hoveredCardIndex = -1;

    sf::Vector2i mousePixel =
        sf::Mouse::getPosition(window);

    sf::Vector2f mousePos(
        static_cast<float>(mousePixel.x),
        static_cast<float>(mousePixel.y)
    );

    const int cardCount =
        static_cast<int>(hand.size());

    float cardWidth  = layout.cardWidth;
    float cardHeight = layout.cardHeight;

    //------------------------------------
    // 自动计算间距
    //------------------------------------

    float spacing =
        cardWidth * 0.75f;

    float totalWidth =
        spacing * (cardCount - 1) +
        cardWidth;

    if(totalWidth > layout.handArea.size.x)
    {
        spacing =
            (layout.handArea.size.x - cardWidth) /
            std::max(1, cardCount - 1);

        totalWidth =
            spacing * (cardCount - 1) +
            cardWidth;
    }

    //------------------------------------
    // 居中
    //------------------------------------

    float startX =
        layout.handArea.position.x +
        (layout.handArea.size.x - totalWidth) * 0.5f;

    //------------------------------------
    // 第一遍：检测Hover
    //------------------------------------

    for(int i=0;i<cardCount;i++)
    {
        float x =
            startX + i * spacing;

        float y =
            layout.handArea.position.y;

        sf::FloatRect cardRect(
            {x,y},
            {cardWidth,cardHeight}
        );

        if(cardRect.contains(mousePos))
        {
            hoveredCardIndex = i;
        }
    }

    //------------------------------------
    // 第二遍：绘制
    //------------------------------------

    for(int i=0;i<cardCount;i++)
    {
        const Card& card = hand[i];

        float x =
            startX + i * spacing;

        float y =
            layout.handArea.position.y;

        bool hovered =
            (i == hoveredCardIndex);

        //--------------------------------
        // Hover效果
        //--------------------------------

        float scale = 1.f;

        if(hovered)
        {
            y -= layout.cardHoverOffset;
            scale = 1.15f;
        }

        //--------------------------------
        // 卡牌底板
        //--------------------------------

        sf::RectangleShape cardShape;

        cardShape.setSize(
        {
            cardWidth,
            cardHeight
        });

        cardShape.setPosition({x,y});

        cardShape.setOrigin(
        {
            cardWidth*0.5f,
            cardHeight*0.5f
        });

        cardShape.setPosition(
        {
            x + cardWidth*0.5f,
            y + cardHeight*0.5f
        });

        cardShape.setScale(
        {
            scale,
            scale
        });

        cardShape.setFillColor(
            sf::Color(230,230,230)
        );

        cardShape.setOutlineThickness(2.f);

        cardShape.setOutlineColor(
            hovered
            ? sf::Color::Yellow
            : sf::Color::Black
        );

        window.draw(cardShape);

        //--------------------------------
        // 费用
        //--------------------------------

        sf::Text cost(font);

        cost.setString(
            std::to_string(card.cost)
        );

        cost.setCharacterSize(28);

        cost.setPosition(
        {
            x + 10.f,
            y + 5.f
        });

        cost.setFillColor(
            sf::Color::Blue
        );

        window.draw(cost);

        //--------------------------------
        // 名称
        //--------------------------------

        sf::Text name(font);

        name.setString(card.name);

        name.setCharacterSize(18);

        name.setPosition(
        {
            x + 15.f,
            y + 45.f
        });

        name.setFillColor(
            sf::Color::Black
        );

        window.draw(name);

        //--------------------------------
        // 效果数值
        //--------------------------------

        sf::Text value(font);

        if(card.damage > 0)
        {
            value.setString(
                "ATK "
                + std::to_string(card.damage)
            );
        }
        else if(card.block > 0)
        {
            value.setString(
                "DEF "
                + std::to_string(card.block)
            );
        }

        value.setCharacterSize(22);

        value.setPosition(
        {
            x + 15.f,
            y + 90.f
        });

        value.setFillColor(
            sf::Color::Red
        );

        window.draw(value);
    }
}