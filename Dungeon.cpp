//
//  Dungeon.cpp
//  project3
//
//  Created by Kay on 5/18/15.
//  Copyright (c) 2015 Kay. All rights reserved.
//

#include "Dungeon.h"
#include "utilities.h"
#include "Player.h"
#include "Monster.h"
#include "GameObject.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;

//#################### Symbol Set Constant ####################
const char WALL = '#';
const char ROOM = '1';
const char BORDER = '2';
const char END_OF_CORRIDOR = 'E';
const char FINAL_ROOM = ' ';

const char STAIR = '<';
const char GOLDEN_IDOL = '&';
const char WEAPON = ')';
const char SCROLL = '?';

const char SNAKEWOMEN = 'S';
const char GOBLINS = 'G';
const char BOGEYMEN = 'B';
const char DRAGONS = 'D';

const char PLAYER = '@';

//#################### Value Constant ####################
const int MIN_NUMBER_OF_ROOM = 4;
const int MAX_NUMBER_OF_ROOM = 7;

const int MIN_ROOM_WIDTH = 4;
const int MIN_ROOM_HEIGHT = 4;
const int MAX_ROOM_WIDTH = 14;
const int MAX_ROOM_HEIGHT = 12;
const int MIN_CORRIDOR_LENGTH = 2;
const int MAX_CORRIDOR_LENGTH = 10;

const int MAX_ARRAY_WIDTH = 70;
const int MAX_ARRAY_HEIGHT = 18;

const int MAX_FAIL_COUNT = 100;

//#################### Enumeration List ####################
enum itemType :int{weapon, scroll};
enum WeaponType : int{ mace, shortSword, longSword, magicAxe, magicFangsOfSleep };
enum ScrollType : int { teleporation, improveArmor, raiseStrength, enhanceHealth, enhanceDexterity };
enum MonsterType : int{ dragons, bogeymen, snakewomen, goblins };

Dungeon::Dungeon(int level, int smellDistance, Player* prevPlayer = NULL){
    dungeonLevel = level;
    goblinSmellDistance = smellDistance;
    
    // Dungeon generation is too random and has to be dealt with specially
    while(!generateDungeon());
    
    generateInitialItems();
    generateStairOrIdol();
    generatePlayer(prevPlayer);
    generateMonsters();
}

Dungeon::~Dungeon(){
    while (!dungeonItems.empty()){
        delete dungeonItems[dungeonItems.size() - 1];
        dungeonItems.pop_back();
    }
}

// print out the dungeon to the screen
void Dungeon::printDungeon(){
    for (unsigned int i = 0; i < dungeonItems.size(); i++){
        Weapon* wptr = dynamic_cast<Weapon*>(dungeonItems[i]);
        if (wptr != NULL){
            dungeonMatrix[dungeonItems[i]->getCoord().x][dungeonItems[i]->getCoord().y] = WEAPON;
        }
        else{
            Scroll* sptr = dynamic_cast<Scroll*>(dungeonItems[i]);
            if (sptr != NULL){
                dungeonMatrix[dungeonItems[i]->getCoord().x][dungeonItems[i]->getCoord().y] = SCROLL;
            }
            else{
                if (dungeonItems[i]->getName() == "Stair"){
                    dungeonMatrix[dungeonItems[i]->getCoord().x][dungeonItems[i]->getCoord().y] = STAIR;
                }
                else{
                    dungeonMatrix[dungeonItems[i]->getCoord().x][dungeonItems[i]->getCoord().y] = GOLDEN_IDOL;
                }
            }
        }
    }
    
    for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
        if (dungeonMonsters[i]->getName() == "Snakewoman"){
            dungeonMatrix[dungeonMonsters[i]->getCoord().x][dungeonMonsters[i]->getCoord().y] = SNAKEWOMEN;
        }
        else{
            if (dungeonMonsters[i]->getName() == "Bogeyman"){
                dungeonMatrix[dungeonMonsters[i]->getCoord().x][dungeonMonsters[i]->getCoord().y] = BOGEYMEN;
            }
            else{
                if (dungeonMonsters[i]->getName() == "Goblin"){
                    dungeonMatrix[dungeonMonsters[i]->getCoord().x][dungeonMonsters[i]->getCoord().y] = GOBLINS;
                }
                else{
                    dungeonMatrix[dungeonMonsters[i]->getCoord().x][dungeonMonsters[i]->getCoord().y] = DRAGONS;
                }
            }
        }
    }
    
    dungeonMatrix[dungeonPlayer->getCoord().x][dungeonPlayer->getCoord().y] = PLAYER;
    
    for (int j = 0; j < MAX_ARRAY_HEIGHT; j++){
        for (int i = 0; i < MAX_ARRAY_WIDTH; i++){
            cout << dungeonMatrix[i][j];
        }
        cout << endl;
    }
    
    cout << "Dungeon Level: " << dungeonLevel;
    dungeonPlayer->outputPlayerData();
    
    // generate the messages related to the player
    for (unsigned int i = 0; i < message.size(); i++){
        cout << message[i] << endl;
    }
    
    // clear the message for the next round
    dungeonMatrix[dungeonPlayer->getCoord().x][dungeonPlayer->getCoord().y] = FINAL_ROOM;
    for (unsigned int i = 0; i < dungeonItems.size(); i++){
        dungeonMatrix[dungeonItems[i]->getCoord().x][dungeonItems[i]->getCoord().y] = FINAL_ROOM;
    }
    for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
        dungeonMatrix[dungeonMonsters[i]->getCoord().x][dungeonMonsters[i]->getCoord().y] = FINAL_ROOM;
    }
    message.clear();
}

