#include "Actor.h"
#include "StudentWorld.h"
#include <vector>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

/////////////////////////////////////// Actor

Actor::~Actor()
{
}

Actor::Actor(int imageID, int startX, int startY, StudentWorld* wd, int startDirection, int hp)
:GraphObject(imageID, startX, startY, startDirection), hitpoints(hp), world(wd)
{
}

void Actor::attacked()
{
	if(findh()<1000) changeh(findh() - 2);
}

void Actor::navigate(int& a, int& b, int d)
{
	a = 0, b = 0;
	if (d == right) a = 1;
	else if (d == left) a = -1;
	else if (d == up) b = 1;
	else if (d == down) b = -1;
}

/////////////////////////////////////// A_Player

A_player::A_player(int imageID, int startX, int startY, StudentWorld* wd)
:Actor(imageID, startX, startY, wd, right, 20), peanum(20)
{
	setVisible(true);
}

void A_player::doSomething()
{
	if (findh() <= 0) return;

	int ch;
	if (findw()->getKey(ch))
	{
		switch (ch)
		{
			case KEY_PRESS_LEFT:
			case KEY_PRESS_RIGHT:
			case KEY_PRESS_UP:
			case KEY_PRESS_DOWN:
				helpmove(getX(), getY(), ch);
				break;
			case KEY_PRESS_SPACE:
				if (findpn())
				{
					changepn(-1);
					findw()->playSound(SOUND_PLAYER_FIRE);
					int x = getX(), y = getY(), d=getDirection();
					if (d == left) x--;
					else if (d == right) x++;
					else if (d == up) y++;
					else if (d == down) y--;
					Actor* pea = new A_pea(IID_PEA, x, y, findw(), d);
					findw()->addmember(pea);
				}
				break;
			case KEY_PRESS_ESCAPE:
				changeh(0);
				return;
			default:
				break;
		}
	}
}

void A_player::helpmove(int x, int y, int ch)	
{
	int coord[4][3] =
	{
		left, -1, 0,
		right, 1, 0,
		up, 0, 1,
		down, 0, -1,
	};
	ch = (ch % 1000) % 4;

	setDirection(coord[ch][0]);
	x += coord[ch][1];
	y += coord[ch][2];				// target position
	int xx = x + coord[ch][1];
	int yy = y + coord[ch][2];		// pos after target position

	std::vector<Actor*> item1;
	if (!findw()->locate(x, y, item1)) moveTo(x, y);	// blank space
	else												// going to move / return directly
	{
		std::vector<bool> movestate(item1.size(),0);
		int pushnum = 0;
		// movestate keep tracks of the moving state of each object
		// pushnum is the number that needs to be pushed

		for (int i = 0;i < item1.size();i++)
		{
			Actor* temp = item1[i];
			if (temp->findpl() == 0) return;			// block, can't move!
			if (temp->findpl() == 1) continue;			// movable, skip!
			// else	temp->findpl()==2				       push needed!
			std::vector<Actor*> item2;
			findw()->locate(xx, yy, item2);				// check the dest. of the object
			for (int j = 0;j < item2.size();j++)		// check each object dest.
			{
				Actor* temp2 = item2[j];
				if (!temp2->findml()) return;			// cannot push! return!
			}
			movestate[i] = 1, pushnum++;				// can push!
		}

		if (pushnum >= 2) return;	// need to push more than 1 object, cannot!
		moveTo(x, y);
		if (pushnum == 1)			// need to push 1 object
		{
			for (int i = 0;i < movestate.size();i++)		// push this object
			{
				if (movestate[i]) item1[i]->moveTo(xx, yy);
				return;
			}
		}
	}

	return;
}

void A_player::attacked()
{
	Actor::attacked();
	if (findh() > 0) findw()->playSound(SOUND_PLAYER_IMPACT);
	else findw()->playSound(SOUND_PLAYER_DIE);
}

/////////////////////////////////////// A_wall

A_wall::A_wall(int imageID, int startX, int startY, StudentWorld* wd)
:Actor(imageID, startX, startY, wd, none, 1000)
{
	setVisible(true);
}

/////////////////////////////////////// A_marble

A_marble::A_marble(int imageID, int startX, int startY, StudentWorld* wd)
:Actor(imageID, startX, startY, wd, none, 10)
{
	setVisible(true);
}

/////////////////////////////////////// A_pea

A_pea::A_pea(int imageID, int startX, int startY, StudentWorld* wd, int startDirection)
:Actor(imageID, startX, startY, wd, startDirection, 1000)
{
	setVisible(true);
}

