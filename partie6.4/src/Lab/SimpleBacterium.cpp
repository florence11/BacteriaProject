#include "SimpleBacterium.hpp"
#include "../Application.hpp"
#include "Random/Random.hpp"
#include "Utility/Vec2d.hpp"
#include <cmath>
#include "Utility/Constants.hpp"
#include "Utility/Utility.hpp"
#include <iostream>
#include "Nutriment.hpp"
#include "NutrimentA.hpp"
#include "NutrimentB.hpp"
#include "ADN.hpp"

using namespace std;

double SimpleBacterium::compteur = 0.0;
double SimpleBacterium::Mbetter = 0.0;
double SimpleBacterium::Mworse = 0.0;
double SimpleBacterium::Mspeed = 0.0;

SimpleBacterium::SimpleBacterium(Vec2d position) //nrj, direction, rayon aléatoires
    : Bacterium(uniform(getConfig()["energy"]), position,
      Vec2d::fromRandomAngle(), uniform(getConfig()["radius"]),
      getConfig()["color"]),
      t(uniform(0.0, M_PI)), //compteur dessin flagelle
      dt(sf::Time::Zero),
      pBasculement(0.0)
{
    addProperty("speed", MutableNumber::positive(getAppConfig()["simple bacterium"]["speed"])); //vitesse
    addProperty("tumble better", MutableNumber::positive(getAppConfig()["simple bacterium"]["tumble"]["better"])); //lambda pour pBasculement mélioratif
    addProperty("tumble worse", MutableNumber::positive(getAppConfig()["simple bacterium"]["tumble"]["worse"])); //lambda pour basculement péjoratif
    compteur += 1;
}

SimpleBacterium::~SimpleBacterium()
{
    compteur -= 1;
    if(compteur == 0){
        Mbetter = 0.0;
        Mworse = 0.0;
        Mspeed = 0.0;
    } else {
        Mbetter = (Mbetter*(compteur+1) - getProperty("tumble better").get())/compteur;
        Mworse = (Mworse*(compteur+1) - getProperty("tumble worse").get())/compteur;
        Mspeed = ((Mspeed*(compteur+1)) - getProperty("speed").get())/compteur;
    }
}

j::Value& SimpleBacterium::getConfig() const
{
    return (getAppConfig()["simple bacterium"]);
}

Bacterium* SimpleBacterium::clone() const
{
    compteur += 1;
    Mbetter = ((Mbetter*(compteur-1)) + getProperty("tumble better").get())/compteur;
    Mworse = ((Mworse*(compteur-1)) + getProperty("tumble worse").get())/compteur;
    Mspeed = ((Mspeed*(compteur-1)) + getProperty("speed").get())/compteur;
    return new SimpleBacterium(*this);
}

Vec2d SimpleBacterium::f(Vec2d position, Vec2d speed) const
{
    return Vec2d(0,0); //force nulle sur les SimpleBacterium
}

Vec2d SimpleBacterium::getSpeedVector() const
{
    double factor(getProperty("speed").get());
    if (getEvolution()){
        factor *= ADN::getConfig()["speed"].toDouble();
        getProperty("speed").set(getProperty("speed").get()*ADN::getConfig()["speed"].toDouble());
    }
    return getDirection().normalised()*factor;
}

void SimpleBacterium::move(sf::Time dt)
{
    double ancien_score(getScore());

    //déplacement
    DiffEqResult resultat(stepDiffEq(getPosition(), getSpeedVector(), dt, *this, DiffEqAlgorithm::EC));
    Vec2d deplacement(resultat.position - this->getPosition());
    if(deplacement.lengthSquared() > 0.001){ //empêche tremblotements
        this->CircularBody::move(deplacement);
        double factor(getConsumption());
        double gain(1.0);
        if (getEvolution()){
            factor *= ADN::getConfig()["consumption"].toDouble();
        }
        if (getAbstinence()){
            gain = Lampe::getConfig()["consumption"].toDouble();
        }
        consumeEnergy(gain*factor*deplacement.length());
    }
    //La vitesse ne varie pas car la force est nulle
    //on utilise pas resultat.speed

    //Basculement
    this->dt += dt;
    double score(getScore()); //à la nouvelle position après le déplacement
    double lambda(getProperty("tumble better").get());
    if(score <= ancien_score){
        lambda = getProperty("tumble worse").get();
        }
    pBasculement = (1-(exp(-(this->dt.asSeconds())/lambda)));
    if(bernoulli(pBasculement)){
        basculement();
        }
}