// perform corresponding command for the player
int Dungeon::performCommand(const char input){
    dungeonPlayer->regenerateHealth();
    
    if (dungeonPlayer->sleepPass()){
        return 0;
    }
    
    // initialize player message
    string playerMessage = "";
    // this bool is used to check if player attempts to pick up something
    bool isPickingUp = false;
    
    switch (input){
            // first four case are for movement and attack
        case ARROW_UP:
            if (!canWalk(Coord(dungeonPlayer->getCoord().x, dungeonPlayer->getCoord().y - 1))){
                for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
                    if (dungeonMonsters[i]->getCoord() == Coord(dungeonPlayer->getCoord().x, dungeonPlayer->getCoord().y - 1)){
                        if (dungeonPlayer->attack(dungeonMonsters[i], playerMessage)){
                            dropItem(dungeonMonsters[i]);
                            delete dungeonMonsters[i];
                            dungeonMonsters.erase(dungeonMonsters.begin() + i);
                        }
                    }
                }
                break;
            }
            else{
                dungeonPlayer->action(input, playerMessage);
                break;
            }
        case ARROW_DOWN:
            if (!canWalk(Coord(dungeonPlayer->getCoord().x, dungeonPlayer->getCoord().y + 1))){
                for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
                    if (dungeonMonsters[i]->getCoord() == Coord(dungeonPlayer->getCoord().x, dungeonPlayer->getCoord().y + 1)){
                        if (dungeonPlayer->attack(dungeonMonsters[i], playerMessage)){
                            dropItem(dungeonMonsters[i]);
                            delete dungeonMonsters[i];
                            dungeonMonsters.erase(dungeonMonsters.begin() + i);
                        }
                    }
                }
                break;
            }
            else{
                dungeonPlayer->action(input, playerMessage);
                break;
            }
        case ARROW_LEFT:
            if (!canWalk(Coord(dungeonPlayer->getCoord().x - 1, dungeonPlayer->getCoord().y))){
                for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
                    if (dungeonMonsters[i]->getCoord() == Coord(dungeonPlayer->getCoord().x - 1, dungeonPlayer->getCoord().y)){
                        if (dungeonPlayer->attack(dungeonMonsters[i], playerMessage)){
                            dropItem(dungeonMonsters[i]);
                            delete dungeonMonsters[i];
                            dungeonMonsters.erase(dungeonMonsters.begin() + i);
                        }
                    }
                }
                break;
            }
            else{
                dungeonPlayer->action(input, playerMessage);
                break;
            }
        case ARROW_RIGHT:
            if (!canWalk(Coord(dungeonPlayer->getCoord().x + 1, dungeonPlayer->getCoord().y))){
                for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
                    if (dungeonMonsters[i]->getCoord() == Coord(dungeonPlayer->getCoord().x + 1, dungeonPlayer->getCoord().y)){
                        if (dungeonPlayer->attack(dungeonMonsters[i], playerMessage)){
                            dropItem(dungeonMonsters[i]);
                            delete dungeonMonsters[i];
                            dungeonMonsters.erase(dungeonMonsters.begin() + i);
                        }
                    }
                }
                break;
            }
            else{
                dungeonPlayer->action(input, playerMessage);
                break;
            }
            // if stand on stair and input '<', generate a new level, return 1 and let Game know
        case '<':
            for (unsigned int i = 0; i < dungeonItems.size(); i++){
                if (dungeonItems[i]->getName() == "Stair" && dungeonItems[i]->getCoord() == dungeonPlayer->getCoord()){
                    return 1;
                }
            }
            break;
            // else will let player decide what should be performed
        default:
            isPickingUp = dungeonPlayer->action(input, playerMessage);
            break;
    }
    
    // if player decides to pick something up, look at the current ground to see if there is something
    if (isPickingUp){
        for (unsigned int i = 0; i < dungeonItems.size(); i++){
            // if there is something, check whether it's a weapon, scroll, or golden idol
            if (dungeonItems[i]->getCoord() == dungeonPlayer->getCoord() && dungeonItems[i]->getName() != "Stair"){
                Weapon* wptr = dynamic_cast<Weapon*>(dungeonItems[i]);
                if (wptr != NULL){
                    playerMessage = "You picked up " + dungeonItems[i]->getName();
                }
                else{
                    Scroll* sptr = dynamic_cast<Scroll*>(dungeonItems[i]);
                    if (sptr != NULL){
                        playerMessage = "You picked up a scroll called " + dungeonItems[i]->getName();
                    }
                    else{
                        if (dungeonItems[i]->getName() == "Golden Idol"){
                            return 2;
                        }
                    }
                }
                // if a weapon or a scroll, pick it up and erase it from the dungeon's itemlist
                dungeonPlayer->insertItem(dungeonItems[i]);
                dungeonItems.erase(dungeonItems.begin() + i);
            }
        }
    }
    
    
    // clear player message
    if (playerMessage != ""){
        message.push_back(playerMessage);
    }
    
    // if player uses a teleport scroll, teleport
    if (dungeonPlayer->checkTeleport()){
        Coord dungeonCoord(randInt(MAX_ARRAY_WIDTH - 1) + 1, randInt(MAX_ARRAY_HEIGHT - 1) + 1);
        while (!(dungeonMatrix[dungeonCoord.x][dungeonCoord.y] != WALL)){
            dungeonCoord.x = randInt(MAX_ARRAY_WIDTH - 1) + 1;
            dungeonCoord.y = randInt(MAX_ARRAY_HEIGHT - 1) + 1;
        }
        
        dungeonPlayer->setCoord((dungeonCoord));
    }
    
    return 0;
}

