#include "Lab.hpp"
#include "../Application.hpp"
#include "../Utility/Utility.hpp"

using namespace std;

void Lab::drawOn(sf::RenderTarget &targetWindow) const {
        boite.drawOn(targetWindow);
}

void Lab::update(sf::Time dt) {
    boite.update(dt);
    automatique.update(dt); //nutrimentGenerator
}

Lab::Lab() :
    boite(getApp().getCentre(), 0.95*getApp().getLabSize().x/2.0)
{}

void Lab::reset(){
    automatique.reset();
    boite.reset();
}

bool Lab::contains(const CircularBody& corps) const {
    return boite.contains(corps);
}

void Lab::addNutriment(Nutriment* n){
    if (!boite.addNutriment(n)) delete n;
}//si pas possible d'ajouter alors supprimé de la zone mémoire

double Lab::getTemperature() const {
    return boite.getTemperature();
}

void Lab::increaseTemperature(){
    boite.increaseTemperature();
}

void Lab::decreaseTemperature(){
    boite.decreaseTemperature();
}

void Lab::resetTemperature() {
    boite.resetTemperature();
};


void Lab::addBacterium(Bacterium *b)
{
    if (!boite.addBacterium(b)) delete b;
} //si pas possible d'ajouter alors supprimé de la zone mémoire


bool Lab::doesCollideWithDish(CircularBody const& body) const
{
    return boite.isColliding(body);
}


Nutriment* Lab::getNutrimentColliding(CircularBody const& body)
{
    return boite.getNutrimentColliding(body);
}

