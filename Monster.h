//
//  Bogeymen.h
//  project3
//
//  Created by Kay on 5/24/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#ifndef MONSTER_INCLUDED
#define MONSTER_INCLUDED

#include "Actor.h"
#include "Coord.h"
#include <vector>

class Dungeon;

class Bogeymen : public Actor{
public:
    Bogeymen(Coord c);
    virtual ~Bogeymen();
    
    void action(Coord playerCoord, Dungeon* dungeon, std::string &outputMessage);
};

class Snakewomen : public Actor{
public:
    Snakewomen(Coord c);
    virtual ~Snakewomen();
    
    void action(Coord playerCoord, Dungeon* dungeon, std::string &outputMessage);
};

class Dragons : public Actor{
public:
    Dragons(Coord c);
    virtual ~Dragons();
    
    void action(Coord playerCoord, Dungeon* dungeon, std::string &outputMessage);
};

class Goblins : public Actor{
public:
    Goblins(Coord c, int smell_dist);
    virtual ~Goblins();
    
    void action(Coord playerCoord, Dungeon* dungeon, std::string &outputMessage);
    
private:
    int smellDistance;  // furtherest distance that a goblin can smell the player
    int shortestDistance;   // record the shortest distance to the player
    int bestPossibleDistance;   // the best possible distance is the Norm-1 distance of two actors
    bool findShortestPath(Coord startCoord, Coord destCoord, Coord nextMove, std::vector<Coord>& path, int depth, Dungeon* dungeon);
    
};

inline
Bogeymen::~Bogeymen()
{}

inline
Snakewomen::~Snakewomen()
{}

inline
Dragons::~Dragons()
{}

inline
Goblins::~Goblins()
{}

#endif
