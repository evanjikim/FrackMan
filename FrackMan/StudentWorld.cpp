#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <algorithm>
#include <random>
#include <utility>
#include <cstdlib>
using namespace std;

int randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 60; j++)
		{
			m_dirt[i][j] = nullptr;
		}
	m_frackman = nullptr;
	m_oil = 0;
	int t = 200 - getLevel();
	m_pTicks = max(25, t);
	m_numTillNextP = m_pTicks;
	int p = getLevel() * 10 + 30;
	probabilityOfHardcore = min(90, p);
	int x = 2 + getLevel()*1.5;
	m_numProtesters = 0;
	m_targetProtesters = min(15, x);
}

StudentWorld::~StudentWorld()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 60; j++)
		{
			delete m_dirt[i][j];
			m_dirt[i][j] = nullptr;
		}
	delete m_frackman;
}

int StudentWorld::init()
{
	m_frackman = new FrackMan(this);
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 60; j++)
			m_dirt[i][j] = new Dirt(this, i, j);
	for (int i = 30; i <= 33; i++)
		for (int j = 4; j <= 59; j++)
		{
			delete m_dirt[i][j];
			m_dirt[i][j] = nullptr;
		}
	Protester * p;
	int random = randInt(1, 100);
	if (random >= probabilityOfHardcore)
		p = new RegularProtester(this, 60, 60);
	else
		p = new HardcoreProtester(this, 60, 60);
	vtr.push_back(p);
	m_numProtesters++;
	int b = getLevel() / 2 + 2;
	int B = min(b, 6);
	for (int i = 0; i < B; i++)
	{
		int randX = randInt(0, 60);
		int randY = randInt(20, 56);
		bool nextTo = false;
		if (randX >= 27 && randX <= 33)
			if (randY > 0)
			{
				i--;
				continue;
			}
		for (int j = 0; j < vtr.size(); j++)
		{
			if (vtr[j] != nullptr && radius(vtr[j], randX, randY, 6))
			{
				i--;
				nextTo = true;
			}
		}
		if (!nextTo)
		{
			Boulder* m_boulder = new Boulder(this, randX, randY);
			vtr.push_back(m_boulder);
		}
	}
	int g = 5 - getLevel();
	int G = max(g, 2);
	for (int i = 0; i < G; i++)
	{
		int randX = randInt(0, 60);
		int randY = randInt(0, 56);
		bool nextTo = false;
		if (randX >= 27 && randX <= 33)
			if (randY > 0)
			{
				i--;
				continue;
			}
		for (int j = 0; j < vtr.size(); j++)
		{
			if (vtr[j] != nullptr && radius(vtr[j], randX, randY, 6))
			{
				i--;
				nextTo = true;
				break;
			}
		}
		if (!nextTo)
		{
			GoldNugget* m_nugget = new GoldNugget(this, randX, randY, false);
			vtr.push_back(m_nugget);
		}
	}
	int l = 2 + getLevel();
	int L = min(l, 20);
	m_oil = L;
	for (int i = 0; i < L; i++)
	{
		int randX = randInt(0, 60);
		int randY = randInt(0, 56);
		bool nextTo = false;
		if (randX >= 27 && randX <= 33)
			if (randY > 0)
			{
				i--;
				continue;
			}
		for (int j = 0; j < vtr.size(); j++)
		{
			if (vtr[j] != nullptr && radius(vtr[j], randX, randY, 6))
			{
				i--;
				nextTo = true;
				break;
			}
		}
		if (!nextTo)
		{
			OilBarrel* m_barrel = new OilBarrel(this, randX, randY);
			vtr.push_back(m_barrel);
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	setDisplayText();
	m_frackman->doSomething();
	for (int i = 0; i < vtr.size(); i++)
		if (vtr[i] != nullptr)
			vtr[i]->doSomething();
	if (!m_frackman->isAlive())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	else if (m_oil == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	for (int i = 0; i < vtr.size(); i++)
	{
		if (vtr[i] != nullptr && !vtr[i]->isAlive())
		{
			delete vtr[i];
			vtr[i] = nullptr;
			break;
		}
	}
	vector<Actor *> ::iterator it;
	it = vtr.begin();
	while (it != vtr.end())
	{
		if (*it == nullptr)
			delete *it;
		it++;
	}

	if (m_numTillNextP == 0 && m_numProtesters < m_targetProtesters)
	{
		RegularProtester * p;
		int random = randInt(1, 100);
		if (random >= probabilityOfHardcore)
			p = new RegularProtester(this, 60, 60);
		else
			p = new RegularProtester(this, 60, 60); // hardcore
		vtr.push_back(p);
		m_numTillNextP = m_pTicks + 1;
		m_numProtesters++;
	}
	m_numTillNextP--;
	int G = getLevel() * 25 + 300;
	int e = randInt(1, G);
	if (e == 1)
	{
		int x = randInt(1, 5);
		if (x > 1)
		{
			int z, y;
			findEmptySpot(z, y);
			WaterPool * w = new WaterPool(this, z, y);
			vtr.push_back(w);
		}
		else
		{
			SonarKit * s = new SonarKit(this, 0, 60);
			vtr.push_back(s);
		}
	}
	return GWSTATUS_CONTINUE_GAME;
}

bool StudentWorld::clearOfDirt(int x, int y) const
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (y + j >= 60)
			{
				continue;
			}
			if (isDirt(x + i, y + j))
				return false;
		}
	return true;
}
void StudentWorld::cleanUp()
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 60; j++)
		{
			delete m_dirt[i][j];
			m_dirt[i][j] = nullptr;
		}
	delete m_frackman;
	for (int i = 0; i < vtr.size(); i++)
	{
		if (vtr[i] != nullptr)
		{
			delete vtr[i];
			vtr[i] = nullptr;
		}
	}
	vector<Actor *> ::iterator it;
	it = vtr.begin();
	while (it != vtr.end())
	{
		delete *it;
		it++;
	}
}

