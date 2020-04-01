#include "SimpleBacterium.hpp"
#include "../Application.hpp"
#include "Random/Random.hpp"
#include "Utility/Vec2d.hpp"
#include <cmath>
#include "Utility/Constants.hpp"
#include "Utility/Utility.hpp"
#include <iostream>

using namespace std;


j::Value& SimpleBacterium::getConfig() const
{
    return (getAppConfig()["simple bacterium"]);
}

Bacterium* SimpleBacterium::clone() const
{
    return new SimpleBacterium(*this);
}


SimpleBacterium::SimpleBacterium(Vec2d position)
    : Bacterium(uniform(getConfig()["energy"]["min"].toDouble(),getConfig()["energy"]["max"].toDouble()), position,
      Vec2d::fromRandomAngle(), uniform(getConfig()["radius"]["min"].toDouble(), getConfig()["radius"]["max"].toDouble()),
      getConfig()["color"]),
      t(uniform(0.0, M_PI)),
      dt(sf::Time::Zero)
{
    addProperty("speed", MutableNumber::positive(getAppConfig()["simple bacterium"]["speed"])); //vitesse
    addProperty("tumble better", MutableNumber::positive(getAppConfig()["simple bacterium"]["tumble"]["better"])); //lambda pour pBasculement mélioratif
    addProperty("tumble worse", MutableNumber::positive(getAppConfig()["simple bacterium"]["tumble"]["worse"])); //lambda pour basculement péjoratif
}

SimpleBacterium::~SimpleBacterium() {}

Vec2d SimpleBacterium::f(Vec2d position, Vec2d speed) const
{
    return Vec2d(0,0);
}

Vec2d SimpleBacterium::getSpeedVector() const
{
    return getDirection().normalised()*getProperty("speed").get();
}

void SimpleBacterium::move(sf::Time dt)
{
    double ancien_score(getScore());
    this->dt += dt;
    DiffEqResult resultat(stepDiffEq(getPosition(), getSpeedVector(), dt, *this, DiffEqAlgorithm::EC));
    Vec2d length(resultat.position - this->getPosition());
    if(length.lengthSquared() > 0.001){
        this->CircularBody::move(length);
        consumeEnergy(getConsumption()*length.length());
    }
    //La vitesse ne varie pas car la force est nulle

    //Basculement
    double score(getScore());
    double lambda(getProperty("tumble better").get());
    if(score<=ancien_score){
        lambda = getProperty("tumble worse").get();
        }
    setpBasculement(1-(exp(-(this->dt.asSeconds())/lambda)));
    if(bernoulli(getpBasculement())){
        basculement();}

}

void SimpleBacterium::basculement()
{
    if(getConfig()["tumble"]["algo"].toString()=="single random vector"){
        setDirection(Vec2d::fromRandomAngle());
    } else if(getConfig()["tumble"]["algo"].toString()=="best of N"){
        Vec2d dir(Vec2d::fromRandomAngle());
        for(int i(0); i<20; ++i){
            Vec2d dir2(Vec2d::fromRandomAngle());
            if(getAppEnv().getPositionScore(dir+getPosition()) < getAppEnv().getPositionScore(dir2+getPosition())){
                dir = dir2;
            }
        }
        setDirection(dir);
    }
    dt = sf::Time::Zero;
}

void SimpleBacterium::drawFlagelle(sf::RenderTarget &targetWindow) const
{
    auto set_of_points = sf::VertexArray(sf::TrianglesStrip);
    set_of_points.append({{0,0}, sf::Color::Black});
    for(int i(1); i<30; ++i){
        float x = static_cast<float>(-i * getRadius() / 10.0);
        float y = static_cast<float>(getRadius() * sin(t) * sin(2 * i / 10.0));
        set_of_points.append({{x,y}, sf::Color::Black});
        set_of_points.append({{x+1,y+1}, sf::Color::Black});
    }
    auto transform = sf::Transform(); // déclare une matrice de transformation

    Vec2d posterieur(-getDirection().normalised()*getRadius());
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
}