void SimpleBacterium::basculement()
{
    if(getConfig()["tumble"]["algo"].toString()=="single random vector"){
        setDirection(getPosition().fromRandomAngle());
    } else if(getConfig()["tumble"]["algo"].toString()=="best of N"){
        newDirection();
    }
    dt = sf::Time::Zero;
}

void SimpleBacterium::drawFlagelle(sf::RenderTarget &targetWindow) const
{
    auto set_of_points = sf::VertexArray(sf::TrianglesStrip);
    set_of_points.append({{0,0}, getColor()});
    for(int i(1); i<30; ++i){
        float x = static_cast<float>(-i * getRadius() / 10.0);
        float y = static_cast<float>(getRadius() * sin(t) * sin(2 * i / 10.0));
        set_of_points.append({{x,y}, getColor()});
        set_of_points.append({{x+2,y+2}, getColor()});
    }
    auto transform = sf::Transform(); // déclare une matrice de transformation

    Vec2d posterieur(-getDirection().normalised()*getRadius()); //position de l'arrière bacterie
    transform.translate(getPosition()+posterieur);
    transform.rotate((getAngleDir())/DEG_TO_RAD);

    targetWindow.draw(set_of_points, transform);
}

void SimpleBacterium::drawOn(sf::RenderTarget& targetWindow) const
{
    drawFlagelle(targetWindow);
    Bacterium::drawOn(targetWindow);
}

void SimpleBacterium::updateFlagelle(sf::Time dt)
{
    t += 3*dt.asSeconds();

    auto const angleDiff = angleDelta(getDirection().angle(), getAngleDir());
    // calcule la différence entre le nouvel angle de direction et l'ancien
    auto dalpha = PI * dt.asSeconds(); // calcule dα
    dalpha = std::min(dalpha, std::abs(angleDiff)); // on ne peut tourner plus que de angleDiff
    dalpha = std::copysign(dalpha, angleDiff); // on tourne dans la direction indiquée par angleDiff
    setAngleDir(getAngleDir()+dalpha); // angle de rotation mis à jour

}

void SimpleBacterium::update(sf::Time dt)
{
    updateFlagelle(dt);
    Bacterium::update(dt);
    Mbetter = ((Mbetter*(compteur-1)) + getProperty("tumble better").get())/compteur;
    Mworse = ((Mworse*(compteur-1)) + getProperty("tumble worse").get())/compteur;
    Mspeed = ((Mspeed*(compteur-1)) + getProperty("speed").get())/compteur;
}



Quantity SimpleBacterium::eatableQuantity(NutrimentB& nutriment)
{
    return nutriment.eatenBy(*this);
}

Quantity SimpleBacterium::eatableQuantity(NutrimentA& nutriment)
{
    return nutriment.eatenBy(*this);
}

Quantity SimpleBacterium::getMaxEatableQuantity() const
{
    return (getConfig()["meal"]["max"].toDouble());
}

void SimpleBacterium::competence(ADN* adn)
{
    Bacterium::competence(adn);
    compteur += 1;
    Mbetter = ((Mbetter*(compteur-1)) + getProperty("tumble better").get())/compteur;
    Mworse = ((Mworse*(compteur-1)) + getProperty("tumble worse").get())/compteur;
    Mspeed = ((Mspeed*(compteur-1)) + getProperty("speed").get())/compteur;
    compteur -= 1;
    Mbetter = (Mbetter*(compteur+1) - getProperty("tumble better").get())/compteur;
    Mworse = (Mworse*(compteur+1) - getProperty("tumble worse").get())/compteur;
    Mspeed = ((Mspeed*(compteur+1)) - getProperty("speed").get())/compteur;
}

bool SimpleBacterium::infection()
{
    compteur += 1;
    Mbetter = ((Mbetter*(compteur-1)) + getProperty("tumble better").get())/compteur;
    Mworse = ((Mworse*(compteur-1)) + getProperty("tumble worse").get())/compteur;
    Mspeed = ((Mspeed*(compteur-1)) + getProperty("speed").get())/compteur;
    compteur -= 1;
    Mbetter = (Mbetter*(compteur+1) - getProperty("tumble better").get())/compteur;
    Mworse = (Mworse*(compteur+1) - getProperty("tumble worse").get())/compteur;
    Mspeed = ((Mspeed*(compteur+1)) - getProperty("speed").get())/compteur;
    return Bacterium::infection();
}

void SimpleBacterium::poisonned()
{
    if(getAppEnv().getPoisonColliding(*this) != nullptr) {
        getAppEnv().getPoisonColliding(*this)->setVanished(true);
        consumeEnergy(getConfig()["poison"].toDouble());
    }
}