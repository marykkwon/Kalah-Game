
#include "Board.h"

Board::Board(int nHoles, int nInitialBeansPerHole)
	:m_nholes(nHoles), m_nbeans(nInitialBeansPerHole)
{
	if (nHoles <= 0)
	{
		nHoles = 1;
		m_nholes = nHoles;
	}
	if (nInitialBeansPerHole < 0)
	{
		nInitialBeansPerHole = 0;
		m_nbeans = nInitialBeansPerHole;
	}
	m_north = new int[m_nholes + 1];
	m_south = new int[m_nholes + 1];
	//pot
	m_north[POT] = 0;
	m_south[POT] = 0;
	//holes
	for (int i = 1; i < m_nholes + 1; i++)
	{
		m_north[i] = m_nbeans;
		m_south[i] = m_nbeans;
	}//end for
	
}//end Board
//Construct a Board with the indicated number of holes per side(not counting the pot) and initial number of beans per hole.
//If nHoles is not positive, act as if it were 1; if nInitialBeansPerHole is negative, act as if it were 0.


Board::Board(const Board& other)
{
	m_nholes = other.m_nholes;
	m_nbeans = other.m_nbeans;

	m_north = new int[m_nholes + 1];
	m_south = new int[m_nholes + 1];

	for (int i = 0; i < other.m_nholes + 1; i++)
	{
		m_north[i] = other.m_north[i];
		m_south[i] = other.m_south[i];
	}
}


int Board::holes() const
{
	return m_nholes;
}
//Return the number of holes on a side(not counting the pot).

int Board::beans(Side s, int hole) const
{
	if (s == NORTH && hole <= m_nholes)
	{
		return m_north[hole];
	}
	else if (s == SOUTH && hole <= m_nholes)
	{
		return m_south[hole];
	}
	else
		return -1;
}
//Return the number of beans in the indicated hole or pot, or −1 if the hole number is invalid.

int Board::beansInPlay(Side s) const
{
	int count = 0;
	if (s == NORTH)
	{
		for (int i = 1; i < m_nholes + 1; i++)
		{
			count += m_north[i];
		}//end for
	}//end if s == north
	else if (s == SOUTH)
	{
		for (int i = 1; i < m_nholes + 1; i++)
		{
			count += m_south[i];
		}//end for
	}//end if s == south
	return count;
}
//Return the total number of beans in all the holes on the indicated side, not counting the beans in the pot.

int Board::totalBeans() const
{
	int count = 0;
	for (int i = 0; i < m_nholes + 1; i++)
	{
		count += m_south[i];
		count += m_north[i];
	}
	return count;
}
//Return the total number of beans in the game, including any in the pots.

bool Board::sow(Side s, int hole, Side& endSide, int& endHole)
{
	int count = 0;
	if (beans(s, hole) == 0 || hole > m_nholes || hole < 1)
		return false;
	if (s == NORTH)
	{
		count = m_north[hole];
		m_north[hole] = 0;
	}
		
	else
	{
		count = m_south[hole];
		m_south[hole] = 0;
	}
	endHole = hole;
	endSide = s;
	for (; count != 0; count--)
	{
		if (endHole == 0)
		{
			if (endSide == SOUTH)
			{
				endHole = m_nholes;
			}
			else
			{
				endHole = 1;
			}
			endSide = opponent(endSide);
		}
		else if (endHole == m_nholes && endSide == SOUTH && s == SOUTH)

			endHole = 0;
		else if (endHole == m_nholes && endSide == SOUTH && s == NORTH)
		{
			endHole = m_nholes;
			endSide = opponent(endSide);
		}

		else if (endHole == 1 && endSide == NORTH && s == NORTH)

			endHole = 0;
		else if (endHole == 1 && endSide == NORTH && s == SOUTH)
		{
			endHole = 1; //if we originally are on the opposite side, skip the hole.
			endSide = opponent(endSide);
		}
		else
		{
			if (endSide == NORTH)
				endHole += -1;
			else
				endHole += 1;//in all other cases, decrement if it were north, increment otherwise
		}

		if (endSide == NORTH && s == SOUTH)
			m_north[endHole]++;
		else if (endSide == SOUTH && s == SOUTH)
			m_south[endHole]++;
		else if (endSide == NORTH && s == NORTH)
			m_north[endHole]++;
		else if (endSide == SOUTH && s == NORTH)
			m_south[endHole]++;
		
	}
return true;
}//end sow
//If the hole indicated by(s, hole) is empty or invalid or a pot, this function returns false without changing anything.
//Otherwise, it will return true after sowing the beans : the beans are removed from hole(s, hole) and sown counterclockwise, 
//including s's pot if encountered, but skipping s's opponent's pot. 
//The parameters endSide and endHole are set to the side and hole where the last bean was placed. 
//(This function does not make captures or multiple turns; 
//different Kalah variants have different rules about these issues, so dealing with them should not be the responsibility of the Board class.)

bool Board::moveToPot(Side s, int hole, Side potOwner)
{
	if (hole > m_nholes || hole < 1 || beans(s, hole) == 0)
		return false;
	if (s == NORTH && potOwner == NORTH)
	{
		m_north[POT] += m_north[hole];
		m_north[hole] = 0;
	}

	else if (s == NORTH && potOwner == SOUTH)
	{
		m_south[POT] += m_north[hole];
		m_north[hole] = 0;
	}
	else if (s == SOUTH && potOwner == NORTH)
	{
		m_north[POT] += m_south[hole];
		m_south[hole] = 0;
	}
	else if (s == SOUTH && potOwner == SOUTH)
	{
		m_south[POT] += m_south[hole];
		m_south[hole] = 0;
	}
	return true;

}
//If the indicated hole is invalid or a pot, return false without changing anything.
//Otherwise, move all the beans in hole(s, hole) into the pot belonging to potOwnerand return true.


bool Board::setBeans(Side s, int hole, int beans)
{
	if (hole < 0 || hole > m_nholes || beans < 0)
		return false;
	if (s == NORTH)
	{
		m_north[hole] = beans;
	}
	else
		m_south[hole] = beans;
	return true;
}
//If the indicated hole is invalid or beans is negative, 
//this function returns false without changing anything.
//Otherwise, it will return true after setting the number of beans in the indicated hole or pot to the value of the third parameter. 
//(This may change what beansInPlayand totalBeans return if they are called later.) 
//This function exists solely so that weand you can more easily test your program : 
//None of your code that implements the member functions of any class is allowed to call this function directly or indirectly. 
//(We'll show an example of its use below.)


