//
//  Game.h
//  project3
//
//  Created by Kay on 5/25/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#ifndef GAME_INCLUDED
#define GAME_INCLUDED

class Dungeon;
class Player;
// You may add data members and other member functions to this class.

class Game
{
public:
    Game(int goblinSmellDistance);
    void play();
private:
    Dungeon* g_dungeon;
};

#endif 