void StudentWorld::setDisplayText()
{
	int score = getScore();
	int level = getLevel();
	int lives = getLives();
	int health = m_frackman->health() * 10;
	int squirts = m_frackman->getWater();
	int gold = m_frackman->getGold();
	int sonar = m_frackman->getSonar();
	int barrelsLeft = m_oil;

	string s = format(score, level, lives, health, squirts, gold, sonar, barrelsLeft);

	setGameStatText(s);
}

string StudentWorld::format(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrelsLeft)
{
	string scoreText = "Scr: " + string(6 - to_string(score).length(), '0') + to_string(score) + " ";
	string levelText = "Lvl: " + string(2 - to_string(level).length(), ' ') + to_string(level) + " ";
	string livesText = "Lives: " + to_string(lives) + " ";
	string healthText = "Hlth: " + string(3 - to_string(health).length(), ' ') + to_string(health) + "%" + " ";
	string squirtsText = "Wtr: " + string(2 - to_string(squirts).length(), ' ') + to_string(squirts) + " ";
	string goldText = "Gld: " + string(2 - to_string(gold).length(), ' ') + to_string(gold) + " ";
	string sonarText = "Sonar: " + string(2 - to_string(sonar).length(), ' ') + to_string(sonar) + " ";
	string barrelsText = "Oil: " + string(2 - to_string(barrelsLeft).length(), ' ') + to_string(barrelsLeft);

	return scoreText + levelText + livesText + healthText + squirtsText + goldText + sonarText + barrelsText;
}

void StudentWorld::foundOil()
{
	m_oil--;
}

void StudentWorld::giveFrackManSonar()
{
	m_frackman->addSonar();
}

void StudentWorld::giveFrackManGold()
{
	m_frackman->addGold();
}

void StudentWorld::giveFrackManWater()
{
	m_frackman->addWater();
}

void StudentWorld::addActor(Actor * a)
{
	vtr.push_back(a);
}

