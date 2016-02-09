//
//  Bogeymen.cpp
//  project3
//
//  Created by Kay on 5/24/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#include "Monster.h"
#include "GameObject.h"
#include "Dungeon.h"
#include "utilities.h"
#include <cstdlib>

using namespace std;

const char TMP_WALL = '!';
const char FINAL_ROOM = ' ';

enum WeaponType : int{ mace, shortSword, longSword, magicAxe, magicFangsOfSleep };

Bogeymen::Bogeymen(Coord c) : Actor("Bogeyman", c, randInt(6) + 5, 2, randInt(2) + 2, randInt(2) + 2){
    changeWieldingWeapon(new Weapon(c, shortSword));
}

// actions performed by bogeymen
void Bogeymen::action(Coord playerCoord, Dungeon* dungeon, string &outputMessage){
    // if asleep, pass the turn
    if (sleepPass())
        return;
    
    int dist = abs(getCoord().x - playerCoord.x) + abs(getCoord().y - playerCoord.y);
    // if distance <= 5, try to reach player
    if (dist <= 5){
        // if distance is 1, can reach player, attack
        if (dist == 1){
            attack((Actor*)(dungeon -> getPlayer()), outputMessage);
        }
        else{
            // either close in by row or by column
            if (getCoord().x < playerCoord.x){
                if (dungeon -> canWalk(Coord(getCoord().x + 1, getCoord().y))){
                    setCoord(Coord(getCoord().x + 1, getCoord().y));
                    return;
                }
            }
            else{
                if (getCoord().x > playerCoord.x){
                    if (dungeon -> canWalk(Coord(getCoord().x - 1, getCoord().y))){
                        setCoord(Coord(getCoord().x - 1, getCoord().y));
                        return;
                    }
                }
            }
            
            if (getCoord().y < playerCoord.y){
                if (dungeon -> canWalk(Coord(getCoord().x, getCoord().y + 1))){
                    setCoord(Coord(getCoord().x, getCoord().y + 1));
                    return;
                }
            }
            else{
                if (getCoord().y > playerCoord.y){
                    if (dungeon -> canWalk(Coord(getCoord().x, getCoord().y - 1))){
                        setCoord(Coord(getCoord().x, getCoord().y - 1));
                                 return;
                    }
                }
            }
        }
    }
}

Snakewomen::Snakewomen(Coord c) : Actor("Snakewoman", c, randInt(4) + 3, 3, 2, 3){
    changeWieldingWeapon(new Weapon(c, magicFangsOfSleep));
}

// actions performed by snakewomen
void Snakewomen::action(Coord playerCoord, Dungeon* dungeon, string &outputMessage){
    // if asleep, pass the turn
    if (sleepPass())
        return;
    
    int dist = abs(getCoord().x - playerCoord.x) + abs(getCoord().y - playerCoord.y);
    // if distance <= 3, try to reach player
    if (dist <= 3){
        // if distance is 1, can reach player, attack
        if (dist == 1){
            attack((Actor*)(dungeon -> getPlayer()), outputMessage);
        }
        else{
            // either close in by row or by column
            if (getCoord().x < playerCoord.x){
                if (dungeon -> canWalk(Coord(getCoord().x + 1, getCoord().y))){
                    setCoord(Coord(getCoord().x + 1, getCoord().y));
                    return;
                }
            }
            else{
                if (getCoord().x > playerCoord.x){
                    if (dungeon -> canWalk(Coord(getCoord().x - 1, getCoord().y))){
                        setCoord(Coord(getCoord().x - 1, getCoord().y));
                        return;
                    }
                }
            }
            
            if (getCoord().y < playerCoord.y){
                if (dungeon -> canWalk(Coord(getCoord().x, getCoord().y + 1))){
                    setCoord(Coord(getCoord().x, getCoord().y + 1));
                    return;
                }
            }
            else{
                if (getCoord().y > playerCoord.y){
                    if (dungeon -> canWalk(Coord(getCoord().x, getCoord().y - 1))){
                        setCoord(Coord(getCoord().x, getCoord().y - 1));
                        return;
                    }
                }
            }
        }
    }
}

Dragons::Dragons(Coord c) : Actor("Dragon", c, randInt(6) + 20, 4, 4, 4){
    changeWieldingWeapon(new Weapon(c, longSword));
}

// actions performed by dragons
void Dragons::action(Coord playerCoord, Dungeon* dungeon, string &outputMessage){
    // possoble to regenerate health
    regenerateHealth();
    
    // if asleep, pass the turn
    if (sleepPass())
        return;
    
    // never moves, attacks when it can reach player
    int dist = abs(getCoord().x - playerCoord.x) + abs(getCoord().y - playerCoord.y);
    if (dist == 1){
        attack((Actor*)(dungeon -> getPlayer()), outputMessage);
    }
}