// perform corresponding command for monsters
void Dungeon::monsterAction(){
    string monsterMessage = "";
    
    // for each monster, checks action
    for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
        if (dungeonMonsters[i]->getName() == "Snakewoman"){
            Snakewomen* sw = (Snakewomen*)(dungeonMonsters[i]);
            sw->action(dungeonPlayer->getCoord(), this, monsterMessage);
        }
        else{
            if (dungeonMonsters[i]->getName() == "Bogeyman"){
                Bogeymen* bm = (Bogeymen*)(dungeonMonsters[i]);
                bm->action(dungeonPlayer->getCoord(), this, monsterMessage);
            }
            else{
                // Goblin needs to know the dungeon to determine its optimal pass
                // hence let all actors be "WALL"s and perform action for each goblin
                if (dungeonMonsters[i]->getName() == "Goblin"){
                    for (unsigned int j = 0; j < dungeonMonsters.size(); j++){
                        if (j != i){
                            setFloor(dungeonMonsters[j]->getCoord(), WALL);
                        }
                    }
                    
                    Goblins* gl = (Goblins*)(dungeonMonsters[i]);
                    gl->action(dungeonPlayer->getCoord(), this, monsterMessage);
                    
                    for (unsigned int j = 0; j < dungeonMonsters.size(); j++){
                        if (j != i){
                            setFloor(dungeonMonsters[j]->getCoord(), FINAL_ROOM);
                        }
                    }
                }
                else{
                    Dragons* dg = (Dragons*)(dungeonMonsters[i]);
                    dg->action(dungeonPlayer->getCoord(), this, monsterMessage);
                }
            }
        }
        
        // push monster action message and then clear it
        if (monsterMessage != ""){
            message.push_back(monsterMessage);
        }
        monsterMessage = "";
    }
}

// check if a coordinate is occupied by an actor or not, return true if not and false otherwise
bool Dungeon::canWalk(Coord c) const{
    // if is wall, cannot be reached
    if (dungeonMatrix[c.x][c.y] == FINAL_ROOM){
        // check if player or a monster is occupying this floor
        if (c != dungeonPlayer->getCoord()){
            for (unsigned int i = 0; i < dungeonMonsters.size(); i++){
                if (c == dungeonMonsters[i]->getCoord()){
                    return false;
                }
                else{
                    // if no, return true
                    if (i == dungeonMonsters.size() - 1){
                        return true;
                    }
                }
            }
        }
    }
    else{
        return false;
    }
    
    // if no monster, will not be unreachable
    return dungeonMonsters.size() == 0;
}

// check if a coordinate is occupied by an Gameobject or not, return true if there is an object and false otherwise
bool Dungeon::hasItem(Coord c) const{
    for (unsigned int i = 0; i < dungeonItems.size(); i++){
        if (c == dungeonItems[i]->getCoord()){
            return true;
        }
    }
    
    return false;
}