void StudentWorld::clearDirt(int x, int y)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (y + j >= 60)
			{
				continue;
			}
			if (isDirt(x + i, y + j))
			{
				removeDirt(x + i, y + j);
				m_dirt[x + i][y + j] = nullptr;
			}
		}
}

bool StudentWorld::isDirt(int x, int y) const
{
	if (m_dirt[x][y] != nullptr)
		return true;
	return false;
}

void StudentWorld::removeDirt(int x, int y)
{
	if (m_dirt[x][y] != nullptr)
	{
		//return;
		delete m_dirt[x][y];
		m_dirt[x][y] = nullptr;
		playSound(SOUND_DIG);
	}
}

bool StudentWorld::radius(Actor * a, int x, int y, int radius) const
{
	/*double midX1 = (double(x) + double(x) + 3) / 2;
	double midY1 = (double(y) + double(y) + 3) / 2;
	double midX2 = (double(a->getX()) + double(a->getX()) + 3) / 2;
	double midY2 = (double(a->getY()) + double(a->getY()) + 3) / 2;
	double distance = sqrt(pow(midX2 - midX1, 2) + pow(midY2 - midX2, 2));
	if (distance > 0 && distance <= radius)
		return true;
	return false;*/
	if (a->getX() == x + 1 && a->getY() == y)
		return false;
	if (a->getX() == x - 1 && a->getY() == y)
		return false;
	if (a->getX() == x && a->getY() == y + 1)
		return false;
	if (a->getX() == x && a->getY() == y - 1)
		return false;
	double distance = sqrt(pow(double(a->getX()) - double(x), 2) + pow(double(a->getY()) - double(y), 2));
	if (distance > 0 && distance <= radius)
		return true;
	return false;
	/*if (a->getX() == x + 1 && a->getY() == y)
		return false;
	if (a->getX() == x - 1 && a->getY() == y)
		return false;
	if (a->getX() == x && a->getY() == y + 1)
		return false;
	if (a->getX() == x && a->getY() == y - 1)
		return false;
	if (a->getX() >= x - radius && (a->getX()) <= x + radius)
		if (a->getY() >= y - radius && (a->getY()) <= y + radius)
			return true;
	return false;*/
}

bool StudentWorld::canActorMoveTo(Actor * a, int x, int y) const
{
	if (x < 0 || y < 0 || x > 60 || y > 60)
		return false;
	for (int i = 0; i < vtr.size(); i++)
		if (vtr[i] != nullptr && radius(vtr[i], x, y, 3) && !vtr[i]->canActorPassThroughMe())
			return false;
	if (radius(m_frackman, x, y, 3))
		return true;
	if (!clearOfDirt(x, y))
	{
		if (a->canDigThroughDirt())
			return true;
		else
			return false;
	}
	return true;
}

int StudentWorld::vtrNum() const
{
	return vtr.size();
}

Actor * StudentWorld::frack() const
{
	return m_frackman;
}

Actor * StudentWorld::findNearbyFrackMan(Actor * a, int r) const
{
	if (radius(m_frackman, a->getX(), a->getY(), r))
		return m_frackman;
	return nullptr;
}

Actor * StudentWorld::findNearbyPickerUpper(Actor * a, int r) const
{
	for (int i = 0; i < vtr.size(); i++)
	{
		if (vtr[i] != nullptr && vtr[i]->canPickThingsUp() && radius(vtr[i], a->getX(), a->getY(), r))
		{
			return vtr[i];
		}
	}
	return nullptr;
}

Actor * StudentWorld::nearFrackMan(int r) const
{
	for (int i = 0; i < vtr.size(); i++)
	{
		if (vtr[i] != nullptr && radius(vtr[i], m_frackman->getX(), m_frackman->getY(), r))
		{
			return vtr[i];
		}

	}
	return nullptr;
}