Goblins::Goblins(Coord c, int smell_dist) : Actor("Goblin", c, randInt(6) + 15, 1, 3, 1){
    smellDistance = smell_dist;
    shortestDistance = INT_MAX;
    bestPossibleDistance = INT_MAX;
    changeWieldingWeapon(new Weapon(c, shortSword));
}

// actions performed by goblins
void Goblins::action(Coord playerCoord, Dungeon* dungeon, string &outputMessage){
    // if asleep, pass the turn
    if (sleepPass())
        return;
    
    // find the current best possible distance in order to exit
    bestPossibleDistance = abs(getCoord().x - playerCoord.x) + abs(getCoord().y - playerCoord.y);
    
    // if can smell player, try to find the optimal path
    if (bestPossibleDistance <= smellDistance){
        // if can reach player, attack
        if (bestPossibleDistance == 1){
            attack((Actor*)(dungeon -> getPlayer()), outputMessage);
        }
        else{
            vector<Coord> minPath;
            Coord nextMove;
            shortestDistance = INT_MAX;
            if (findShortestPath(getCoord(), playerCoord, Coord(0, 0), minPath, 0, dungeon)){
                setCoord(Coord(getCoord().x + minPath[minPath.size() - 1].x, getCoord().y + minPath[minPath.size() - 1].y));
            }
        }
    }
}

// recursively find the shortest path
bool Goblins::findShortestPath(Coord startCoord, Coord destCoord, Coord nextMove, vector<Coord> &path, int depth, Dungeon *dungeon){
    // if can't go through, return false
    if (dungeon -> getFloor(startCoord) != FINAL_ROOM)
        return false;
    
    // if reached destination, current distance is shorter, return true, else return false
    if (startCoord == destCoord){
        if (shortestDistance > depth){
            shortestDistance = depth;
            path.push_back(nextMove);
            return true;
        }
        else
            return false;
    }
    
    // make sure no circle exists
    dungeon -> setFloor(startCoord, TMP_WALL);
    
    // find the current distance
    int dist = abs(destCoord.x - startCoord.x) + abs(destCoord.y - startCoord.y);
    
    // if player out of reach, can't be smelled, return false
    if (dist + depth > smellDistance){
        dungeon -> setFloor(startCoord, FINAL_ROOM);
        return false;
    }
    
    // recursively find the best path from east, west, south and north
    bool hasEast = false, hasWest = false, hasSouth = false, hasNorth = false;
    
    // if the shortest distance is already the best possible, no need to perform recursion, return true
    // since each recursive performance may find the best result, have to check for each recursion
    if (bestPossibleDistance != shortestDistance){
        hasEast = findShortestPath(Coord(startCoord.x + 1, startCoord.y), destCoord, Coord(1, 0), path, depth + 1, dungeon);
    }
    else{
        if (depth != 0)
            path.push_back(nextMove);
        dungeon -> setFloor(startCoord, FINAL_ROOM);
        return true;
    }
    
    if (bestPossibleDistance != shortestDistance){
        hasWest = findShortestPath(Coord(startCoord.x - 1, startCoord.y), destCoord, Coord(-1, 0), path, depth + 1, dungeon);
    }
    else{
        if (depth != 0)
            path.push_back(nextMove);
        dungeon -> setFloor(startCoord, FINAL_ROOM);
        return true;
    }
    
    if (bestPossibleDistance != shortestDistance){
        hasSouth = findShortestPath(Coord(startCoord.x, startCoord.y + 1), destCoord, Coord(0, 1), path, depth + 1, dungeon);
    }
    else{
        if (depth != 0)
            path.push_back(nextMove);
        dungeon -> setFloor(startCoord, FINAL_ROOM);
        return true;
    }
    
    if (bestPossibleDistance != shortestDistance){
        hasNorth = findShortestPath(Coord(startCoord.x, startCoord.y - 1), destCoord, Coord(0, -1), path, depth + 1, dungeon);
    }
    else{
        if (depth != 0)
            path.push_back(nextMove);
        dungeon -> setFloor(startCoord, FINAL_ROOM);
        return true;
    }
    
    // if found a possible optimal path, push the step into the vector
    // when depth = 0, the step is (0, 0), no need to push it in
    if ((hasEast || hasWest || hasSouth || hasNorth) && depth != 0){
        path.push_back(nextMove);
    }
    
    // change the floor back so that other paths may pass it
    dungeon -> setFloor(startCoord, FINAL_ROOM);
    
    return hasEast || hasWest || hasSouth || hasNorth;
 
}
















