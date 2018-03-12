#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* s, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, int hitpoints, bool visible)
	: GraphObject(imageID, startX, startY, dir, size, depth)
{
	m_student = s;
	setVisible(visible);
	m_hitPoints = hitpoints;
}

Actor::~Actor()
{
	setVisible(false);
}

StudentWorld * Actor::getStudent() const
{
	return m_student;
}

int Actor::health() const
{
	return m_hitPoints;
}

void Actor::addGold()
{
	return;
}

bool Actor::isGoldForProtester() const
{
	return false;
}

bool Actor::isAlive() const
{
	if (m_hitPoints <= 0)
		return false;
	return true;
}

bool Actor::canActorPassThroughMe() const
{
	return true;
}

void Actor::getAnnoyed(int amt)
{
	m_hitPoints -= amt;
}

bool Actor::canDigThroughDirt() const
{
	return false;
}

bool Actor::canPickThingsUp() const
{
	return false;
}

bool Actor::huntsFrackman() const
{
	return false;
}


void Actor::setDead()
{
	if (huntsFrackman())
		m_hitPoints = -1;
	m_hitPoints = 0;
	setVisible(false);
}


Dirt::Dirt(StudentWorld* s, int x, int y)
	:Actor(s, IID_DIRT, x, y, right, .25, 3)
{
}


void Dirt::doSomething()
{
	return;
}

bool Dirt::canActorPassThroughMe() const
{
	return false;
}

Boulder::Boulder(StudentWorld *s, int x, int y)
	:Actor(s, IID_BOULDER, x, y, down, 1, 1)
{
	m_stable = true;
	m_waiting = false;
	m_falling = false;
	m_ticks = 0;
	s->clearDirt(x, y);
}

void Boulder::doSomething()
{
	if (!isAlive())
		return;
	else if (m_stable)
	{
		for (int i = 0; i < 4; i++)
		{
			if (getStudent()->isDirt(getX() + i, getY() - 1))
				return;
		}
		m_stable = false;
		m_waiting = true;
		m_ticks++;
	}
	else if (m_waiting && m_ticks != 30)
		m_ticks++;
	else if (m_waiting && m_ticks == 30)
	{
		m_waiting = false;
		m_falling = true;
		getStudent()->playSound(SOUND_FALLING_ROCK);
	}
	else if (m_falling)
	{
		if (getY() - 1 < 0)
			setDead();
		else if (!getStudent()->canActorMoveTo(this, getX(), getY() - 1))
			setDead();
		else if (getStudent()->findNearbyPickerUpper(this, 3) != nullptr)
			getStudent()->findNearbyPickerUpper(this, 3)->getAnnoyed(100);
		else if (getStudent()->findNearbyFrackMan(this, 3))
			getStudent()->findNearbyFrackMan(this, 3)->getAnnoyed(100);
		else
			moveTo(getX(), getY() - 1);

	}

}
bool Boulder::canActorPassThroughMe() const
{
	return false;
}
bool Boulder::stableState()
{
	return m_stable;
}
bool Boulder::waitingState()
{
	return m_waiting;
}
bool Boulder::fallingState()
{
	return m_falling;
}
FrackMan::FrackMan(StudentWorld* s)
	:Actor(s, IID_PLAYER, 30, 60, right, 1, 0, 10)
{
	m_water = 5;
	m_sonar = 1;
	m_nuggets = 0;
}

