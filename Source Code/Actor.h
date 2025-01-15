#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

// directions: none(-1), rest follows the angles
class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, StudentWorld* wd, int startDirection, int hp);
	virtual ~Actor();
	virtual void doSomething() { ; }
	virtual bool findml() const = 0; // { return marble_land; }
	virtual int findpl() const = 0;  // { return player_land; }
	virtual bool cankillpea() const { return false; }	// only returns true for walls & factory
	virtual bool isgoodie() const { return false; }	// only returns true for goodies
	virtual bool isbot() const { return false; }
	virtual bool isobstacle() const { return false; }	// true for wall, marble, robots, factory
										// note: returns false for pits!! for moving judgement, refer to findpl()
	int findh() const { return hitpoints; }
	StudentWorld* findw() const { return world; }
	void changeh(int h) { hitpoints = h; }
	virtual void attacked();
	void navigate(int& a, int& b, int d);

private:
	//int xcoord;
	//int ycoord; can use m_x & m_y in GraphObject
	int hitpoints;	// not the same as health in StudentWorld	=1000 -> undestroyable
	StudentWorld* world;
	//bool marble_land;  // can something be pushed on to this block; 0 = no, 1 = yes
	//int player_land;  // can player step on this block; 0=no, 1=yes, 2 means the stuff is pushable (maybe)
	//rather than adding more and more stuff to the initializer list, for ml & pl and all similar stuffs
	//(which is the same for all objects of the same class), I decided to put it as a pure virtual function
};

const bool findplayer(int x, int y, StudentWorld* wd);

////////////////////////////////////////////////////////////////

class A_player : public Actor
{
public:
	A_player(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
	virtual bool findml() const { return 0; }
	virtual int findpl() const { return 0; }

	int findpn() const { return peanum; }
	void changepn(int n) { peanum+=n; }
	void helpmove(int x, int y, int ch);
	virtual void attacked();

private:
	int peanum;
};

////////////////////////////////////////////

class A_wall : public Actor
{
public:
	A_wall(int imageID, int startX, int startY, StudentWorld* wd);
	virtual bool findml() const { return 0; }
	virtual int findpl() const { return 0; }
	virtual bool cankillpea() const { return 1; }
	virtual bool isobstacle() const { return true; }
};

////////////////////////////////////////////

// since marble is the only pushable object in this game, I leaved it this way
// if there are more than 1 pushable objects, then there should be a class like "pushable objects"
// which should be a base class of marbles
class A_marble : public Actor
{
public:
	A_marble(int imageID, int startX, int startY, StudentWorld* wd);
	virtual bool findml() const { return 0; }
	virtual int findpl() const { return 2; }
	virtual bool isobstacle() const { return true; }
};

////////////////////////////////////////////

class A_pea : public Actor
{
public:
	A_pea(int imageID, int startX, int startY, StudentWorld* wd, int startDirection);
	virtual void doSomething();
	virtual bool findml() const { return 1; }
	virtual int findpl() const { return 1; }
};

////////////////////////////////////////////

class A_pit : public Actor
{
public:
	A_pit(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
	virtual bool findml() const { return 1; }
	virtual int findpl() const { return 0; }
};

////////////////////////////////////////////

class A_exit : public Actor
{
public:
	A_exit(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
	virtual bool findml() const { return 0; }
	virtual int findpl() const { return 1; }
private:
	bool revealed;
};

////////////////////////////////////////////

class A_goodies : public Actor
{
public:
	A_goodies(int imageID, int startX, int startY, StudentWorld* wd, int pts);
	virtual void doSomething();
	virtual bool findml() const { return 0; }
	virtual int findpl() const { return 1; }
	virtual bool isgoodie() const { return 1; }
	bool function() const { return findh() == 1000 && findplayer(getX(), getY(), findw()); }
private:
	int point;
};

class A_crystal : public A_goodies
{
public:
	A_crystal(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
	virtual bool isgoodie() const { return 0; }
};

class A_extralife : public A_goodies
{
public:
	A_extralife(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
};

class A_restorehealth : public A_goodies
{
public:
	A_restorehealth(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
};

class A_ammo : public A_goodies
{
public:
	A_ammo(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();

};

////////////////////////////////////////////

class A_bots : public Actor
{
public:
	A_bots(int imageID, int startX, int startY, StudentWorld* wd, int startDirection, int hp, int pts);
	virtual void doSomething();
	virtual bool findml() const { return 0; }
	virtual int findpl() const { return 0; }
	virtual bool isobstacle() const { return true; }
	virtual bool isbot() const { return true; }
	bool canmove() const { return move; }
	virtual void attacked();
	bool trytomove(int x, int y, int d);
	bool trytoshoot(int x, int y);
	void changepoints(int n) { points = n; }

private:
	int tick_count;
	int tick_max;
	bool move;
	int points;
};

class A_ragebot : public A_bots
{
public:
	A_ragebot(int imageID, int startX, int startY, StudentWorld* wd, int startDirection);
	virtual void doSomething();
};

class A_thiefbot : public A_bots
{
public:
	A_thiefbot(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
	virtual void attacked();
	void startup();
	bool pickup();
	void move();

private:
	int distance_before_turning;
	Actor* object;		// nullptr = can pick up; store the pointer to goodie
	int distance_moved;
};

class A_meanthiefbot : public A_thiefbot
{
public:
	A_meanthiefbot(int imageID, int startX, int startY, StudentWorld* wd);
	virtual void doSomething();
};

////////////////////////////////////////////

class A_factory : public Actor
{
public:
	A_factory(int imageID, int startX, int startY, StudentWorld* wd, bool tp);
	virtual void doSomething();
	virtual bool findml() const { return 0; }
	virtual int findpl() const { return 0; }
	virtual bool isobstacle() const { return true; }
	virtual bool cankillpea() const { return true; }

private:
	bool type; // 0=normal, 1=mean
};



#endif // ACTOR_H_