void A_pea::doSomething()
{
	if (findplayer(getX(),getY(), findw()))
	{	// hit the player
		findw()->locatep()->attacked();
		changeh(0);
		return;
	}
	else
	{
		std::vector<Actor*> item;
		for (int i = 0;i < findw()->locate(getX(), getY(), item);i++)
		{	// check every object
			Actor* temp = item[i];
			if (temp != nullptr)			// this is not needed, but just to be safe
			{
				if (temp->findh() < 1000)	// damagable objects
				{
					temp->attacked();
					changeh(0);
				}
				else if (temp->cankillpea())	// wall & factory
				{
					changeh(0);
				}
				// else pass through it
			}
		}
	}
	if (findh() <= 0) return;

	int a, b;
	navigate(a, b, getDirection());
	moveTo(getX() + a, getY() + b);

	if (findplayer(getX(), getY(),findw()))
	{	// hit the player
		findw()->locatep()->attacked();
		changeh(0);
		return;
	}
	else
	{
		std::vector<Actor*> item;
		for (int i = 0;i < findw()->locate(getX(), getY(), item);i++)
		{	// check every object
			Actor* temp = item[i];
			if (temp != nullptr)			// this is not needed, but just to be safe
			{
				if (temp->findh() < 1000)	// damagable objects
				{
					temp->attacked();
					changeh(0);
					return;
				}
				else if (temp->cankillpea())	// wall & factory
				{
					changeh(0);
					return;
				}
				// else pass through it
			}
		}
	}
	// small edge case for peas: if the player fires a pea and walks in the same direction
	// at the exact next tick, then this pea will damage the player
	// in real game, human is not fast enough to do this, but in slow motion it's possible
}

/////////////////////////////////////// A_pit

A_pit::A_pit(int imageID, int startX, int startY, StudentWorld* wd)
:Actor(imageID, startX, startY, wd, none, 1000)
{
	setVisible(true);
}

void A_pit::doSomething()
{
	std::vector<Actor*> item;
	for (int i = 0;i < findw()->locate(getX(), getY(), item);i++)
	{
		if (item[i]->findpl() == 2)		// is a pushable object (in this case, marble)
		{
			item[i]->changeh(0);
			changeh(0);
		}
	}
}

/////////////////////////////////////// A_exit

A_exit::A_exit(int imageID, int startX, int startY, StudentWorld* wd)
:Actor(imageID, startX, startY, wd, none, 1000), revealed(false)
{
	setVisible(false);
}

void A_exit::doSomething()
{
	if ((!revealed) && findw()->findcn() == 0)
	{
		revealed = true;
		setVisible(true);
		findw()->playSound(SOUND_REVEAL_EXIT);
	}

	if (revealed && findplayer(getX(), getY(), findw())) findw()->win();
}

/////////////////////////////////////// A_goodies & all the goodies

A_goodies::A_goodies(int imageID, int startX, int startY, StudentWorld* wd, int pts)
:Actor(imageID, startX, startY, wd, none, 1000), point(pts)
{
	setVisible(true);
}

void A_goodies::doSomething()
{
	if (function())
	{
		findw()->increaseScore(point);
		changeh(0);
		findw()->playSound(SOUND_GOT_GOODIE);
	}
}

A_crystal::A_crystal(int imageID, int startX, int startY, StudentWorld* wd)
:A_goodies(imageID, startX, startY, wd, 50)
{
	setVisible(true);
}

void A_crystal::doSomething()
{
	if (function())
	{
		A_goodies::doSomething();
		findw()->deccrystal();
	}
}

A_extralife::A_extralife(int imageID, int startX, int startY, StudentWorld* wd)
:A_goodies(imageID, startX, startY, wd, 1000)
{
	setVisible(true);
}

void A_extralife::doSomething()
{
	if (function())
	{
		A_goodies::doSomething();
		findw()->incLives();
	}
}

A_restorehealth::A_restorehealth(int imageID, int startX, int startY, StudentWorld* wd)
:A_goodies(imageID, startX, startY, wd, 500)
{
}

void A_restorehealth::doSomething()
{
	{
		if (function())
		{
			A_goodies::doSomething();
			findw()->locatep()->changeh(20);
		}
	}
}

A_ammo::A_ammo(int imageID, int startX, int startY, StudentWorld* wd)
:A_goodies(imageID, startX, startY, wd, 100)
{
}

void A_ammo::doSomething()
{
	if (function())
	{
		A_goodies::doSomething();
		findw()->locatep()->changepn(20);
	}
}

/////////////////////////////////////// A_bots & all the bots

A_bots::A_bots(int imageID, int startX, int startY, StudentWorld* wd, int startDirection, int hp, int pts)
:Actor(imageID, startX, startY, wd, startDirection, hp), tick_count(0), tick_max(0), move(false), points(pts)
{
	setVisible(true);
	int ticks = (28 - findw()->getLevel()) / 4;
	if (ticks < 3) ticks = 3;
	tick_max = ticks;
}

void A_bots::doSomething()
{
	if (move == true) move = false;

	if (tick_count == tick_max)
	{
		tick_count = 0;
		move = true;
	}
	else tick_count++;
}

void A_bots::attacked()
{
	Actor::attacked();
	if (findh() > 0) findw()->playSound(SOUND_ROBOT_IMPACT);
	else
	{
		findw()->playSound(SOUND_ROBOT_DIE);
		findw()->increaseScore(points);
	}
}

bool A_bots::trytomove(int x, int y, int d)
{
	int a, b;
	navigate(a, b, d);
	x += a, y += b;
	if (findplayer(x, y, findw())) return false;

	std::vector<Actor*> item;
	for (int i = 0;i < findw()->locate(x, y, item);i++)
	{
		if (item[i]->findpl() != 1)	return false;	// =0, cannot land; =2, marble
	}

	moveTo(x, y);
	return true;
}

