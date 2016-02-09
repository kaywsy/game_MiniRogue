//
//  Actor.cpp
//  project3
//
//  Created by Kay on 5/18/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#include "Actor.h"
#include "GameObject.h"
#include "utilities.h"

#include <iostream>
#include <algorithm>

using namespace std;

enum WeaponType : int{ mace, shortSword, longSword, magicAxe, magicFangsOfSleep };

// constructor
Actor::Actor(string name, Coord c, int hp, int ap, int sp, int dp)
:actorName(name), curCoord(c), curHitPoints(hp), maxHitPoints(hp), armorPoints(ap), strengthPoints(sp), dexterityPoints(dp), sleepTime(0)
{}

// copy constructor, for Player
Actor::Actor(const Actor &a)
:actorName(a.actorName), curCoord(a.curCoord), curHitPoints(a.curHitPoints), maxHitPoints(a.maxHitPoints), armorPoints(a.armorPoints), strengthPoints(a.strengthPoints), dexterityPoints(a.dexterityPoints), sleepTime(a.sleepTime)
{}

// destructor
Actor::~Actor(){
    if (wieldingWeapon != NULL)
        delete wieldingWeapon;
}

// set the actor's coordinate to c
void Actor::setCoord(Coord c){
    curCoord.x = c.x;
    curCoord.y = c.y;
}

// get the actor's coordinate
Coord Actor::getCoord() const{
    return curCoord;
}

// get the actor's wielding weapon
Weapon* Actor::getWieldingWeapon() const{
    return wieldingWeapon;
}

// change the actor's wielding weapon to another
void Actor::changeWieldingWeapon(Weapon* w){
    wieldingWeapon = w;
}

// get the actor's name
string Actor::getName() const{
    return actorName;
}

// if the actor is asleep, pass the turn and reduce the sleep time by 1
bool Actor::sleepPass(){
    if (sleepTime > 0){
        sleepTime --;
        return true;
    }
    else
        return false;
}

// for player and dragon
void Actor::regenerateHealth(){
    if (trueWithProbability(0.1)){
        if (curHitPoints < maxHitPoints)
            curHitPoints ++;
    }
}

// let the current actor attack an opponent
bool Actor::attack(Actor* opponent, string &message){
   // calculate the attackerPoints and the defenderPoints
   // to determine whether the attacker using that weapon hits the defender
    int attackerPoints = dexterityPoints + wieldingWeapon -> getWeaponDexterity();
    int defenderPoints = opponent -> dexterityPoints + opponent -> armorPoints;
    
    // if the attacker hits the defender
    // calculate the damage amount
    if (randInt(attackerPoints) >= randInt(defenderPoints)){
        int damagePoints = randInt(strengthPoints + wieldingWeapon -> getWeaponDamage());
        opponent -> curHitPoints -= damagePoints;
        
        // if the opponent's hit point is below 0
        // generate the corresponding message
        if (opponent -> curHitPoints <= 0){
            message = getName() + wieldingWeapon -> action() + opponent -> getName() + "dealing a final blow.";
            // return true indicates the attacker kills the opponent
            return true;
        }
        
        // if wielding magic fangs of sleep, the attacker can make the opponent sleep
        if (wieldingWeapon -> getWeaponType() == magicFangsOfSleep){
            if (trueWithProbability(0.2)){
                opponent -> sleepTime = max(opponent -> sleepTime, randInt(5) + 2);
                message = getName() + wieldingWeapon -> action() + opponent -> getName() + " and hits, putting " + opponent -> getName() + " to sleep";
                return false;
            }
        }
        
        message = getName() + wieldingWeapon->action() + opponent->getName() + " and hits.";
    }
    else
        message = getName() + wieldingWeapon->action() + opponent->getName() + " and misses.";
    return false;
    
}














