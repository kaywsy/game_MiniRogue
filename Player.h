//
//  Player.h
//  project3
//
//  Created by Kay on 5/18/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#ifndef PLAYER_INCLUDED
#define PLAYER_INCLUDED

#include "Coord.h"
#include "Actor.h"

#include <vector>
#include <string>

class GameObject;

class Player : public Actor{
public:
    Player(Coord c);
    Player(Coord c, const Player &p);
    virtual ~Player();
    
    bool action(char input, std::string &outputMessage);
    void insertItem(GameObject* item);
    bool checkTeleport();
    
    void outputPlayerData() const;
    bool isDead();
    
private:
    bool isTeleported = false;
    std::vector<GameObject*> itemList;
    
    char openInventory() const;
    std::string useItem(int type);
    
};

inline
bool Player::isDead(){
    return Player::curHitPoints <= 0;
}

#endif