bool A_bots::trytoshoot(int x, int y)
{
	int d = getDirection();
	int a, b;
	navigate(a, b, d);
	x += a, y += b;
	// shoot if needed
	if (x == findw()->locatep()->getX() || y == findw()->locatep()->getY())
	{
		while (x >= 0 && y >= 0 && x < VIEW_WIDTH && y < VIEW_HEIGHT)
		{
			if (findplayer(x, y, findw()))
			{
				findw()->playSound(SOUND_ENEMY_FIRE);
				Actor* pea = new A_pea(IID_PEA, getX() + a, getY() + b, findw(), d);
				findw()->addmember(pea);
				return true;
			}
			if (findw()->locateobstacle(x, y)) break;
			x += a, y += b;
		}
	}

	return false;
}

A_ragebot::A_ragebot(int imageID, int startX, int startY, StudentWorld* wd, int startDirection)
:A_bots(imageID, startX, startY, wd, startDirection, 10, 100)
{
}

void A_ragebot::doSomething()
{
	if (findh() <= 0) return;
	A_bots::doSomething();
	if (!canmove()) return;

	int x = getX(), y = getY();
	int d = getDirection();

	// try to shoot
	if (trytoshoot(x, y)) return;

	//try to move
	if (!trytomove(x, y, d))	// if trytomove() returns true, then moving occurs already
	{
		setDirection((getDirection() + 180) % 360);
	}
}

A_thiefbot::A_thiefbot(int imageID, int startX, int startY, StudentWorld* wd)
:A_bots(imageID, startX, startY, wd, right, 5, 10)
{
	startup();
}

void A_thiefbot::doSomething()
{
	if (findh() <= 0) return;
	A_bots::doSomething();
	if (!canmove()) return;

	if (pickup()) return;	// try to pick up a goodie
	move();
}

bool A_thiefbot::pickup()
{
	if (object == nullptr)
	{
		std::vector<Actor*> item;
		for (int i = 0;i < findw()->locate(getX(), getY(), item);i++)
		{
			if (item[i]->isgoodie() && item[i]->findh() == 1000 && randInt(1, 10) == 1)
			{
				item[i]->setVisible(false);
				item[i]->changeh(2000);
				findw()->playSound(SOUND_ROBOT_MUNCH);
				object = item[i];
				return true;				
			}
		}
	}

	return false;
}

void A_thiefbot::move()
{
	int x = getX(), y = getY();
	if (distance_moved < distance_before_turning)	// try to move
	{
		if (trytomove(x, y, getDirection()))
		{
			distance_moved++;
			return;
		}
	}
	// else, choose a new distance & direction
	distance_before_turning = randInt(1, 6);
	distance_moved = 0;
	setDirection(randInt(0, 3) * 90);
	for (int i = 0;i < 4;i++)
	{
		if (trytomove(x, y, getDirection()))
		{
			distance_moved++;
			return;
		}
		setDirection((getDirection() + 90) % 360);
	}
}

void A_thiefbot::attacked()
{
	A_bots::attacked();
	if (findh() <= 0)
	{
		if (object != nullptr)
		{
			object->setVisible(true);
			object->changeh(1000);
			object->moveTo(getX(), getY());
		}
	}
}

void A_thiefbot::startup()
{
	object = nullptr;
	distance_moved=0;
	distance_before_turning = randInt(1, 6);
}

A_meanthiefbot::A_meanthiefbot(int imageID, int startX, int startY, StudentWorld* wd)
:A_thiefbot(imageID, startX, startY, wd)
{
	startup();
	changeh(8);
	changepoints(20);
}

void A_meanthiefbot::doSomething()
{
	if (findh() <= 0) return;
	A_bots::doSomething();
	if (!canmove()) return;

	if (trytoshoot(getX(),getY())) return;
	if (pickup()) return;
	move();

	// here, if we call thiefbot::doSomething, it will call bot::doSomething, which will
	// increment distance_moved twice; so just state out pickup() and move()
}

/////////////////////////////////////// A_factory

A_factory::A_factory(int imageID, int startX, int startY, StudentWorld* wd, bool tp)
:Actor(imageID, startX, startY, wd, none, 1000), type(tp)
{
	setVisible(true);
}

void A_factory::doSomething()
{
	if (findw()->canproduce(getX(),getY()) && randInt(1,50)==1)
	{
		Actor* ob;
		if (type) ob = new A_meanthiefbot(IID_MEAN_THIEFBOT, getX(), getY(), findw());
		else ob = new A_thiefbot(IID_MEAN_THIEFBOT, getX(), getY(), findw());
		findw()->addmember(ob);
		findw()->playSound(SOUND_ROBOT_BORN);
	}
}

/////////////////////////////////////// Helper function

// helper function to check if the player is on the spot
const bool findplayer(int x,int y,StudentWorld* wd)
{
	Actor* temp = wd->locatep();
	return (temp->getX() == x && temp->getY() == y);
}