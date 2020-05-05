#include "ADN.hpp"
#include "../Application.hpp"
#include <SFML/Graphics.hpp>
#include "Utility/Utility.hpp"
#include "SimpleBacterium.hpp"
#include "SwarmBacterium.hpp"
#include "TwitchingBacterium.hpp"

ADN::ADN(double rayon, Vec2d centre) : CircularBody(centre, rayon)
{}

ADN::~ADN() {}

void ADN::drawOn(sf::RenderTarget &target) const
{
    auto const& texture = getAppTexture(getConfig()["texture"].toString());
    auto adnSprite = buildSprite(getPosition(), 6, texture);
    // adapte la taille du Sprite au rayon de l'ADN:
       adnSprite.setScale(2 * getRadius()/ texture.getSize().x, 2 *getRadius()/ texture.getSize().y);
       target.draw(adnSprite);
}

j::Value const& ADN::getConfig() const
{
    return getAppConfig()["ADN"];
}


void ADN::eatenBy(Bacterium &bact)
{
    bact.
}

void ADN::eatenBy(SimpleBacterium &bact)
{
    bact.mutate();
}

void ADN::eatenBy(SwarmBacterium &bact)
{
    bact.mutate();
}

void ADN::eatenBy(TwitchingBacterium &bact)
{
    bact.mutate();
}
