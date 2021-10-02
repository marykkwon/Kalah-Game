#include "Player.h"


//==========================================================================
// AlarmClock ac(numMilliseconds);  // Set an alarm clock that will time out
//                                  // after the indicated number of ms
// if (ac.timedOut())  // Will be false until the alarm clock times out; after
//                     // that, always returns true.
//==========================================================================

#include <chrono>
#include <future>
#include <atomic>

class AlarmClock
{
public:
	AlarmClock(int ms)
	{
		m_timedOut = false;
		m_isRunning = true;
		m_alarmClockFuture = std::async([=]() {
			for (int k = 0; k < ms && m_isRunning; k++)
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (m_isRunning)
				m_timedOut = true;
			});
	}

	~AlarmClock()
	{
		m_isRunning = false;
		m_alarmClockFuture.get();
	}

	bool timedOut() const
	{
		return m_timedOut;
	}

	AlarmClock(const AlarmClock&) = delete;
	AlarmClock& operator=(const AlarmClock&) = delete;
private:
	std::atomic<bool> m_isRunning;
	std::atomic<bool> m_timedOut;
	std::future<void> m_alarmClockFuture;
};


//Player
Player::Player(std::string name)
	:m_name(name)
{}
//Create a Player with the indicated name.

std::string Player::name() const
{
	return m_name;
}
//Return the name of the player.

bool Player::isInteractive() const
{
	return false;
}
//Return false if the player is a computer player.Return true if the player is human.Most kinds of players will be computer players.

Player::~Player()
{}
//Since this class is designed as a base class, it should have a virtual destructor.


//HumanPlayer
HumanPlayer::HumanPlayer(std::string name)
	:Player(name)
{}

bool HumanPlayer::isInteractive() const
{
	return true;
}

int HumanPlayer::chooseMove(const Board& b, Side s) const
{
	//if beans are < 1 return -1
	if (b.beansInPlay(s) < 1)
		return -1;

	int in_hole;
	std::cout << "It's " << name() << "'s turn. Select a hole: ";
	std::cin >> in_hole;

	//check for valid hole
	while (in_hole < 1 || in_hole > b.holes())
	{
		std::cout << "Invalid hole number. Select a hole from 1 to " << b.holes() << "." << std::endl;
		std::cout << "It's " << name() << "'s turn. Select a hole: ";
		std::cin >> in_hole;
	}//end while invalid hole
	while (b.beans(s, in_hole) == 0)
	{
		std::cout << "There are no beans." << std::endl;
		std::cout << "It's " << name() << "'s turn. Select a hole: ";
		std::cin >> in_hole;
	}//end while no beans
	return in_hole;
}

HumanPlayer::~HumanPlayer()
{}


BadPlayer::BadPlayer(std::string name) : Player(name)
{}

int BadPlayer::chooseMove(const Board& b, Side s) const
{
	//check 
	if (b.beansInPlay(SOUTH) == 0 || b.beansInPlay(NORTH) == 0)
		return -1;

	//we do the 'leftmost' since it is very simple

	for (int i = 1; i < b.holes() + 1; ++i)
	{
		if (b.beans(s, i) != 0)
		{
			return i;
		}
	}//end for
	return -1;
}

BadPlayer::~BadPlayer()
{}

AlarmClock clk(4900);
//SmartPlayer

SmartPlayer::SmartPlayer(std::string name)
	: Player(name)
{}

int SmartPlayer::minimax(const Board& b, Side m_turn, Side me, Side other, int& bestHole, bool finish) const
{
	int value=-1;
	int endHole;
	int  v2;
	Side endSide;
	Board tempB(b);

	//if finished, return -1;
	if (finish)
	{
		return -1;
	}
		

		if (clk.timedOut())
		{
			value = b.beans(SOUTH, POT) - b.beans(NORTH, POT);
			return value;
		}

		if (b.beansInPlay(m_turn) == 0 || b.beansInPlay(opponent(m_turn)) == 0)
		{
			//difference between two pots
			value = b.beans(SOUTH, POT) - b.beans(NORTH, POT);
			return value;
		}
		//return the best one
		else if ((b.beansInPlay(m_turn) == 0 || b.beansInPlay(opponent(m_turn)) == 0) && (tempB.beansInPlay(m_turn) == 0 || tempB.beansInPlay(opponent(m_turn)) == 0))
		{
			for (int i = 0; i < tempB.holes() + 1; i++)
			{
				//move beans to pots
				tempB.moveToPot(opponent(m_turn), i, opponent(m_turn));
				tempB.moveToPot(m_turn, i, m_turn);
			}
		}//end if

		//for holes
		for (int i = 1; i < b.holes() + 1; i++)
		{
			if (b.beans(m_turn, i) == 0)
				continue;
			//sow on tempB board
			tempB.sow(m_turn, i, endSide, endHole);

			//change m_turn to opponent if it is on pot
			if (endSide == m_turn && endHole == 0)
			{
				m_turn = opponent(m_turn);
			}
			//if it is not on pot
			if (endHole != 0 && endSide == m_turn && tempB.beans(m_turn, endHole) == 1 && tempB.beans(opponent(m_turn), endHole) > 0)
			{
				tempB.moveToPot(m_turn, endHole, m_turn);
				tempB.moveToPot(opponent(m_turn), endHole, m_turn);
			}
			//value2
			v2 = minimax(tempB, opponent(m_turn), me, other, bestHole, finish);

			//compare values when m_turn is south which is for positive infinity
			if (m_turn == SOUTH && v2 > value)
			{
				bestHole = i;
				value = v2;
			}
			//compare values when m_turn is north which is for negative infinity
			else if (m_turn == NORTH && v2 < value)
			{
				bestHole = i;
				v2 = value;
			}//end else if
		}//end for
		return value;
	}

	int SmartPlayer::chooseMove(const Board& b, Side s) const
	{
		int bestHole = 0;
		int min = -10000;
		//int max = 10000;
		Side me = s;
		Side other = opponent(s);
		bool finish = false;

		int tempHole;
		int tempVal;
		int value = min;

		//Get the best hole by comparing values
		for (int i = 1; i < b.holes() + 1; i++)
		{
			AlarmClock clk(4900);
			if (!clk.timedOut())
			{
				tempVal = minimax(b, s, me, other, i, finish);
				//compare values 
				if (tempVal > value)
				{
					value = tempVal;
					tempHole = i;
				}
			}
		}

		bestHole = tempHole;
		if (b.beans(s, bestHole) == 0)
		{
			for (int i = 1; i < b.holes() + 1; i++)
			{
				if (b.beans(s, i) != 0)
				{
					bestHole = i;
					break;
				}
			}
		}
		return bestHole;
	}
	
	SmartPlayer::~SmartPlayer()
	{}


