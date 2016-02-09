//
//  Actor.h
//  project3
//
//  Created by Kay on 5/18/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include "Coord.h"
#include <string>

class Weapon;
class Dungeon;

class Actor{
public:
    Actor(std::string name, Coord c, int hp, int ap, int sp, int dp);
    Actor(const Actor &a);
    virtual ~Actor();
    
    void setCoord(Coord c);
    Coord getCoord() const;
    Weapon* getWieldingWeapon() const;
    void changeWieldingWeapon(Weapon* w);
    std::string getName() const;
    bool sleepPass();
    void regenerateHealth();
    bool attack(Actor* opponent, std::string &message);
    
    
protected:
    int curHitPoints;
    int maxHitPoints;
    int armorPoints;
    int strengthPoints;
    int dexterityPoints;
    int sleepTime;
    
private:
    Coord curCoord;
    Weapon* wieldingWeapon;
    std::string actorName;
    
};

#endif
