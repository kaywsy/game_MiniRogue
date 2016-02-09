//
//  GameObject.h
//  project3
//
//  Created by Kay on 5/18/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#ifndef GAMEOBJECT_INCLUDED
#define GAMEOBJECT_INCLUDED

#include "Coord.h"
#include <string>

class GameObject{
public:
    GameObject(Coord c, std::string name);
    virtual ~GameObject();
    std::string getName() const;
    Coord getCoord() const;
    
private:
    Coord curCoord;
    std::string gameObjectName;
};

class Weapon : public GameObject{
public:
    Weapon(Coord c, int weaponType);
    virtual ~Weapon();
    std::string action();
    int getWeaponType() const;
    int getWeaponDexterity() const;
    int getWeaponDamage() const;
    
private:
    int type;
    int dexterityBonus;
    int damageAmount;
};

class Scroll : public GameObject{
public:
    Scroll(Coord c, int scrollType);
    virtual ~Scroll();
    std::string action();
    int getScrollType() const;
private:
    int type;
};

inline
GameObject::GameObject(Coord c, std::string name) : curCoord(c), gameObjectName(name)
{}

inline
GameObject::~GameObject()
{}

inline
std::string GameObject::getName() const{
    return gameObjectName;
}

inline
Coord GameObject::getCoord() const{
    return curCoord;
}

inline
int Weapon::getWeaponType() const{
    return type;
}

inline
int Weapon::getWeaponDexterity() const{
    return dexterityBonus;
}

inline
int Weapon::getWeaponDamage() const{
    return damageAmount;
}

inline
Weapon::~Weapon()
{}

inline
int Scroll::getScrollType() const{
    return type;
}

inline
Scroll::~Scroll()
{}





#endif