bool FrackMan::canDigThroughDirt() const
{
	return true;
}
bool FrackMan::canPickThingsUp() const
{
	return true;
}
void FrackMan::addGold()
{
	m_nuggets++;
}
void FrackMan::getAnnoyed(int amt)
{
	Actor::getAnnoyed(amt);
	if (!isAlive())
	{
		setDead();
		getStudent()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}
void FrackMan::addSonar()
{
	m_sonar++;
}
void FrackMan::addWater()
{
	m_water += 5;
}
int FrackMan::getGold() const
{
	return m_nuggets;
}
int FrackMan::getSonar() const
{
	return m_sonar;
}
int FrackMan::getWater() const
{
	return m_water;
}
void FrackMan::doSomething()
{
	if (!isAlive())
		return;
	getStudent()->clearDirt(getX(), getY());
	int value;
	if (getStudent()->getKey(value))
	{
		switch (value)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
				setDirection(left);
			else if (getX() != 0 && getStudent()->canActorMoveTo(this, getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			return;
		case KEY_PRESS_UP:
			if (getDirection() != up)
				setDirection(up);
			else if (getY() != 60 && getStudent()->canActorMoveTo(this, getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			return;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			else if (getX() != 60 && getStudent()->canActorMoveTo(this, getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			return;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			else if (getY() != 0 && getStudent()->canActorMoveTo(this, getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			return;
		case KEY_PRESS_ESCAPE:
			setDead();
			return;
		case KEY_PRESS_SPACE:
			if (m_water != 0)
			{
				getStudent()->playSound(SOUND_PLAYER_SQUIRT);
				Squirt* m_squirt;
				switch (getDirection())
				{
				case up:
					if (getY() < 60)
					{
						m_squirt = new Squirt(getStudent(), getX(), getY() + 3, getDirection());
						getStudent()->addActor(m_squirt);
					}
					break;
				case right:
					if (getX() < 60)
					{
						m_squirt = new Squirt(getStudent(), getX() + 3, getY(), getDirection());
						getStudent()->addActor(m_squirt);
					}
					break;
				case down:
					if (getY() > 0)
					{
						m_squirt = new Squirt(getStudent(), getX(), getY() - 3, getDirection());
						getStudent()->addActor(m_squirt);
					}
					break;
				case left:
					if (getX() > 0)
					{
						m_squirt = new Squirt(getStudent(), getX() - 3, getY(), getDirection());
						getStudent()->addActor(m_squirt);
					}
					break;
				}
				m_water--;
				return;
		case 'Z':
		case 'z':
			if (m_sonar != 0)
			{
				getStudent()->playSound(SOUND_SONAR);
				for (int i = 0; i < getStudent()->vtrNum(); i++)
				{
					if (getStudent()->nearFrackMan(12) != nullptr)
						getStudent()->nearFrackMan(12)->setVisible(true);
				}
				m_sonar--;
			}
			return;
		case KEY_PRESS_TAB:
			if (m_nuggets != 0)
			{
				GoldNugget* nugget = new GoldNugget(getStudent(), getX(), getY(), true);
				getStudent()->addActor(nugget);
				m_nuggets--;
			}
			return;
			}
		}
	}

	return;
}

Squirt::Squirt(StudentWorld * s, int x, int y, Direction startDir)
	:Actor(s, IID_WATER_SPURT, x, y, startDir, 1, 1)
{
	startX = x;
	startY = y;
}

Squirt::~Squirt()
{
	setVisible(false);
}

void Squirt::doSomething()
{
	if (getStudent()->findNearbyPickerUpper(this, 3) != nullptr)
	{
		getStudent()->findNearbyPickerUpper(this, 3)->getAnnoyed(2);
		setDead();
	}
	if (getX() == startX - 4 || getX() == startX + 4 || getY() == startY - 4 || getY() == startY + 4)
		setDead();
	switch (getDirection())
	{
	case up:
		if (!getStudent()->canActorMoveTo(this, getX(), getY() + 1))
			setDead();
		moveTo(getX(), getY() + 1);
		return;
	case right:
		if (!getStudent()->canActorMoveTo(this, getX() + 1, getY()))
			setDead();
		moveTo(getX() + 1, getY());
		return;
	case down:
		if (!getStudent()->canActorMoveTo(this, getX(), getY() - 1))
			setDead();
		moveTo(getX(), getY() - 1);
		return;
	case left:
		if (!getStudent()->canActorMoveTo(this, getX() - 1, getY()))
			setDead();
		moveTo(getX() - 1, getY());
		return;
	}
}

ActivatingObject::ActivatingObject(StudentWorld * s, int x, int y, int imageID, bool activateOnPlayer, bool activateOnProtester, bool initiallyVisible)
	: Actor(s, imageID, x, y, right, 1, 2, 1, initiallyVisible)
{
	m_actPlayer = activateOnPlayer;
	m_actProt = activateOnProtester;
}

bool ActivatingObject::actByPlayer()
{
	return m_actPlayer;
}

bool ActivatingObject::actByProtester()
{
	return m_actProt;
}

OilBarrel::OilBarrel(StudentWorld * s, int x, int y)
	:ActivatingObject(s, x, y, IID_BARREL, true, false, false)
{
	m_visible = false;
}

void OilBarrel::doSomething()
{
	if (!isAlive())
		return;
	if (!m_visible && actByPlayer() && getStudent()->findNearbyFrackMan(this, 4))
	{
		m_visible = true;
		setVisible(true);
		return;
	}
	if (actByPlayer() && getStudent()->findNearbyFrackMan(this, 3))
	{
		setDead();
		getStudent()->playSound(SOUND_FOUND_OIL);
		getStudent()->increaseScore(1000);
		getStudent()->foundOil();
	}
}

GoldNugget::GoldNugget(StudentWorld * s, int startX, int startY, bool temporary)
	:ActivatingObject(s, startX, startY, IID_GOLD, !temporary, temporary, temporary)
{
	m_temp = temporary;
	m_permanent = !temporary;
	m_visible = temporary;
	m_ticks = 100;
}

void GoldNugget::doSomething()
{
	if (!isAlive())
		return;
	if (!m_visible && m_permanent && actByPlayer() && getStudent()->findNearbyFrackMan(this, 4))
	{
		m_visible = true;
		setVisible(true);
		return;
	}
	else if (m_permanent && getStudent()->findNearbyFrackMan(this, 3))
	{
		setDead();
		getStudent()->playSound(SOUND_GOT_GOODIE);
		getStudent()->increaseScore(10);
		getStudent()->giveFrackManGold();
	}
	else if (m_temp && actByProtester() && getStudent()->findNearbyPickerUpper(this, 3))
	{
		setDead();
		getStudent()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		getStudent()->findNearbyPickerUpper(this, 3)->addGold();
		/*The Gold Nugget must tell the Protester object that it just received a new
			Nugget so it can react appropriately(e.g., be bribed).
			d.The Gold Nugget increases the player’s score by 25 points(This increase can
				be performed by the Protester class or the Gold Nugget class).*/
	}
	if (m_temp)
	{
		if (m_ticks == 0)
			setDead();
		m_ticks--;
	}
}

bool GoldNugget::isGoldForProtester() const
{
	return m_temp;
}

SonarKit::SonarKit(StudentWorld * s, int startX, int startY)
	:ActivatingObject(s, startX, startY, IID_SONAR, true, false, true)
{
	int t = 300 - 10 * getStudent()->getLevel();
	m_ticks = max(100, t);
}

void SonarKit::doSomething()
{
	if (!isAlive())
		return;
	else if (actByPlayer() && getStudent()->findNearbyFrackMan(this, 3))
	{
		setDead();
		getStudent()->playSound(SOUND_GOT_GOODIE);
		getStudent()->giveFrackManSonar();
		getStudent()->increaseScore(75);
	}
	else
	{
		if (m_ticks == 0)
			setDead();
		m_ticks--;
	}
}

WaterPool::WaterPool(StudentWorld * s, int startX, int startY)
	:ActivatingObject(s, startX, startY, IID_WATER_POOL, true, false, true)
{
	int t = 300 - 10 * getStudent()->getLevel();
	m_ticks = max(100, t);
}

void WaterPool::doSomething()
{
	if (!isAlive())
		return;
	else if (actByPlayer() && getStudent()->findNearbyFrackMan(this, 3))
	{
		setDead();
		getStudent()->playSound(SOUND_GOT_GOODIE);
		getStudent()->giveFrackManWater();
		getStudent()->increaseScore(100);
	}
	else
	{
		if (m_ticks == 0)
			setDead();
		m_ticks--;
	}

}

Protester::Protester(StudentWorld * s, int startX, int startY, int imageID, int hitpoints)
	:Actor(s, imageID, startX, startY, left, 1, 0, hitpoints)
{
	int t = 3 - getStudent()->getLevel() / 4;
	ticksToWait = max(0, t);
	m_ticks = ticksToWait;
	m_leave = false;
	m_ticksTillShout = 15;
	int n = 100 - getStudent()->getLevel() * 10;
	m_stun = max(50, n);
	m_ticksPerp = 200;
}

void Protester::doSomething()
{
	numSquaresToMoveInCurrenctDirection--;
	m_ticksPerp--;
	m_ticksTillShout--;
}

bool Protester::canPickThingsUp() const
{
	return true;
}


bool Protester::huntsFrackman() const
{
	return true;
}

void Protester::setMustLeaveOilField()
{
	m_leave = true;
	ticksToWait = 0;
}

void Protester::stun()
{
	ticksToWait += m_stun;
}

bool Protester::resting()
{
	if (ticksToWait == 0)
	{
		ticksToWait = m_ticks;
		m_ticksTillShout--;
		return false;
	}
	else
	{
		ticksToWait--;
		return true;
	}
}

bool Protester::leavingState() const
{
	return m_leave;
}

bool Protester::shout()
{
	if (m_ticksTillShout <= 0)
	{
		m_ticksTillShout = 15;
		return true;
	}
	else
	{
		return false;
	}
}

int Protester::numSquaresLeft() const
{
	return numSquaresToMoveInCurrenctDirection;
}

void Protester::setNewnumOfSquares()
{
	numSquaresToMoveInCurrenctDirection = randInt(8, 60);
}

void Protester::setNewDirection()
{
	while (true)
	{
		switch (randInt(1, 4))
		{
		case 1:
			if (getDirection() == right || getDirection() == left)
				m_ticksPerp = 200;
			setDirection(up);
			if (getStudent()->canActorMoveTo(this, getX(), getY() + 1))
				return;
		case 2:
			if (getDirection() == up || getDirection() == down)
				m_ticksPerp = 200;
			setDirection(right);
			if (getStudent()->canActorMoveTo(this, getX() + 1, getY()))
				return;
		case 3:
			if (getDirection() == right || getDirection() == left)
				m_ticksPerp = 200;
			setDirection(down);
			if (getStudent()->canActorMoveTo(this, getX(), getY() - 1))

				return;
		case 4:
			if (getDirection() == up || getDirection() == down)
				m_ticksPerp = 200;
			setDirection(left);
			if (getStudent()->canActorMoveTo(this, getX() - 1, getY()))

				return;
		}
	}
}

void Protester::setNumOfSquaresToZero()
{
	numSquaresToMoveInCurrenctDirection = 0;
}

int Protester::getTicksPerp() const
{
	return m_ticksPerp;
}

void Protester::resetPerp()
{
	m_ticksPerp = 200;
}

RegularProtester::RegularProtester(StudentWorld * s, int startX, int startY)
	:Protester(s, startX, startY, IID_PROTESTER, 5)
{
	setNewnumOfSquares();
}

void RegularProtester::doSomething()
{
	if (!isAlive())
		return;
	if (resting())
		return;
	if (leavingState())
	{
		/*if (getX() == 60 && getY() == 60)*/
		setDead();
	}
	if (getStudent()->findNearbyFrackMan(this, 4) != nullptr && shout())
	{
		getStudent()->playSound(SOUND_PROTESTER_YELL);
		getStudent()->frack()->getAnnoyed(2);
		return;
	}
	if (getStudent()->findNearbyFrackMan(this, 4) == nullptr && getStudent()->lineOfSightToFrackMan(this))
	{
		setNumOfSquaresToZero();
		return;
	}
	Protester::doSomething();
	if (numSquaresLeft() <= 0)
	{
		setNewDirection();
		setNewnumOfSquares();
	}
	if (getTicksPerp() <= 0)
	{
		resetPerp();
		switch (getDirection())
		{
		case up:
			if (getStudent()->canActorMoveTo(this, getX() + 1, getY()) || getStudent()->canActorMoveTo(this, getX() + 1, getY()))
			{
				if (getStudent()->canActorMoveTo(this, getX() - 1, getY()) && getStudent()->canActorMoveTo(this, getX() + 1, getY()))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(right);
						break;
					case 2:
						setDirection(left);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX() + 1, getY()))
					setDirection(right);
				else
					setDirection(left);
			}
			break;
		case right:
			if (getStudent()->canActorMoveTo(this, getX(), getY()+1) || getStudent()->canActorMoveTo(this, getX(), getY()-1))
			{
				if (getStudent()->canActorMoveTo(this, getX(), getY()+1) && getStudent()->canActorMoveTo(this, getX(), getY()-1))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(up);
						break;
					case 2:
						setDirection(down);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX(), getY()+1))
					setDirection(up);
				else
					setDirection(down);
			}
			break;
		case down:
			if (getStudent()->canActorMoveTo(this, getX() + 1, getY()) || getStudent()->canActorMoveTo(this, getX() + 1, getY()))
			{
				if (getStudent()->canActorMoveTo(this, getX() - 1, getY()) && getStudent()->canActorMoveTo(this, getX() + 1, getY()))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(right);
						break;
					case 2:
						setDirection(left);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX() + 1, getY()))
					setDirection(right);
				else
					setDirection(left);
			}
			break;
		case left:
			if (getStudent()->canActorMoveTo(this, getX(), getY() + 1) || getStudent()->canActorMoveTo(this, getX(), getY() - 1))
			{
				if (getStudent()->canActorMoveTo(this, getX(), getY() + 1) && getStudent()->canActorMoveTo(this, getX(), getY() - 1))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(up);
						break;
					case 2:
						setDirection(down);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX(), getY() + 1))
					setDirection(up);
				else
					setDirection(down);
			}
			break;
		}
	}
	switch (getDirection())
	{
	case up:
		if (!getStudent()->canActorMoveTo(this, getX(), getY() + 1))
			setNumOfSquaresToZero();
		moveTo(getX(), getY() + 1);
		break;
	case right:
		if (!getStudent()->canActorMoveTo(this, getX() + 1, getY()))
			setNumOfSquaresToZero();
		moveTo(getX() + 1, getY());
		break;
	case down:
		if (!getStudent()->canActorMoveTo(this, getX(), getY() - 1))
			setNumOfSquaresToZero();
		moveTo(getX(), getY() - 1);
		break;
	case left:
		if (!getStudent()->canActorMoveTo(this, getX() - 1, getY()))
			setNumOfSquaresToZero();
		moveTo(getX() - 1, getY());
		break;
	}
	return;
}