// the function to generate an empty dungeon
bool Dungeon::generateDungeon(){
    // file the dungeon with walls
    srand(static_cast<unsigned int>(time(0)));
    memset(dungeonMatrix, WALL, sizeof(char) * MAX_ARRAY_WIDTH * MAX_ARRAY_HEIGHT);
    
    // initialize some parameters
    int numberOfRoom = randInt(MAX_NUMBER_OF_ROOM - MIN_NUMBER_OF_ROOM + 1) + MIN_NUMBER_OF_ROOM;
    int currNoOfRoom = 0;
    
    Direction dir;
    int width, height;
    
    Coord startCoord(0, 0);
    Coord foundCoord(0, 0);
    
    vector<Coord> borderIndex;
    vector<Coord> corridorEndIndex;
    
    // make the first room at the top left of the dungeon
    startCoord.x = randInt(5) + 1;
    startCoord.y = randInt(5) + 1;
    width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
    height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
    while (!makeRoom(startCoord, height, width, borderIndex)){
        startCoord.x = randInt(5) + 1;
        startCoord.y = randInt(5) + 1;
        width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
        height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
    }
    
    currNoOfRoom++;
    
    // start generate corridors and rooms
    // note: some time (rarely) this part goes into a dead loop, hence we use a failing counter here.
    // if failed 1000 times, we abort the current model and regenerate the dungeon.
    int failMakeCorridorCounts = 0;
    while (currNoOfRoom < numberOfRoom){
        // find a wall or the end of a corridor
        findFloor(foundCoord, borderIndex, dir);
        startCoord = foundCoord;
        
        // start to make a corridor
        // if unable to make a corridor of specified attributes, put back the coordinate into the border coordinate and try again
        if (makeCorridor(startCoord, randInt(MAX_CORRIDOR_LENGTH - MIN_CORRIDOR_LENGTH + 1) + MIN_CORRIDOR_LENGTH, dir, corridorEndIndex) == 0){
            if (failMakeCorridorCounts == 100){
                return false;
            }
            
            pushbackCoord(foundCoord, borderIndex, dir);
            failMakeCorridorCounts++;
            continue;
        }
        else{
            // if the corridor does not generate a new end, but instead connects two existing rooms, continue
            if (makeCorridor(startCoord, randInt(MAX_CORRIDOR_LENGTH - MIN_CORRIDOR_LENGTH + 1) + MIN_CORRIDOR_LENGTH, dir, corridorEndIndex) == 2){
                // every time we successfully generate a corridor, reset the failing counter
                failMakeCorridorCounts = 0;
                continue;
            }
        }
        
        // every time we successfully generate a corridor, reset the failing counter
        failMakeCorridorCounts = 0;
        
        // if no available "end of corridor" to set up a new room, continue
        if (corridorEndIndex.size() > 0){
            findFloor(foundCoord, corridorEndIndex, dir);
        }
        else{
            continue;
        }
        
        // may not have enough space to set up a room, in such case a max attempt needs to be specified
        int failCount = 0;
        
        // generate a new room according to the specified direction
        // increase the fail count every time we fail to generate a room
        switch (dir){
            case north:
                width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                startCoord.x = foundCoord.x - randInt(width);
                startCoord.y = foundCoord.y - height + 1;
                
                while (failCount < MAX_FAIL_COUNT && !makeRoom(startCoord, height, width, borderIndex)){
                    failCount++;
                    width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                    height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                    startCoord.x = foundCoord.x - randInt(width);
                    startCoord.y = foundCoord.y - height + 1;
                }
                break;
            case south:
                width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                startCoord.x = foundCoord.x - randInt(width);
                startCoord.y = foundCoord.y;
                
                while (failCount < MAX_FAIL_COUNT && !makeRoom(startCoord, height, width, borderIndex)){
                    failCount++;
                    width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                    height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                    startCoord.x = foundCoord.x - randInt(width);
                    startCoord.y = foundCoord.y;
                }
                break;
            case west:
                width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                startCoord.x = foundCoord.x - width + 1;
                startCoord.y = foundCoord.y - randInt(height);
                
                while (failCount < MAX_FAIL_COUNT && !makeRoom(startCoord, height, width, borderIndex)){
                    failCount++;
                    width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                    height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                    startCoord.x = foundCoord.x - width + 1;
                    startCoord.y = foundCoord.y - randInt(height);
                }
                break;
            case east:
                width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                startCoord.x = foundCoord.x;
                startCoord.y = foundCoord.y - randInt(height);
                
                while (failCount < MAX_FAIL_COUNT && !makeRoom(startCoord, height, width, borderIndex)){
                    failCount++;
                    width = randInt(MAX_ROOM_WIDTH - MIN_ROOM_WIDTH + 1) + MIN_ROOM_WIDTH;
                    height = randInt(MAX_ROOM_HEIGHT - MIN_ROOM_HEIGHT + 1) + MIN_ROOM_HEIGHT;
                    startCoord.x = foundCoord.x;
                    startCoord.y = foundCoord.y - randInt(height);
                }
                break;
        }
        
        // if successfully generate a new room, increase the room count
        if (failCount < MAX_FAIL_COUNT){
            currNoOfRoom++;
        }
        else{
            // else we push the "end of the corridor" coordinate as a new possible start point
            pushbackCoord(foundCoord, borderIndex, dir);
        }
    }
    
    // update the dungeon to the right look ('#'s and ' 's)
    for (int j = 0; j < MAX_ARRAY_HEIGHT; j++){
        for (int i = 0; i < MAX_ARRAY_WIDTH; i++){
            if (dungeonMatrix[i][j] != WALL){
                dungeonMatrix[i][j] = ' ';
            }
        }
    }
    
    return true;
}

