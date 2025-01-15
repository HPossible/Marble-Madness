#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "Level.h"
#include "Actor.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), player(nullptr), bonus(1000), crystalnum(0), passgame(false)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp(); 
}

int StudentWorld::init()
{
    player = nullptr;
    bonus = 1000;
    crystalnum = 0;
    passgame = false;
    
    int l = getLevel();
    if (l == 100) return GWSTATUS_PLAYER_WON;
    string s;
    ostringstream oss;
    oss.fill('0');
    oss << "level" << setw(2) << l << ".txt";
    s = oss.str();

    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(s);
    if (result == Level::load_fail_file_not_found) return GWSTATUS_PLAYER_WON;
    if (result == Level::load_fail_bad_format) return GWSTATUS_LEVEL_ERROR;
    if (result == Level::load_success) loadactors(lev);
    
    return GWSTATUS_CONTINUE_GAME;
}
// loading the current level's maze from a data file
// constructing a presentation of the current level in StudentWorld object

// no next file / reached level 99, return _WON
// exist but not correct format, return_ERROR
// else return _GAME

int StudentWorld::move()
{
    updateDisplayText();
    int s = actor.size();       // so that at the tick something was created, it can't move
                                // otherwise peas will move 2 blocks at first, for example    
    player->doSomething();
    if (player->findh() <= 0)
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    if (passgame) return GWSTATUS_FINISHED_LEVEL;
    for (int i = 0;i < s;i++)
    {
        if (actor[i]->findh() > 0)
        {
            actor[i]->doSomething();
            if (player->findh() <= 0)
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (passgame) return GWSTATUS_FINISHED_LEVEL;
        }
    }

    actor.reserve(s); // I don't know if the vector reallocates when shrinking?
    vector<Actor*>::iterator i = actor.begin();
    for (;i != actor.end();)
    {
        if ((*i)->findh() <= 0)
        {
            delete (*i);
            i = actor.erase(i);
        }
        else i++;
    }       
    
    if (bonus) bonus--;

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete player;
    player = nullptr;
    for (int i = 0;i < actor.size();i++) delete actor[i];
    actor.clear();
}
// free all actors currently in the game


//////////////////////////////////////////////////////////////////
// helper functions


void StudentWorld::updateDisplayText()      // used in move()
{
    int score = getScore();
    int level = getLevel();
    int livesleft = getLives();

    string s;
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << score<<"  ";
    oss << "Level: " << setw(2) << level<<"  ";
    oss.fill(' ');
    oss << "Lives: " << setw(2) << livesleft << "  ";
    oss << "Health: " << setw(3) << player->findh()*5 << "%  ";
    oss << "Ammo: " << setw(3) << player->findpn() << "  ";
    oss << "Bonus: " << setw(4) << bonus;
    s = oss.str();

    setGameStatText(s);
}

void StudentWorld::loadactors(Level lev)       // used in init()
{
    for (int i = 0;i < VIEW_WIDTH;i++)
    {
        for (int j = 0;j < VIEW_HEIGHT;j++)
        {
            Level::MazeEntry object = lev.getContentsOf(i, j);
            Actor* ob=nullptr;
            
            switch (object)
            {
            case Level::empty:
                break;
            case Level::exit:
                ob = new A_exit(IID_EXIT, i, j, this);
                break;
            case Level::player:
                player = new A_player(IID_PLAYER, i, j, this);
                break;
            case Level::horiz_ragebot:
                ob = new A_ragebot(IID_RAGEBOT, i, j, this, 0); // right = 0
                break;
            case Level::vert_ragebot:
                ob = new A_ragebot(IID_RAGEBOT, i, j, this, 270); // down = 0
                break;
            case Level::thiefbot_factory:
                ob = new A_factory(IID_ROBOT_FACTORY, i, j, this, 0);
                break;
            case Level::mean_thiefbot_factory:
                ob = new A_factory(IID_ROBOT_FACTORY, i, j, this, 1);
                break;
            case Level::wall:
                ob = new A_wall(IID_WALL, i, j, this);
                break;
            case Level::marble:
                ob = new A_marble(IID_MARBLE, i, j, this);
                break;
            case Level::pit:
                ob = new A_pit(IID_PIT, i, j, this);
                break;
            case Level::crystal:
                ob = new A_crystal(IID_CRYSTAL, i, j, this);
                crystalnum++;
                break;
            case Level::restore_health:
                ob = new A_restorehealth(IID_RESTORE_HEALTH, i, j, this);
                break;
            case Level::extra_life:
                ob = new A_extralife(IID_EXTRA_LIFE, i, j, this);
                break;
            case Level::ammo:
                ob = new A_ammo(IID_AMMO, i, j, this);
                break;
            }

            if (ob != nullptr) actor.push_back(ob);
        }
    }
}

//int StudentWorld::locatenum(int x, int y)
//{
//    int ans = 0;
//    for (int i = 0;i < actor.size();i++)
//    {
//        if (actor[i]->getX() == x && actor[i]->getY() == y) ans++;
//    }
//    return ans;
//}
//
//Actor* StudentWorld::locate(int x, int y, int n)
//{
//    for (int i = 0;i < actor.size();i++)
//    {
//        if (actor[i]->getX() == x && actor[i]->getY() == y)
//        {
//            if(!n) return actor[i];
//            n--;
//        }
//    }
//    return nullptr;
//}

int StudentWorld::locate(int x, int y, vector<Actor*> &items)
{
    int ans = 0;
    for (int i = 0;i < actor.size();i++)
    {
        if (actor[i]->getX() == x && actor[i]->getY() == y)
        {
            ans++;
            items.push_back(actor[i]);
        }
    }
    return ans;
}

void StudentWorld::win()
{
    playSound(SOUND_FINISHED_LEVEL);
    increaseScore(2000 + bonus);
    passgame = true;
}

bool StudentWorld::locateobstacle(int x, int y)
{
    for (int i = 0;i < actor.size();i++)
    {
        if (actor[i]->getX() == x && actor[i]->getY() == y && actor[i]->isobstacle()) return true;
    }
    return false;
}

bool StudentWorld::canproduce(int x, int y) const
{
    std::vector<Actor*> item;
    int a1 = x - 3;
    int a2 = x + 3;
    int b1 = y - 3;
    int b2 = y + 3;

    int cnt = 0;
    for (int i = 0;i < actor.size();i++)
    {
        if (actor[i]->isbot())
        {
            int xx = actor[i]->getX(), yy = actor[i]->getY();
            if (x == xx && y == yy) return false;   // no bot on the factory
            if (a1 <= xx && xx <= a2 && b1 <= yy && yy <= b2) cnt++;
        }
    }
    if (cnt < 3) return true;
    return false;
}