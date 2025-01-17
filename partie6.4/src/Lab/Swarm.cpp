#include "Swarm.hpp"
#include "../Application.hpp"
#include "SwarmBacterium.hpp"
#include "Utility/MutableColor.hpp"
#include <utility>
#include "Utility/Utility.hpp"
#include <iostream>

using namespace std;

Swarm::Swarm(string id)
    : identificateur(id), leader(nullptr), directionPoison(Vec2d::fromRandomAngle()), timePoison(0.0)
{}

void Swarm::addSwarmBacterium(SwarmBacterium* bacterium)
{
    if(bacterium != nullptr) bacteries.push_back(bacterium);
    if (bacteries.size() == 1) leader = bacteries[0];
}


void Swarm::removeSwarmBacterium(const SwarmBacterium *bacterium)
{
    bacteries.erase(remove(bacteries.begin(), bacteries.end(), bacterium), bacteries.end());
}

Vec2d Swarm::getPositionLeader() const
{
    return leader->getPosition();
}

MutableColor Swarm::getOriginalColor() const
{
    return MutableColor(getConfig()["color"]);
}

void Swarm::update(sf::Time dt)
{
    if (not bacteries.empty()) {
        double leaderScore(leader->getScore());
        for (auto& b : bacteries) {
            double newScore(getAppEnv().getPositionScore(b->getPosition()));
            if (newScore > leaderScore) leader = b;
        }
    }
    if (bacteries.size() == 1) leader = bacteries[0];
    timePoison += dt.asSeconds();
    if(bacteries.size() >= getConfig()["size"].toDouble() and timePoison > getConfig()["poison time"].toDouble()){
        directionPoison = Vec2d::fromAngle(directionPoison.angle()+dt.asSeconds());
        getAppEnv().addPoison(new Poison(getPositionLeader(), getAppConfig()["swarms"]["poison radius"].toDouble(), directionPoison.normalised(), identificateur));
        timePoison = 0.0;
       }
}


string Swarm::getIdentificator() const
{
    return identificateur;
}

Swarm::~Swarm()
{
    for (auto& bacterie : bacteries) delete bacterie;
    bacteries.clear();
}

j::Value& Swarm::getConfig() const
{
    return getAppConfig()["swarms"][identificateur];
}
