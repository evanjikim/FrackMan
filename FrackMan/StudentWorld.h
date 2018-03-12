#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Dirt;
class FrackMan;
class Actor;
class GraphObject;

int randInt(int min, int max);

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);

	~StudentWorld();

	virtual int init();

	virtual int move();

	virtual void cleanUp();

	bool clearOfDirt(int x, int y) const;

	void setDisplayText();

	string format(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrelsLeft);

	void foundOil();

	void giveFrackManSonar();

	void giveFrackManGold();

	void giveFrackManWater();

	void addActor(Actor* a);

	void clearDirt(int x, int y);

	bool isDirt(int x, int y) const;

	void removeDirt(int x, int y);	

	bool radius(Actor * a, int x, int y, int radius) const;

	bool canActorMoveTo(Actor* a, int x, int y) const;

	int vtrNum() const;

	Actor* frack() const;

	Actor* findNearbyFrackMan(Actor* a, int r) const;

	Actor* findNearbyPickerUpper(Actor* a, int r) const;

	Actor* nearFrackMan(int r) const;

	bool lineOfSightToFrackMan(Actor* a) const;

	void findEmptySpot(int &x, int &y);

private:
	Dirt* m_dirt[64][60];
	FrackMan* m_frackman;
	vector<Actor*> vtr;
	int m_oil;
	int m_pTicks;
	int m_numTillNextP;
	int probabilityOfHardcore;
	int m_numProtesters;
	int m_targetProtesters;
};

#endif // STUDENTWORLD_H_
