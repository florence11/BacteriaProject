#include "Nutriment.hpp"
#include "../Application.hpp"
#include "Utility/Utility.hpp"
#include <string>
#include <cmath>

using namespace std;

Nutriment::Nutriment(Quantity rayon, Vec2d centre)
    : CircularBody(centre, rayon), quantite(rayon) {}

Quantity Nutriment::takeQuantity(Quantity q) {
    /* si la quantité demandée est inférieure ou egale à celle disponible
     * on la prélève et la renvoie
     *
     * sinon on prélève toute la quantité présente
     *
*/
    if(q<=quantite) {
        quantite -= q;
        return q;
    }
    else {
        q=quantite;
        quantite=0.0;
        return q;
    }
}

void Nutriment::setQuantity(Quantity q) {
    if(q>=0.0) {
        setRadius(q);
        quantite=q;
    }
}

void Nutriment::drawOn(sf::RenderTarget& target) const{
    auto const& texture = getAppTexture(getConfig()["texture"].toString());
    auto nutrimentSprite = buildSprite(getPosition(), 6, texture);
    // adapte la taille du Sprite au rayon du nutriment:
       nutrimentSprite.setScale(2 * getRadius()/ texture.getSize().x, 2 *getRadius()/ texture.getSize().y);
       target.draw(nutrimentSprite);

    if(isDebugOn()){
           int a(floor(quantite)); //affichage d'entier et pas de double
           sf::Text const text = buildText(to_string(a), getPosition(), getAppFont(), 15, sf::Color::Black);
           target.draw(text);
       }
}

j::Value const& Nutriment::getConfig() const {
    return getAppConfig()["nutriments"];
}
