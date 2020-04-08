#include "Swarm.hpp"
#include "../Application.hpp"
#include "SwarmBacterium.hpp"
#include "Utility/MutableColor.hpp"
#include <utility>
#include "Utility/Utility.hpp"

using namespace std;

Swarm::Swarm(string id) : identificateur(id), leader(nullptr)
{}

void Swarm::addSwarmBacterium(SwarmBacterium* bacteria)
{
    if(bacteria != nullptr) bacteries.push_back(bacteria);
    if (bacteries.size() == 1) leader = bacteries[0];
}


void Swarm::removeSwarmBacterium(SwarmBacterium* bacteria)
{
    for(size_t i(0); i<bacteries.size(); ++i) {
        if (bacteria==bacteries[i]) { //FAUX ATTENTION !!!!
            swap(bacteries[i], bacteries.back());
            delete bacteries.back();
            bacteries.pop_back();
        }
    }
}

Vec2d Swarm::getPositionLeader() const
{
    return leader->getPosition();
}

sf::Color Swarm::getOriginalColor() const
{
    return MutableColor(getAppConfig()["swarms"][identificateur]["color"]).get();
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
}


string Swarm::getIdentificator() const
{
    return identificateur;
}

Swarm::~Swarm(){}