// generate the inital items in the dungeon
void Dungeon::generateInitialItems(){
    // generate initial items
    int itemNumber = randInt(2) + 2;
    int type = 0;
    Coord dungeonCoord(randInt(MAX_ARRAY_WIDTH - 1) + 1, randInt(MAX_ARRAY_HEIGHT - 1) + 1);
    
    for (int i = 0; i < itemNumber; i++){
        int itemType = randInt(2);
        if (itemType == weapon){
            // cannot have magicAxe or magic fangs of sleep
            type = randInt(3);
        }
        else{
            // cannot have scroll of teleportation
            type = randInt(4) + 1;
        }
        while (!generateItem(dungeonCoord, itemType, type)){
            dungeonCoord.x = randInt(MAX_ARRAY_WIDTH - 1) + 1;
            dungeonCoord.y = randInt(MAX_ARRAY_HEIGHT - 1) + 1;
        }
    }
}

// generate the win condition or the current level, either a stair or a golden idol
void Dungeon::generateStairOrIdol(){
    Coord dungeonCoord(randInt(MAX_ARRAY_WIDTH - 1) + 1, randInt(MAX_ARRAY_HEIGHT - 1) + 1);
    
    // find a spot that does not have an item placed on it
    while (!(dungeonMatrix[dungeonCoord.x][dungeonCoord.y] != WALL && !hasItem(dungeonCoord))){
        dungeonCoord.x = randInt(MAX_ARRAY_WIDTH - 1) + 1;
        dungeonCoord.y = randInt(MAX_ARRAY_HEIGHT - 1) + 1;
    }
    
    // generate a stair or a golden idol based on the dungeon level
    dungeonItems.push_back(new GameObject(dungeonCoord, (dungeonLevel < 4) ? "Stair" : "Golden Idol"));
    dungeonMatrix[dungeonCoord.x][dungeonCoord.y] = (dungeonLevel < 4) ? STAIR : GOLDEN_IDOL;
}

// generate the player or inherit the player from the previous level
void Dungeon::generatePlayer(Player* dPlayer = NULL){
    // find a empty spot
    Coord dungeonCoord(randInt(MAX_ARRAY_WIDTH - 1) + 1, randInt(MAX_ARRAY_HEIGHT - 1) + 1);
    while (dungeonMatrix[dungeonCoord.x][dungeonCoord.y] == WALL){
        dungeonCoord.x = randInt(MAX_ARRAY_WIDTH - 1) + 1;
        dungeonCoord.y = randInt(MAX_ARRAY_HEIGHT - 1) + 1;
    }
    
    // if dungeon level is 0, generate a new player
    if (dungeonLevel == 0){
        dungeonPlayer = new Player(dungeonCoord);
    }
    // else inherit the player from the last level
    else{
        dungeonPlayer = new Player(dungeonCoord, *dPlayer);
    }
}

// generate monsters according to the current level
void Dungeon::generateMonsters(){
    // generate the number of monsters based on the dungeon level
    int numberOfMonsters = 2 + randInt(5 * (dungeonLevel + 1));
    int monsterType = 0;
    
    // find a empty spot
    Coord dungeonCoord(randInt(MAX_ARRAY_WIDTH - 1) + 1, randInt(MAX_ARRAY_HEIGHT - 1) + 1);
    for (int i = 0; i < numberOfMonsters; i++){
        while (!canWalk(dungeonCoord)){
            dungeonCoord.x = randInt(MAX_ARRAY_WIDTH - 1) + 1;
            dungeonCoord.y = randInt(MAX_ARRAY_HEIGHT - 1) + 1;
        }
        
        // if dungeon level < 2, can only have goblins and snakewomen
        if (dungeonLevel < 2){
            monsterType = randInt(2) + 2;
        }
        else{
            // if dungeon level < 3, can have bogeymen but no dragons
            if (dungeonLevel < 3){
                monsterType = randInt(3) + 1;
            }
            // else can have anything :/
            else{
                monsterType = randInt(4);
            }
        }
        
        // push the monster into the monster list
        Actor* monster = NULL;
        switch (monsterType){
            case snakewomen:
                monster = new Snakewomen(dungeonCoord);
                dungeonMonsters.push_back(monster);
                break;
            case bogeymen:
                monster = new Bogeymen(dungeonCoord);
                dungeonMonsters.push_back(monster);
                break;
            case dragons:
                monster = new Dragons(dungeonCoord);
                dungeonMonsters.push_back(monster);
                break;
            case goblins:
                monster = new Goblins(dungeonCoord, getGoblinSmellDistance());
                dungeonMonsters.push_back(monster);
                break;
        }
    }
}

