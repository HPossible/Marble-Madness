#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>
#include "Actor.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class A_player;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath = "");
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	/////////////////////////////////////////////////////////

	void updateDisplayText();
	void loadactors(Level lev);
	//int locatenum(int x, int y);			// number of objects on the block (other than player)
	//Actor* locate(int x, int y, int n);		// return the nth object on the block (n starts from 0)
	int locate(int x, int y, std::vector<Actor*> &items);	// return number of objects
	A_player* locatep() { return player; }	// remember to specially check for player when locating
	bool locateobstacle(int x, int y);
	void addmember(Actor* n) { actor.push_back(n); }
	void deccrystal() { crystalnum--; }
	int findcn() const { return crystalnum; }
	bool canproduce(int x,int y) const;
	void win();

private:
	A_player* player;
	std::vector<Actor*> actor;

	int bonus;
	int crystalnum;
	bool passgame;
	//int health;		// player's hitpoint * 5; may get rid of this variable
	//int ammo;		// player's peanum; may get rid of this variable
};




#endif // STUDENTWORLD_H_
