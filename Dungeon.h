//
//  Dungeon.h
//  project3
//
//  Created by Kay on 5/18/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#ifndef DUNGEON_INCLUDED
#define DUNGEON_INCLUDED

#include "Coord.h"
#include <vector>

class Player;
class GameObject;
class Actor;

enum Direction :int{ north, west, south, east };

class Dungeon{
public:
    Dungeon(int level, int smellDistance, Player* prevPlayer);
    ~Dungeon();
    
    void printDungeon();
    int performCommand(const char input);
    void monsterAction();
    
    int getGoblinSmellDistance() const;
    void setFloor(Coord c, char floorType);
    char getFloor(Coord c) const;
    
    int getLevel() const;
    Player* getPlayer() const;
    bool canWalk(Coord c) const;
    bool hasItem(Coord c) const;
    
private:
    // members
    int dungeonLevel;
    int goblinSmellDistance;
    char dungeonMatrix[70][18];
    Player* dungeonPlayer;
    std::vector<GameObject*> dungeonItems;
    std::vector<Actor*> dungeonMonsters;
    
    std::vector<std::string> message;
    
    // functions to set up the dungeon maze
    bool generateDungeon();
    void generateInitialItems();
    void generateStairOrIdol();
    void generateMonsters();
    void generatePlayer(Player* dPlayer);
    
    // help functions to generate the dungeon
    bool makeRoom(Coord startCoord, int height, int width, std::vector<Coord> &borderIndex);
    int makeCorridor(Coord startCoord, int length, Direction dir, std::vector<Coord> &corridorEndIndex);
    void findFloor(Coord &coord, std::vector<Coord> &indexVector, Direction& dir);
    void pushbackCoord(Coord coord, std::vector<Coord> &borderIndex, Direction dir);
    
    bool generateItem(Coord coordinate, int itemType, int type);
    
    // help funtions to generate loots
    void dropItem(Actor* deadMonster);
    
};

inline
int Dungeon::getGoblinSmellDistance() const{
    return goblinSmellDistance;
}

inline
void Dungeon::setFloor(Coord c, char floorType){
    dungeonMatrix[c.x][c.y] = floorType;
}

inline
char Dungeon::getFloor(Coord c) const{
    return dungeonMatrix[c.x][c.y];
}

inline
int Dungeon::getLevel() const{
    return dungeonLevel;
}

inline
Player* Dungeon::getPlayer() const{
    return dungeonPlayer;
}

#endif 