// generate a room
bool Dungeon::makeRoom(Coord startCoord, int height, int width, vector<Coord> &borderIndex){
    // if the starting point is out of bound, return false
    if (startCoord.x < 1 || startCoord.y < 1 || startCoord.x + width >= 69 || startCoord.y + height >= 17){
        return false;
    }
    else{
        // if the current room overlaps with some other rooms or corridors, return false
        for (int j = startCoord.y; j < startCoord.y + height; j++){
            for (int i = startCoord.x; i < startCoord.x + width; i++){
                if (dungeonMatrix[i][j] == BORDER ||
                    dungeonMatrix[i - 1][j] == BORDER ||
                    dungeonMatrix[i + 1][j] == BORDER ||
                    dungeonMatrix[i][j - 1] == BORDER ||
                    dungeonMatrix[i][j + 1] == BORDER ||
                    dungeonMatrix[i][j] == ROOM ||
                    dungeonMatrix[i - 1][j] == ROOM ||
                    dungeonMatrix[i + 1][j] == ROOM ||
                    dungeonMatrix[i][j - 1] == ROOM ||
                    dungeonMatrix[i][j + 1] == ROOM){
                    return false;
                }
            }
        }
        
        // mark the border and the internal of the room. Push all border coordinate into the vector for future use
        for (int j = startCoord.y; j < startCoord.y + height; j++){
            for (int i = startCoord.x; i < startCoord.x + width; i++){
                if (j == startCoord.y || j == startCoord.y + height - 1 || i == startCoord.x || i == startCoord.x + width - 1){
                    Coord index(i, j);
                    borderIndex.push_back(index);
                    dungeonMatrix[i][j] = BORDER;
                }
                else{
                    dungeonMatrix[i][j] = ROOM;
                }
            }
        }
        
        return true;
    }
}

// find the starting of the corridor for new corridor
// or the end of the corridor for the new room
// note: this returns a index that is WALL, adjacent to the found index
void Dungeon::findFloor(Coord &coord, vector<Coord> &indexVector, Direction& dir){
    int index = randInt(indexVector.size());
    
    // if a corresponding floor has been found, set the direction, push the coordinate into the vector
    // and pass the coordinate into coord
    if (dungeonMatrix[indexVector[index].x + 1][indexVector[index].y] == WALL &&
        dungeonMatrix[indexVector[index].x - 1][indexVector[index].y] != WALL &&
        indexVector[index].x + 1 >= 1 && indexVector[index].x + 1 <= MAX_ARRAY_WIDTH - 1){
        dir = east;
        coord.x = indexVector[index].x + 1;
        coord.y = indexVector[index].y;
        indexVector.erase(indexVector.begin() + index);
        return;
    }
    
    if (dungeonMatrix[indexVector[index].x][indexVector[index].y + 1] == WALL &&
        dungeonMatrix[indexVector[index].x][indexVector[index].y - 1] != WALL &&
        indexVector[index].y + 1 >= 1 && indexVector[index].y + 1 <= MAX_ARRAY_HEIGHT - 1){
        dir = south;
        coord.x = indexVector[index].x;
        coord.y = indexVector[index].y + 1;
        indexVector.erase(indexVector.begin() + index);
        return;
    }
    
    if (dungeonMatrix[indexVector[index].x - 1][indexVector[index].y] == WALL &&
        dungeonMatrix[indexVector[index].x + 1][indexVector[index].y] != WALL &&
        indexVector[index].x - 1 >= 1 && indexVector[index].x - 1 <= MAX_ARRAY_WIDTH - 1){
        dir = west;
        coord.x = indexVector[index].x - 1;
        coord.y = indexVector[index].y;
        indexVector.erase(indexVector.begin() + index);
        return;
    }
    
    if (dungeonMatrix[indexVector[index].x][indexVector[index].y - 1] == WALL &&
        dungeonMatrix[indexVector[index].x][indexVector[index].y + 1] != WALL &&
        indexVector[index].y - 1 >= 1 && indexVector[index].y - 1 <= MAX_ARRAY_HEIGHT - 1){
        dir = north;
        coord.x = indexVector[index].x;
        coord.y = indexVector[index].y - 1;
        indexVector.erase(indexVector.begin() + index);
        return;
    }
}

