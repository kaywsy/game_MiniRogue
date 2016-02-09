//
//  GameObject.cpp
//  project3
//
//  Created by Kay on 5/18/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#include "GameObject.h"
#include <iostream>

using namespace std;

enum WeaponType : int{ mace, shortSword, longSword, magicAxe, magicFangsOfSleep };
string weaponName[5] = { "mace", "short sword", "long sword", "magic axe", "magic fangs of sleep" };
enum ScrollType : int{ teleportation, improveArmor, raiseStrength, enhanceHealth, enhanceDexterity };
string scrollName[5] = { "scroll of teleportation", "scroll of improve armor", "scroll of raise strength", "scroll of enhance health", "scroll of enhance dexterity" };

// set the characters of each type of weapon
Weapon::Weapon(Coord c, int weaponType) : GameObject(c, weaponName[weaponType]){
    type = weaponType;
    
    switch (type){
        case mace:
            dexterityBonus = 0;
            damageAmount = 2;
            break;
        case shortSword:
            dexterityBonus = 0;
            damageAmount = 2;
            break;
        case longSword:
            dexterityBonus = 2;
            damageAmount = 4;
            break;
        case magicAxe:
            dexterityBonus = 5;
            damageAmount = 5;
            break;
        case magicFangsOfSleep:
            dexterityBonus = 3;
            damageAmount = 2;
            break;
    }
}

// help form the message displayed when one actor attacks another
string Weapon::action(){
    switch (type){
        case mace:
            return " swings mace at ";
        case shortSword:
            return " slashes short sword at ";
        case longSword:
            return " swings long sword at ";
        case magicAxe:
            return " chops magic axe at ";
        case magicFangsOfSleep:
            return " strikes magic fangs at ";
        default:
            return "";
    }
}


Scroll::Scroll(Coord c, int scrollType) : GameObject(c, scrollName[scrollType]){
    type = scrollType;
}

// help form the message displayed when the actor read a scroll
string Scroll::action(){
    switch (type){
        case teleportation:
            return "You feel your body wrenched in space and time.";
        case improveArmor:
            return "Your armor glows sliver.";
        case raiseStrength:
            return "You muscles bulge.";
        case enhanceHealth:
            return "You feel your heart beating stronger.";
        case enhanceDexterity:
            return "You feel like less of a klutz.";
        default:
            return "";
    }
}