void RegularProtester::addGold()
{
	getStudent()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getStudent()->increaseScore(25);
	setMustLeaveOilField();
}

void RegularProtester::getAnnoyed(int amt)
{
	if (leavingState())
		return;
	Actor::getAnnoyed(amt);
	getStudent()->playSound(SOUND_PROTESTER_ANNOYED);
	if (health() > 0)
		stun();
	else
	{
		setMustLeaveOilField();
		getStudent()->playSound(SOUND_PROTESTER_GIVE_UP);
		if (health() < -10)
			getStudent()->increaseScore(500);
		else
			getStudent()->increaseScore(100);
	}
}

HardcoreProtester::HardcoreProtester(StudentWorld * s, int startX, int startY)
	: Protester(s, startX, startY, IID_HARD_CORE_PROTESTER, 20)
{
	setNewnumOfSquares();
}

void HardcoreProtester::doSomething()
{
	if (!isAlive())
		return;
	if (resting())
		return;
	if (leavingState())
	{
		/*if (getX() == 60 && getY() == 60)*/
		setDead();
	}
	if (getStudent()->findNearbyFrackMan(this, 4) != nullptr && shout())
	{
		getStudent()->playSound(SOUND_PROTESTER_YELL);
		getStudent()->frack()->getAnnoyed(2);
		return;
	}
	if (getStudent()->findNearbyFrackMan(this, 4) == nullptr && getStudent()->lineOfSightToFrackMan(this))
	{
		setNumOfSquaresToZero();
		return;
	}
	Protester::doSomething();
	if (numSquaresLeft() <= 0)
	{
		setNewDirection();
		setNewnumOfSquares();
	}
	if (getTicksPerp() <= 0)
	{
		resetPerp();
		switch (getDirection())
		{
		case up:
			if (getStudent()->canActorMoveTo(this, getX() + 1, getY()) || getStudent()->canActorMoveTo(this, getX() + 1, getY()))
			{
				if (getStudent()->canActorMoveTo(this, getX() - 1, getY()) && getStudent()->canActorMoveTo(this, getX() + 1, getY()))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(right);
						break;
					case 2:
						setDirection(left);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX() + 1, getY()))
					setDirection(right);
				else
					setDirection(left);
			}
			break;
		case right:
			if (getStudent()->canActorMoveTo(this, getX(), getY() + 1) || getStudent()->canActorMoveTo(this, getX(), getY() - 1))
			{
				if (getStudent()->canActorMoveTo(this, getX(), getY() + 1) && getStudent()->canActorMoveTo(this, getX(), getY() - 1))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(up);
						break;
					case 2:
						setDirection(down);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX(), getY() + 1))
					setDirection(up);
				else
					setDirection(down);
			}
			break;
		case down:
			if (getStudent()->canActorMoveTo(this, getX() + 1, getY()) || getStudent()->canActorMoveTo(this, getX() + 1, getY()))
			{
				if (getStudent()->canActorMoveTo(this, getX() - 1, getY()) && getStudent()->canActorMoveTo(this, getX() + 1, getY()))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(right);
						break;
					case 2:
						setDirection(left);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX() + 1, getY()))
					setDirection(right);
				else
					setDirection(left);
			}
			break;
		case left:
			if (getStudent()->canActorMoveTo(this, getX(), getY() + 1) || getStudent()->canActorMoveTo(this, getX(), getY() - 1))
			{
				if (getStudent()->canActorMoveTo(this, getX(), getY() + 1) && getStudent()->canActorMoveTo(this, getX(), getY() - 1))
				{
					int r = randInt(1, 2);
					switch (r)
					{
					case 1:
						setDirection(up);
						break;
					case 2:
						setDirection(down);
						break;
					}
				}
				else if (getStudent()->canActorMoveTo(this, getX(), getY() + 1))
					setDirection(up);
				else
					setDirection(down);
			}
			break;
		}
	}
	switch (getDirection())
	{
	case up:
		if (!getStudent()->canActorMoveTo(this, getX(), getY() + 1))
			setNumOfSquaresToZero();
		moveTo(getX(), getY() + 1);
		break;
	case right:
		if (!getStudent()->canActorMoveTo(this, getX() + 1, getY()))
			setNumOfSquaresToZero();
		moveTo(getX() + 1, getY());
		break;
	case down:
		if (!getStudent()->canActorMoveTo(this, getX(), getY() - 1))
			setNumOfSquaresToZero();
		moveTo(getX(), getY() - 1);
		break;
	case left:
		if (!getStudent()->canActorMoveTo(this, getX() - 1, getY()))
			setNumOfSquaresToZero();
		moveTo(getX() - 1, getY());
		break;
	}
	return;
}

void HardcoreProtester::addGold()
{

	getStudent()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getStudent()->increaseScore(50);
	stun();
}

void HardcoreProtester::getAnnoyed(int amt)
{
	if (leavingState())
		return;
	Actor::getAnnoyed(amt);
	getStudent()->playSound(SOUND_PROTESTER_ANNOYED);
	if (health() > 0)
		stun();
	else
	{
		setMustLeaveOilField();
		getStudent()->playSound(SOUND_PROTESTER_GIVE_UP);
		if (health() < -10)
			getStudent()->increaseScore(500);
		else
			getStudent()->increaseScore(250);
	}
}