// generate a new corridor of specific starting point, length and direction
int Dungeon::makeCorridor(Coord startCoord, int length, Direction dir, std::vector<Coord> &corridorEndIndex){
    // for each direction, the following procedures are performed:
    // if there is no space to set up a new room after generate this corridor, return 0
    // if there is corridors or rooms in the way, return 0
    // start generating the corridor
    // in the end determine whether the corridor connects two different room
    // if no connection, return 1 and try to generate a room
    // if there is a connection, return 2 and does not attempt to generate a room
    switch (dir){
            // take north as example
        case north:
            // if the end of corridor lies somewhere that cannot create a room, return 0
            if (startCoord.y - length <= MIN_ROOM_HEIGHT){
                return 0;
            }
            
            // if the end of corridor is in a very tight spot e.g: not far away from a room. return 0
            if (dungeonMatrix[startCoord.x][startCoord.y - length] == WALL && dungeonMatrix[startCoord.x][startCoord.y - length - MIN_ROOM_HEIGHT] != WALL){
                return 0;
            }
            
            // if the path overlaps with some corridor of is near a corridor or room, return 0
            for (int i = startCoord.y; i > startCoord.y - length; i--){
                if (dungeonMatrix[startCoord.x - 1][i] != WALL || dungeonMatrix[startCoord.x + 1][i] != WALL || dungeonMatrix[startCoord.x][i] != WALL){
                    return 0;
                }
            }
            
            // start to generate the corridor
            for (int i = startCoord.y; i > startCoord.y - length; i--){
                if (i == startCoord.y - length + 1){
                    // if the end is adjacent to a border, connect
                    if (dungeonMatrix[startCoord.x][i - 1] != BORDER){
                        if (dungeonMatrix[startCoord.x - 1][i - 1] == BORDER ||
                            dungeonMatrix[startCoord.x + 1][i - 1] == BORDER){
                            dungeonMatrix[startCoord.x][i] = ROOM;
                            dungeonMatrix[startCoord.x][i - 1] = ROOM;
                            return 2;
                        }
                        // else set it to the end of the corridor
                        else{
                            corridorEndIndex.push_back(Coord(startCoord.x, i));
                            dungeonMatrix[startCoord.x][i] = END_OF_CORRIDOR;
                        }
                    }
                    // else it is inside a room, connect
                    else{
                        dungeonMatrix[startCoord.x][i] = ROOM;
                        return 2;
                    }
                }
                // else still generating.
                else{
                    dungeonMatrix[startCoord.x][i] = ROOM;
                }
            }
            break;
            
        case south:
            if (startCoord.y + length >= MAX_ARRAY_HEIGHT - MIN_ROOM_HEIGHT){
                return 0;
            }
            
            if (dungeonMatrix[startCoord.x][startCoord.y + length] == WALL && dungeonMatrix[startCoord.x][startCoord.y + length + MIN_ROOM_HEIGHT] != WALL){
                return 0;
            }
            
            for (int i = startCoord.y; i < startCoord.y + length; i++){
                if (dungeonMatrix[startCoord.x - 1][i] != WALL || dungeonMatrix[startCoord.x + 1][i] != WALL || dungeonMatrix[startCoord.x][i] != WALL){
                    return 0;
                }
            }
            
            for (int i = startCoord.y; i < startCoord.y + length; i++){
                if (i == startCoord.y + length - 1){
                    if (dungeonMatrix[startCoord.x][i + 1] != BORDER){
                        if (dungeonMatrix[startCoord.x - 1][i + 1] == BORDER ||
                            dungeonMatrix[startCoord.x + 1][i + 1] == BORDER){
                            dungeonMatrix[startCoord.x][i] = ROOM;
                            dungeonMatrix[startCoord.x][i + 1] = ROOM;
                            return 2;
                        }
                        else{
                            corridorEndIndex.push_back(Coord(startCoord.x, i));
                            dungeonMatrix[startCoord.x][i] = END_OF_CORRIDOR;
                        }
                    }
                    else{
                        dungeonMatrix[startCoord.x][i] = ROOM;
                        return 2;
                    }
                }
                else{
                    dungeonMatrix[startCoord.x][i] = ROOM;
                }
            }
            break;
            
        case west:
            if (startCoord.x - length <= MIN_ROOM_WIDTH){
                return 0;
            }
            
            if (dungeonMatrix[startCoord.x - length][startCoord.y] == WALL && dungeonMatrix[startCoord.x - length - MIN_ROOM_WIDTH][startCoord.y] != WALL){
                return 0;
            }
            
            for (int i = startCoord.x; i > startCoord.x - length; i--){
                if (dungeonMatrix[i][startCoord.y - 1] != WALL || dungeonMatrix[i][startCoord.y + 1] != WALL || dungeonMatrix[i][startCoord.y] != WALL){
                    return 0;
                }
            }
            
            for (int i = startCoord.x; i > startCoord.x - length; i--){
                if (i == startCoord.x - length + 1){
                    if (dungeonMatrix[i - 1][startCoord.y] != BORDER){
                        if (dungeonMatrix[i - 1][startCoord.y - 1] == BORDER ||
                            dungeonMatrix[i - 1][startCoord.y + 1] == BORDER){
                            dungeonMatrix[i][startCoord.y] = ROOM;
                            dungeonMatrix[i - 1][startCoord.y] = ROOM;
                            return 2;
                        }
                        else{
                            corridorEndIndex.push_back(Coord(i, startCoord.y));
                            dungeonMatrix[i][startCoord.y] = END_OF_CORRIDOR;
                        }
                    }
                    else{
                        dungeonMatrix[i][startCoord.y] = ROOM;
                        return 2;
                    }
                }
                else{
                    dungeonMatrix[i][startCoord.y] = ROOM;
                }
            }
            break;
        case east:
            if (startCoord.x + length >= MAX_ARRAY_WIDTH - MIN_ROOM_WIDTH){
                return 0;
            }
            
            if (dungeonMatrix[startCoord.x + length][startCoord.y] == WALL && dungeonMatrix[startCoord.x + length - MIN_ROOM_WIDTH][startCoord.y] != WALL){
                return 0;
            }
            
            for (int i = startCoord.x; i < startCoord.x + length; i++){
                if (dungeonMatrix[i][startCoord.y - 1] != WALL || dungeonMatrix[i][startCoord.y + 1] != WALL || dungeonMatrix[i][startCoord.y] != WALL){
                    return 0;
                }
            }
            
            for (int i = startCoord.x; i < startCoord.x + length; i++){
                if (i == startCoord.x + length - 1){
                    if (dungeonMatrix[i + 1][startCoord.y] != BORDER){
                        if (dungeonMatrix[i + 1][startCoord.y - 1] == BORDER ||
                            dungeonMatrix[i + 1][startCoord.y + 1] == BORDER){
                            dungeonMatrix[i][startCoord.y] = ROOM;
                            dungeonMatrix[i + 1][startCoord.y] = ROOM;
                            return 2;
                        }
                        else{
                            corridorEndIndex.push_back(Coord(i, startCoord.y));
                            dungeonMatrix[i][startCoord.y] = END_OF_CORRIDOR;
                        }
                    }
                    else{
                        dungeonMatrix[i][startCoord.y] = ROOM;
                        return 2;
                    }
                }
                else{
                    dungeonMatrix[i][startCoord.y] = ROOM;
                }
            }
            break;
    }
    
    return 1;
}