bool StudentWorld::lineOfSightToFrackMan(Actor * a) const
{
	for (int i = a->getX(); i <= 60; i++)
	{
		if (m_frackman->getY() == a->getY() && m_frackman->getX() > a->getX() && m_frackman->getX() <= 60)
		{
			for (int j = 0; j <= 3; j++)
			{
				if (a->getY() + j >= 60)
				{
					continue;
				}
				if (isDirt(i, a->getY() + j))
					return false;
			}
			for (int k = 0; k < vtr.size(); k++)
				if (vtr[k] != nullptr && !vtr[k]->canActorPassThroughMe())
					for (int z = -3; z <= 3; z++)
						if (vtr[k]->getY() + z == a->getY() && vtr[k]->getX() == i)
							return false;
			a->setDirection(GraphObject::right);
			if (canActorMoveTo(a, a->getX() + 1, a->getY()))
				a->moveTo(a->getX() + 1, a->getY());
			return true;
		}
	}
	for (int i = 0; i <= a->getX(); i++)
	{
		if (m_frackman->getY() == a->getY() && m_frackman->getX() > 0 && m_frackman->getX() <= a->getX())
		{
			for (int j = 0; j <= 3; j++)
			{
				if (a->getY() + j >= 60)
				{
					continue;
				}
				if (isDirt(i, a->getY() + j))
					return false;
			}
			for (int k = 0; k < vtr.size(); k++)
				if (vtr[k] != nullptr && !vtr[k]->canActorPassThroughMe())
					for (int z = -3; z <= 3; z++)
						if (vtr[k]->getY() + z == a->getY() && vtr[k]->getX() == i)
							return false;
			a->setDirection(GraphObject::left);
			if (canActorMoveTo(a, a->getX() - 1, a->getY()))
				a->moveTo(a->getX() - 1, a->getY());
			return true;
		}
	}
	for (int i = a->getY(); i <= 60; i++)
	{
		if (m_frackman->getX() == a->getX() && m_frackman->getY() > a->getY() && m_frackman->getY() <= 60)
		{
			for (int j = 0; j <= 3; j++)
			{
				if (a->getY() >= 60)
				{
					continue;
				}
				if (isDirt(a->getX() + j, i))
					return false;
			}
			for (int k = 0; k < vtr.size(); k++)
				if (vtr[k] != nullptr && !vtr[k]->canActorPassThroughMe())
					for (int z = -3; z <= 3; z++)
						if (vtr[k]->getX() + z == a->getX() && vtr[k]->getY() == i)
							return false;
			a->setDirection(GraphObject::up);
			if (canActorMoveTo(a, a->getX(), a->getY() + 1))
				a->moveTo(a->getX(), a->getY() + 1);
			return true;
		}
	}
	for (int i = 0; i <= a->getY(); i++)
	{
		if (m_frackman->getX() == a->getX() && m_frackman->getY() > 0 && m_frackman->getY() <= a->getY())
		{
			for (int j = 0; j <= 3; j++)
			{
				if (a->getY() >= 60)
				{
					continue;
				}
				if (isDirt(a->getX() + j, i))
					return false;
			}
			for (int k = 0; k < vtr.size(); k++)
				if (vtr[k] != nullptr && !vtr[k]->canActorPassThroughMe())
					for (int z = -3; z <= 3; z++)
						if (vtr[k]->getX() + z == a->getX() && vtr[k
						]->getY() == i)
							return false;
			a->setDirection(GraphObject::down);
			if (canActorMoveTo(a, a->getX(), a->getY() - 1))
				a->moveTo(a->getX(), a->getY() - 1);
			return true;
		}
	}
	return false;
}

void StudentWorld::findEmptySpot(int & x, int & y)
{
	int i = randInt(1, 4);
	while (true)
	{
		switch (i)
		{
		case 1:
			x = randInt(0, 30);
			y = randInt(0, 30);
			if (clearOfDirt(x, y))
				return;
		case 2:
			x = randInt(30, 60);
			y = randInt(0, 30);
			if (clearOfDirt(x, y))
				return;
		case 3:
			x = randInt(0, 30);
			y = randInt(30, 60);
			if (clearOfDirt(x, y))
				return;
		case 4:
			x = randInt(30, 60);
			y = randInt(30, 60);
			if (clearOfDirt(x, y))
				return;
		}
	}
}


