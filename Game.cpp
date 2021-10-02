#include "Game.h"

Game::Game(const Board& b, Player* south, Player* north)
	: m_board(b), m_southP(south), m_northP(north), m_turn(SOUTH)
{}
//Construct a Game to be played with the indicated players on a copy of the board b.
//The player on the south side always moves first.

void Game::display() const
{
	//display north player name
	std::cout << "        " << m_northP->name() << std::endl;
	std::cout << "    ";
	//display north holes in one line
	for (int i = 1; i <= m_board.holes(); i++)
		std::cout << m_board.beans(NORTH, i) << "    ";
	//change the line
	std::cout << std::endl;
	//display north pot
	std::cout << "  " << m_board.beans(NORTH, POT);
	for (int i = 0; i < m_board.holes(); i++)
		std::cout << "    ";
	std::cout << m_board.beans(SOUTH, POT) << std::endl;
	//display south holes in one line
	for (int i = 1; i <= m_board.holes(); i++)
		std::cout << "    " << m_board.beans(SOUTH, i) ;
	//change the line
	std::cout << std::endl;
	//display south player name
	std::cout << "        " << m_southP->name() << std::endl;
}//end display
//Display the game's board in a manner of your choosing, 
//provided you show the names of the players and a reasonable representation of the state of the board.

void Game::status(bool& over, bool& hasWinner, Side& winner) const
{
	int northbean = m_board.beansInPlay(NORTH);
	int southbean = m_board.beansInPlay(SOUTH);
	int northpot = m_board.beans(NORTH, POT);
	int southpot = m_board.beans(SOUTH, POT);
	//when game is over
	if (northbean == 0 || southbean == 0)
	{
		over = true;
		//if tie, hasWinner is false
		if (southpot == northpot)
			hasWinner = false;
		//when not tie
		else
		{
			if (southpot > northpot)
				winner = SOUTH;
			else
				winner = NORTH;
		}//end else 
		if (winner == SOUTH || winner == NORTH)
			hasWinner = true;
	}//end if
	//if game is not over
	else
		over = false;
}
//If the game isn't over (i.e., more moves are possible), 
//set over to false and do not change anything else. Otherwise, 
//set over to true and hasWinner to true if the game has a winner, 
//or false if it resulted in a tie. If hasWinner is set to false, leave winner unchanged; 
//otherwise, set it to the winning side.

bool Game::move()
{
	bool over = false;
	bool hasWinner = false;
	Side s;

	Side endSide;
	int endHole;
	Player* tempP;
	int c_hole = 0;

	status(over, hasWinner, s);
	//if game is over, return false
	if (over)
		return false;
	//check whose turn it is
	if (m_turn == NORTH)
		tempP = m_northP;
	else
		tempP = m_southP;
	//chosen hole
	c_hole = tempP->chooseMove(m_board, m_turn);
	std::cout << tempP->name() << " choose hole: " << c_hole << std::endl;
	//sow
	m_board.sow(m_turn, c_hole, endSide, endHole);

	//if last bean placed in place a my hole not the pot && empty hole && directly opposite hole is not empty then capture
	if (endHole != 0 && endSide == m_turn && m_board.beans(m_turn, endHole) == 1 && m_board.beans(opponent(m_turn), endHole) > 0)
	{
		//If it was placed in one of the player's own holes that was empty just a moment before, and if the opponent's hole directly opposite from that hole is not empty, capture the beans.
		m_board.moveToPot(m_turn, endHole, m_turn);
		m_board.moveToPot(opponent(m_turn), endHole, m_turn);
		display();
	}
	else
		display();
	//if last bean placed in the pot
	while (endHole == 0 )
	{
		if (m_board.beansInPlay(SOUTH) == 0 || m_board.beansInPlay(NORTH) == 0)
			break;
		//display();
		std::cout << tempP->name() << " gets another turn" << std::endl;
		c_hole = tempP->chooseMove(m_board, m_turn);
		
		std::cout << tempP->name() << " chooses hole " << c_hole << std::endl;
		m_board.sow(m_turn, c_hole, endSide, endHole);
		display();
			
	}//end while


	//when game is end, if south player has no bean
	if (m_board.beansInPlay(SOUTH) == 0)
	{
		//display();

		std::cout << "Sweeping remaining beans into " << m_northP->name() << "'s pot." << std::endl;
		for (int i = 1; i < m_board.holes() + 1; i++)
		{
			m_board.moveToPot(m_turn, i, m_turn);
			m_board.moveToPot(opponent(m_turn), i, opponent(m_turn));
		}//move the remaining beans to the pots when the game is over
		display();
	}
	//if 
	else if (m_board.beansInPlay(NORTH) == 0)
	{
		//display();

		std::cout << "Sweeping remaining beans into " << m_southP->name() << "'s pot." << std::endl;
		for (int i = 1; i < m_board.holes() + 1; i++)
		{
			m_board.moveToPot(m_turn, i, m_turn);
			m_board.moveToPot(opponent(m_turn), i, opponent(m_turn));
		}//move the remaining beans to the pots when the game is over
		display();
	}
	m_turn = opponent(m_turn);
	return true;

}
//If the game is over, return false.Otherwise, 
//make a complete move for the player whose turn it is
//(so that it becomes the other player's turn) and return true. 
//"Complete" means that the player sows the seeds from a hole and 
//takes any additional turns required or completes a capture. 
//If the player gets an additional turn, 
//you should display the board so someone looking at the screen can follow what's happening.

void Game::play()
{
	bool over;
	bool hasWinner;
	Side s;
	Player* tempP;
	status(over, hasWinner, s);

	display();
	while (!over)
	{
		if (!m_southP->isInteractive() && !m_northP->isInteractive())
		{
			std::cout << "Press ENTER to continue ";
			std::cin.ignore(1000,'\n');
		}//end if
		move();
		//check status for every move
		status(over, hasWinner, s);
	}//end while



	if (hasWinner)
	{
		if (s == NORTH)
			tempP = m_northP;
		else
			tempP = m_southP;
		std::cout << "The winner is: " << tempP->name() << std::endl;
	}
	else
		std::cout << "The game is a tie" << std::endl;
}//end play

//Play the game.Display the progress of the game in a manner of your choosing, 
//provided that someone looking at the screen can follow what's happening. 
//If neither player is interactive,
//then to keep the display from quickly scrolling through the whole game, 
//it would be reasonable periodically to prompt the viewer to press ENTER 
//to continue and not proceed until ENTER is pressed. 
//(The ignore function for input streams is useful here.) 
//Announce the winner at the end of the game. 
//You can apportion to your liking the responsibility for displaying the board 
//between this function and the move function. 
//(Note: If when this function is called, South has no beans in play, so can't make the first move, 
//sweep any beans on the North side into North's pot and act as if the game is thus over.)

int Game::beans(Side s, int hole) const
{
	if (hole < 0 || hole > m_board.holes())
		return -1;
	return m_board.beans(s, hole);
}
//Return the number of beans in the indicated hole or pot of the game's board, 
//or −1 if the hole number is invalid. This function exists
//so that we and you can more easily test your program.