// push a coordinate generated from findFloor back to the vector
// note that the coordinate has been modified accordingly
void Dungeon::pushbackCoord(Coord coord, std::vector<Coord> &borderIndex, Direction dir){
    switch (dir){
        case north:
            borderIndex.push_back(Coord(coord.x, coord.y + 1));
            break;
        case south:
            borderIndex.push_back(Coord(coord.x, coord.y - 1));
            break;
        case west:
            borderIndex.push_back(Coord(coord.x + 1, coord.y));
            break;
        case east:
            borderIndex.push_back(Coord(coord.x - 1, coord.y));
            break;
    }
}

// generate an item of specific type at a given location
bool Dungeon::generateItem(Coord coordinate, int itemType, int type){
    // should not overlap with walls or other items
    if (dungeonMatrix[coordinate.x][coordinate.y] != WALL && !hasItem(coordinate)){
        if (itemType == weapon){
            GameObject* item = new Weapon(coordinate, type);
            dungeonItems.push_back(item);
            dungeonMatrix[coordinate.x][coordinate.y] = WEAPON;
        }
        else{
            GameObject* item = new Scroll(coordinate, type);
            dungeonItems.push_back(item);
            dungeonMatrix[coordinate.x][coordinate.y] = SCROLL;
        }
        
        return true;
    }
    else{
        return false;
    }
}

// generate loot system, monsters drop items when they die
void Dungeon::dropItem(Actor* deadMonster){
    // generate loot based on the monster type
    if (deadMonster->getName() == "Snakewoman"){
        if (!hasItem(deadMonster->getCoord())){
            if (trueWithProbability(1.0 / 3.0)){
                // can only drop magic fangs of sleep
                generateItem(deadMonster->getCoord(), weapon, magicFangsOfSleep);
            }
        }
    }
    else{
        if (deadMonster->getName() == "Goblin"){
            if (!hasItem(deadMonster->getCoord())){
                if (trueWithProbability(1.0 / 3.0)){
                    // can only drop magic fangs of sleep or magic axe
                    generateItem(deadMonster->getCoord(), weapon, randInt(2) + 3);
                }
            }
        }
        else{
            if (deadMonster->getName() == "Bogeyman"){
                if (!hasItem(deadMonster->getCoord())){
                    if (trueWithProbability(1.0 / 10.0)){
                        // can only drop magic axe
                        generateItem(deadMonster->getCoord(), weapon, magicAxe);
                    }
                }
            }
            else{
                if (deadMonster->getName() == "Dragon"){
                    if (!hasItem(deadMonster->getCoord())){
                        // can drop any kinds of scroll
                        generateItem(deadMonster->getCoord(), scroll, randInt(5));
                    }
                }
            }
        }
    }
}
