#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* s, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0, int hitPoints = 1,bool visible = true);
	~Actor();
	virtual void doSomething() = 0;
	StudentWorld* getStudent() const;
	int health() const;
	virtual void addGold();
	virtual bool isGoldForProtester() const;
	virtual bool isAlive() const;
	virtual bool canActorPassThroughMe() const;
	virtual void getAnnoyed(int amt);
	virtual bool canDigThroughDirt() const;
	virtual bool canPickThingsUp() const;
	virtual bool huntsFrackman() const;
	void setDead();
private:
	int m_hitPoints;
	StudentWorld* m_student;
};

class Dirt : public Actor
{
public:
	Dirt(StudentWorld *s, int x, int y);
	virtual void doSomething();
	virtual bool canActorPassThroughMe() const;
private:
};
class Boulder : public Actor
{
public:
	Boulder(StudentWorld *s, int x, int y);
	virtual void doSomething();
	virtual bool canActorPassThroughMe() const;
	bool stableState();
	bool waitingState();
	bool fallingState();
private:
	int m_ticks;
	bool m_stable;
	bool m_waiting;
	bool m_falling;
};
class FrackMan :public Actor
{
public:
	FrackMan(StudentWorld *s);
	virtual void doSomething();
	virtual bool canDigThroughDirt() const;
	virtual bool canPickThingsUp() const;
	virtual void addGold();
	virtual void getAnnoyed(int amt);
	void addSonar();
	void addWater();
	int getGold() const;
	int getSonar() const;
	int getWater() const;
private:
	int m_water;
	int m_sonar;
	int m_nuggets;
};

class Squirt : public Actor
{
public:
	Squirt(StudentWorld *s, int x, int y, Direction startDir);
	virtual ~Squirt();
	virtual void doSomething();
private:
	int startX;
	int startY;
};
class ActivatingObject : public Actor
{
public:
	ActivatingObject(StudentWorld * s, int x, int y, int imageID, bool activateOnPlayer, bool activateOnProtester, bool initiallyVisible);
	virtual void doSomething() = 0;
	bool actByPlayer();
	bool actByProtester();
private:
	bool m_actPlayer;
	bool m_actProt;
};

class OilBarrel : public ActivatingObject
{
public:
	OilBarrel(StudentWorld * s, int x, int y);
	virtual void doSomething();
private:
	bool m_visible;
};

class GoldNugget : public ActivatingObject
{
public:
	GoldNugget(StudentWorld * s, int startX, int startY, bool temporary);
	virtual void doSomething();
	virtual bool isGoldForProtester() const;
private:
	bool m_temp;
	bool m_permanent;
	bool m_visible;
	int m_ticks;
};

class SonarKit : public ActivatingObject
{
public:
	SonarKit(StudentWorld * s, int startX, int startY);
	virtual void doSomething();
private:
	int m_ticks;
};

class WaterPool : public ActivatingObject
{
public:
	WaterPool(StudentWorld * s, int startX, int startY);
	virtual void doSomething();
private:
	int m_ticks;
};

class Protester :public Actor
{
public:
	Protester(StudentWorld * s, int startX, int startY, int imageID, int hitpoints);
	virtual void doSomething();
	virtual bool canPickThingsUp() const;
	virtual void addGold() = 0;
	virtual bool huntsFrackman() const;
	void setMustLeaveOilField();
	void stun();
	bool resting();
	bool leavingState() const;
	bool shout();
	int numSquaresLeft() const;
	void setNewnumOfSquares();
	void setNewDirection();
	void setNumOfSquaresToZero();
	int getTicksPerp() const;
	void resetPerp();
private:
	int ticksToWait;
	int m_ticks;
	bool m_leave;
	int m_ticksTillShout;
	int numSquaresToMoveInCurrenctDirection;
	int m_stun;
	int m_ticksPerp;
};

class RegularProtester : public Protester
{
public: 
	RegularProtester(StudentWorld * s, int startX, int startY);
	virtual void doSomething();
	virtual void addGold();
	virtual void getAnnoyed(int amt);
};

class HardcoreProtester : public Protester
{
public:
	HardcoreProtester(StudentWorld * s, int startX, int startY);
	virtual void doSomething();
	virtual void addGold();
	virtual void getAnnoyed(int amt);
};
#endif // ACTOR_H_
