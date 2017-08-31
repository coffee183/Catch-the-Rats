// rats.cpp

// Portions you are to complete are marked with a TODO: comment.
// We've provided some incorrect return statements (so indicated) just
// to allow this skeleton program to compile and run, albeit incorrectly.
// The first thing you probably want to do is implement the utterly trivial
// functions (marked TRIVIAL).  Then get Arena::display going.  That gives
// you more flexibility in the order you tackle the rest of the functionality.
// As you finish implementing each TODO: item, remove its TODO: comment.

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>    //exit()
#include <cctype>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;            // max number of rows in the arena
const int MAXCOLS = 20;            // max number of columns in the arena
const int MAXRATS = 100;           // max number of rats allowed

const int NORTH = 0;
const int EAST = 1;
const int SOUTH = 2;
const int WEST = 3;
const int NUMDIRS = 4;

const int EMPTY = 0;
const int HAS_POISON = 1;

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
			  // type name, since it's mentioned in the Rat declaration.

class Rat
{
public:
	// Constructor
	Rat(Arena* ap, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;
	bool isDead() const;

	// Mutators
	void move();
	
private:
	Arena* m_arena;
	int    m_row;
	int    m_col;
	bool m_dead;
	bool m_poisoned;
	int attemptToMove;
	void poison_Move(int m);
	void setPoisoned();
	void setDead();// TODO: You'll probably find that a rat object needs additional
	// data members to support your implementation of the behavior affected
	// by poison pellets.
};

class Player
{
public:
	// Constructor
	Player(Arena* ap, int r, int c);

	// Accessors
	int  row() const;
	int  col() const;
	bool isDead() const;

	// Mutators
	string dropPoisonPellet();
	string move(int dir);
	void   setDead();

private:
	Arena* m_arena;
	int    m_row;
	int    m_col;
	bool   m_dead;
};

class Arena
{
public:
	// Constructor/destructor
	Arena(int nRows, int nCols);
	~Arena();

	// Accessors
	int     rows() const;
	int     cols() const;
	Player* player() const;
	int     ratCount() const;
	int     getCellStatus(int r, int c) const;
	int     numberOfRatsAt(int r, int c) const;
	void    display(string msg) const;

	// Mutators
	void setCellStatus(int r, int c, int status);
	bool addRat(int r, int c);
	bool addPlayer(int r, int c);
	void moveRats();

private:
	int     m_grid[MAXROWS][MAXCOLS];
	int     m_rows;
	int     m_cols;
	Player* m_player;
	Rat*    m_rats[MAXRATS];
	int     m_nRats;
	int     m_turns;

	// Helper functions
	void checkPos(int r, int c) const;
	bool isPosInBounds(int r, int c) const;
};

class Game
{
public:
	// Constructor/destructor
	Game(int rows, int cols, int nRats);
	~Game();

	// Mutators
	void play();

private:
	Arena* m_arena;

	// Helper functions
	string takePlayerTurn();
};

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int randInt(int min, int max);
bool decodeDirection(char ch, int& dir);
bool attemptMove(const Arena& a, int dir, int& r, int& c);
bool recommendMove(const Arena& a, int r, int c, int& bestDir);
void clearScreen();
int numberOfRatsAround(const Arena& a, int r, int c);
void doBasicTests();
void pelletAroundIt(const Arena& a, int pelletAround[], int r, int c);

///////////////////////////////////////////////////////////////////////////
//  Rat implementation
///////////////////////////////////////////////////////////////////////////

Rat::Rat(Arena* ap, int r, int c)
{
	if (ap == nullptr)
	{
		cout << "***** A rat must be created in some Arena!" << endl;
		exit(1);
	}
	if (r < 1 || r > ap->rows() || c < 1 || c > ap->cols())
	{
		cout << "***** Rat created with invalid coordinates (" << r << ","
			<< c << ")!" << endl;
		exit(1);
	}
	m_arena = ap;
	m_row = r;
	m_col = c;
	m_dead = false;
	m_poisoned = false;
}

int Rat::row() const
{
	return m_row;
}

int Rat::col() const
{
	return m_col;  
}

bool Rat::isDead() const
{
	return m_dead;  
}
void Rat::setPoisoned()
{
	m_poisoned = true;
	attemptToMove = 1;
}
void Rat::setDead()
{
	m_dead = true;
}
void Rat::poison_Move(int m)
{
	attemptToMove = m;
}
void Rat::move()
{
	if (m_poisoned == true && attemptToMove == 1)
	{
		//cerr << "The rat won't move because he is poisoned and he tried to move last time" << endl;
		poison_Move(0);
		return;
	}
	//cerr << "Before Move----Row No.: " << m_row << " " << "Col No.: " << m_col << endl;
	int dir;
	dir = randInt(0, 3);
	//cerr << "The direction is " << dir << endl;
	if ((m_poisoned == true) && (attemptToMove == 0 )&& ((m_row == 1 && dir == 0) || (m_row == m_arena -> rows() && dir == 2)
		|| (m_col == 1 && dir == 3) || (m_col == m_arena -> cols() && dir == 1)))
	{
		poison_Move(1);
		return;
	}

	bool m = attemptMove(*m_arena, dir, m_row, m_col);
	if (!m)
		return;		
	//cerr << "After Move ---- Row No.: " << m_row << " " << "Col No.: " << m_col << endl;
	int count = m_arena->numberOfRatsAt(m_row, m_col);	
	//cerr << count << " rats in that posiiton after move" << endl;
	if (m_arena->getCellStatus(m_row, m_col) == HAS_POISON)
	{
		if (m_poisoned == true)
		{
			setDead();
			//cerr << "One rat died because he ate two pellets" << endl;
		}
		else
			setPoisoned();
		m_arena->setCellStatus(m_row, m_col, EMPTY);
	}
	if (m_poisoned == true && attemptToMove == 0)
	{
		poison_Move(1);
	}
}


///////////////////////////////////////////////////////////////////////////
//  Player implementation
///////////////////////////////////////////////////////////////////////////

Player::Player(Arena* ap, int r, int c)
{
	if (ap == nullptr)
	{
		cout << "***** The player must be created in some Arena!" << endl;
		exit(1);
	}
	if (r < 1 || r > ap->rows() || c < 1 || c > ap->cols())
	{
		cout << "**** Player created with invalid coordinates (" << r
			<< "," << c << ")!" << endl;
		exit(1);
	}
	m_arena = ap;
	m_row = r;
	m_col = c;
	m_dead = false;
}
int Player::row() const
{
	return m_row;  
}

int Player::col() const
{
	return m_col; 
}

string Player::dropPoisonPellet()
{
	if (m_arena->getCellStatus(m_row, m_col) == HAS_POISON)
		return "There's already a poison pellet at this spot.";
	m_arena->setCellStatus(m_row, m_col, HAS_POISON);
	return "A poison pellet has been dropped.";
}

string Player::move(int dir)
{
	if ((m_row == 1 && dir == 0) || (m_row == m_arena -> rows() && dir == 2)
		|| (m_col == 1 && dir == 3) || (m_col == m_arena -> cols() && dir == 1))
		return "Player couldn't move; player stands.";
	
	attemptMove(*m_arena, dir, m_row, m_col);
	
	if (m_arena->numberOfRatsAt(m_row, m_col) > 0)
	{
		setDead();
		return "Player walked into a rat and died.";
	}
	if (dir == 0)
		return "Player moved north.";
	if (dir == 2)
		return "Player moved south.";
	if (dir == 1)
		return "Player moved east.";
	if (dir == 3)
		return "Player moved west.";
		// TODO:  Attempt to move the player one step in the indicated
	//        direction.  If this fails,
	//        return "Player couldn't move; player stands."
	//        A player who moves onto a rat dies, and this
	//        returns "Player walked into a rat and died."
	//        Otherwise, return one of "Player moved north.",
	//        "Player moved east.", "Player moved south.", or
	//        "Player moved west."
	  // This implementation compiles, but is incorrect.
}

bool Player::isDead() const
{
	return m_dead;  
}

void Player::setDead()
{
	m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Arena implementation
///////////////////////////////////////////////////////////////////////////

Arena::Arena(int nRows, int nCols)
{
	if (nRows <= 0 || nCols <= 0 || nRows > MAXROWS || nCols > MAXCOLS)
	{
		cout << "***** Arena created with invalid size " << nRows << " by "
			<< nCols << "!" << endl;
		exit(1);
	}
	m_rows = nRows;
	m_cols = nCols;
	m_player = nullptr;
	m_nRats = 0;
	m_turns = 0;
	for (int r = 1; r <= m_rows; r++)
		for (int c = 1; c <= m_cols; c++)
			setCellStatus(r, c, EMPTY);
}

Arena::~Arena()
{
	cerr << "Entering Arena destructor" << endl;
	delete m_player;
	for (int i = 0; i < m_nRats; i++)
		delete m_rats[i];	
	cerr << "Leaving Arena destructor" << endl;					
}

int Arena::rows() const
{
	return m_rows;  
}

int Arena::cols() const
{
	return m_cols; 
}

Player* Arena::player() const
{
	return m_player;
}

int Arena::ratCount() const
{
	return m_nRats; 
}

int Arena::getCellStatus(int r, int c) const
{
	checkPos(r, c);
	return m_grid[r - 1][c - 1];
}

int Arena::numberOfRatsAt(int r, int c) const
{
	int count = 0;
	//cerr << "There are "<< m_nRats << " rats now."<< endl;
	for (int i = 0; i < m_nRats; i++)
	{
		if ((m_rats[i]->row() == r) && (m_rats[i]->col() == c))
			count++;
	}
	//cerr << count << " rats are in this position." << endl;
	return count;  
}

void Arena::display(string msg) const
{
	char displayGrid[MAXROWS][MAXCOLS];
	int r, c;

	// Fill displayGrid with dots (empty) and stars (poison pellets)
	for (r = 1; r <= rows(); r++)
		for (c = 1; c <= cols(); c++)
			displayGrid[r - 1][c - 1] = (getCellStatus(r, c) == EMPTY ? '.' : '*');

	for (r = 1; r <= rows(); r++)
	{
		for (c = 1; c <= cols(); c++)
		{
			if (numberOfRatsAt(r, c) == 1)
				displayGrid[r - 1][c - 1] = 'R';
			else if (numberOfRatsAt(r, c) >= 2 && numberOfRatsAt(r, c) < 9)
			{
				int m = numberOfRatsAt(r, c);
				displayGrid[r - 1][c - 1] = m + '0';
			}
				
			else if (numberOfRatsAt(r, c) >= 9)
				displayGrid[r - 1][c - 1] = '9';
		}
	}
		// Indicate each rat's position
	// TODO:  If one rat is at some grid point, set the displayGrid char
	//        to 'R'.  If it's 2 though 8, set it to '2' through '8'.
	//        For 9 or more, set it to '9'.

	// Indicate player's position
	if (m_player != nullptr)
		displayGrid[m_player->row() - 1][m_player->col() - 1] = (m_player->isDead() ? 'X' : '@');

	// Draw the grid
	clearScreen();
	for (r = 1; r <= rows(); r++)
	{
		for (c = 1; c <= cols(); c++)
			cout << displayGrid[r - 1][c - 1];
		cout << endl;
	}
	cout << endl;

	// Write message, rat, and player info
	if (msg != "")
		cout << msg << endl;
	cout << "There are " << ratCount() << " rats remaining." << endl;
	if (m_player == nullptr)
		cout << "There is no player!" << endl;
	else if (m_player->isDead())
		cout << "The player is dead." << endl;
	cout << m_turns << " turns have been taken." << endl;
}

void Arena::setCellStatus(int r, int c, int status)
{
	checkPos(r, c);
	m_grid[r - 1][c - 1] = status;
}

bool Arena::addRat(int r, int c)
{
	if (!isPosInBounds(r, c))
		return false;

	// Don't add a rat on a spot with a poison pellet
	if (getCellStatus(r, c) != EMPTY)
		return false;

	// Don't add a rat on a spot with a player
	if (m_player != nullptr  &&  m_player->row() == r  &&  m_player->col() == c)
		return false;

	// If there are MAXRATS existing rats, return false.  Otherwise,
	// dynamically allocate a new rat at coordinates (r,c).  Save the
	// pointer to the newly allocated rat and return true.
	if (m_nRats == MAXRATS)
		return false;
	m_rats[m_nRats] = new Rat(this, r, c);
	m_nRats++;
	//cerr << "A total of " << m_nRats << " rats are added." << endl;
	return true;
}

bool Arena::addPlayer(int r, int c)
{
	if (!isPosInBounds(r, c))
		return false;

	// Don't add a player if one already exists
	if (m_player != nullptr)
		return false;

	// Don't add a player on a spot with a rat
	if (numberOfRatsAt(r, c) > 0)
		return false;

	m_player = new Player(this, r, c);
	return true;
}

void Arena::moveRats()
{
	//cerr << "Currently, there are " << m_nRats << " rats." << endl;
	for (int i = 0; i < m_nRats; i++)
	{
		m_rats[i]->move();
		if ((m_player->row() == m_rats[i]->row()) && (m_player->col() == m_rats[i]->col()))
		{
			//cerr << "bong" << endl;
			m_player->setDead();
		}		
	}
	int j = 0;
	while (j < m_nRats)
	{
		if (m_rats[j]->isDead())
		{
			delete m_rats[j];
			m_rats[j] = m_rats[m_nRats-1];
			m_nRats--;
		}
		else
			j++;
	}
	m_turns++;
}

bool Arena::isPosInBounds(int r, int c) const
{
	return (r >= 1 && r <= m_rows  &&  c >= 1 && c <= m_cols);
}

void Arena::checkPos(int r, int c) const
{
	if (r < 1 || r > m_rows || c < 1 || c > m_cols)
	{
		cout << "***** " << "Invalid arena position (" << r << ","
			<< c << ")" << endl;
		exit(1);
	}
}

///////////////////////////////////////////////////////////////////////////
//  Game implementation
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nRats)
{
	if (nRats < 0)
	{
		cout << "***** Cannot create Game with negative number of rats!" << endl;
		exit(1);
	}
	if (nRats > MAXRATS)
	{
		cout << "***** Trying to create Game with " << nRats
			<< " rats; only " << MAXRATS << " are allowed!" << endl;
		exit(1);
	}
	if (rows == 1 && cols == 1 && nRats > 0)
	{
		cout << "***** Cannot create Game with nowhere to place the rats!" << endl;
		exit(1);
	}

	// Create arena
	m_arena = new Arena(rows, cols);

	// Add player
	int rPlayer;
	int cPlayer;
	do
	{
		rPlayer = randInt(1, rows);
		cPlayer = randInt(1, cols);
	} while (m_arena->getCellStatus(rPlayer, cPlayer) != EMPTY);
	m_arena->addPlayer(rPlayer, cPlayer);

	// Populate with rats
	while (nRats > 0)
	{
		int r = randInt(1, rows);
		int c = randInt(1, cols);
		if (r == rPlayer && c == cPlayer)
			continue;
		m_arena->addRat(r, c);
		nRats--;
	}
}

Game::~Game()
{
	delete m_arena;
}

string Game::takePlayerTurn()
{
	for (;;)
	{
		cout << "Your move (n/e/s/w/x or nothing): ";
		string playerMove;
		getline(cin, playerMove);

		Player* player = m_arena->player();
		int dir;

		if (playerMove.size() == 0)
		{
			if (recommendMove(*m_arena, player->row(), player->col(), dir))
				return player->move(dir);
			else
				return player->dropPoisonPellet();
		}
		else if (playerMove.size() == 1)
		{
			if (tolower(playerMove[0]) == 'x')
				return player->dropPoisonPellet();
			else if (decodeDirection(playerMove[0], dir))
				return player->move(dir);
		}
		cout << "Player move must be nothing, or 1 character n/e/s/w/x." << endl;
	}
}

void Game::play()
{
	m_arena->display("");
	while (!m_arena->player()->isDead() && m_arena->ratCount() > 0) // isDead() is a public member function of Player
	{
		string msg = takePlayerTurn();
		Player* player = m_arena->player();  // player() is a public member function of Arena
		if (player->isDead())
		{
			cout << msg << endl;
			break;
		}
		m_arena->moveRats();
		m_arena->display(msg);
	}
	if (m_arena->player()->isDead())
		cout << "You lose." << endl;
	else
		cout << "You win." << endl;
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementation
///////////////////////////////////////////////////////////////////////////

// Return a uniformly distributed random int from min to max, inclusive
int randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

bool decodeDirection(char ch, int& dir)
{
	switch (tolower(ch))
	{
	default:  return false;
	case 'n': dir = NORTH; break;
	case 'e': dir = EAST;  break;
	case 's': dir = SOUTH; break;
	case 'w': dir = WEST;  break;
	}
	return true;
}

// Return false without changing anything if moving one step from (r,c)
// in the indicated direction would run off the edge of the arena.
// Otherwise, update r and c to the position resulting from the move and
// return true.
bool attemptMove(const Arena& a, int dir, int& r, int& c)
{
	if ((r == 1 && dir == 0) || (r == a.rows() && dir == 2)
		|| (c == 1 && dir == 3) || (c == a.cols() && dir == 1))
		return false;
	if (dir == 0)
		r--;
	if (dir == 2)
		r++;
	if (dir == 1)
		c++;
	if (dir == 3)
		c--;
	return true; 
}
int numberOfRatsAround(const Arena& a, int r, int c)
{
	int count = 0;
	if (r != 1)
		count = count + a.numberOfRatsAt(r - 1, c);
	//cerr << count << endl;
	if (r != a.rows())
		count = count + a.numberOfRatsAt(r + 1, c);
	//cerr << count << endl;
	if (c != 1)
		count = count + a.numberOfRatsAt(r, c - 1);
	//cerr << count << endl;
	if (c != a.cols())
		count = count + a.numberOfRatsAt(r, c + 1);
	//cerr << count << endl;
	return count;
}


// Recommend a move for a player at (r,c):  A false return means the
// recommendation is that the player should drop a poison pellet and not
// move; otherwise, this function sets bestDir to the recommended
// direction to move and returns true.
bool recommendMove(const Arena& a, int r, int c, int& bestDir)
{
	int around[NUMDIRS];
	if (r == 1)
		around[0] = 999;
	else
	{
		around[0] = a.numberOfRatsAt(r - 1, c);
		
	}
	if (c == 1)
		around[3] = 999;
	else
	{
		around[3] = a.numberOfRatsAt(r, c - 1);
		
	}
	if (c == a.cols())
		around[1] = 999;
	else
	{
		around[1] = a.numberOfRatsAt(r, c + 1);
		
	}
	if (r == a.rows())
		around[2] = 999;
	else
	{
		around[2] = a.numberOfRatsAt(r + 1, c);                  //around[4] is an array, around = {N, E, S, W} number of Rats aound the player
		
	}
	int i = 0;
	while (i < NUMDIRS)                                          // if there is at least one rat on each side of the player, the player should not move.
	{
		if (around[i] == 0)
			break;
		else
			i++;
	}
	if (i == NUMDIRS)                                             // there is at least one mouse on each side
		return false;
	else
	{
		int noRats[NUMDIRS];                                         //If at least one side of the player has no rats, create an array of none rats side
		int m = 0;													 // noRats = {1,3} represents only the eastern and western sides have no rats, we are 
		int n = 0;                                                   // going to compare which side has more rats around to decide whether to move or where to move
		while (m < NUMDIRS)
		{
			if (around[m] == 0)
			{
				noRats[n] = m;
				n++;
			}
			m++;
		}
		int amountOfRats[NUMDIRS];                                   // amountOfRats is an array, each element represents the amount of rats around this spot
		for (int p = 0; p < n; p++)                                  // amountOfRats = {13, 15} represents 13 rats around the eastern spot of the player, 15 rats around
		{															 // the western spot of the player
			switch (noRats[p])
			{
			case 0:
				amountOfRats[p] = numberOfRatsAround(a, r - 1, c);
				break;
			case 1:
				amountOfRats[p] = numberOfRatsAround(a, r, c + 1);
				break;
			case 2:
				amountOfRats[p] = numberOfRatsAround(a, r + 1, c);
				break;
			case 3:
				amountOfRats[p] = numberOfRatsAround(a, r, c - 1);
				break;
			}
		}
		int ratsAround = numberOfRatsAround(a, r, c);
		int min = amountOfRats[0];
		int direction = noRats[0];
		for (int i = 0; i < n; i++)
		{
			if (min > amountOfRats[i])
			{
				min = amountOfRats[i];
				//direction = noRats[i];
			}

		}
		if (ratsAround < min)
			return false;
		else if (ratsAround == min && a.getCellStatus(r, c) == EMPTY)              // move is not saver than stay, just stay and drop a pellet
			return false;
		int pelletAround[NUMDIRS];
		pelletAroundIt(a, pelletAround, r, c);
		int j = 0;
		int minSide[NUMDIRS];
		for (int i = 0; i < n; i++)
		{
			if (amountOfRats[i] == min)
			{
				minSide[j] = noRats[i];
				cerr << minSide[j] << endl;
				j++;
			}
		}
		int i = 0;
		while (i < j)
		{
			if (pelletAround[minSide[i]] == 0)
			{
				direction = minSide[i];
				break;
			}
			i++;
			
		}
		if (i == j)
			direction = minSide[0];
		bestDir = direction;
		return true;
	}
				   // Your replacement implementation should do something intelligent.
				   // You don't have to be any smarter than the following, although
				   // you can if you want to be:  If staying put runs the risk of a
				   // rat possibly moving onto the player's location when the rats
				   // move, yet moving in a particular direction puts the player in a
				   // position that is safe when the rats move, then the chosen
				   // action is to move to a safer location.  Similarly, if staying put
				   // is safe, but moving in certain directions puts the player in danger,
				   // then the chosen action should not be to move in one of the
				   // dangerous directions; instead, the player should stay put or move to
				   // another safe position.  In general, a position that may be moved to
				   // by many rats is more dangerous than one that may be moved to by
				   // few.
				   //
				   // Unless you want to, you do not have to take into account that a
				   // rat might be poisoned and thus sometimes less dangerous than one
				   // that is not.  That requires a more sophisticated analysis that
				   // we're not asking you to do.
}
void pelletAroundIt(const Arena& a, int pelletAround[], int r, int c)
{
	if (r == 1)
		pelletAround[0] = 999;
	else
		pelletAround[0] = a.getCellStatus(r - 1, c);
	if (c == a.cols())
		pelletAround[1] = 999;
	else
		pelletAround[1] = a.getCellStatus(r, c + 1);
	if (r == a.rows())
		pelletAround[2] = 999;
	else
		pelletAround[2] = a.getCellStatus(r + 1, c);
	if (c == 1)
		pelletAround[3] = 999;
	else
		pelletAround[3] = a.getCellStatus(r, c - 1);
}

///////////////////////////////////////////////////////////////////////////
// main()
///////////////////////////////////////////////////////////////////////////

int main()
{
	
	// Use this instead to create a mini-game:   Game g(3, 5, 2);
	Game g(10, 15, 20);

	// Play the game
	g.play();
}

///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANY CODE BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++ UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _MSC_VER  //  Microsoft Visual C++

#pragma warning(disable : 4005)
#include <windows.h>

void clearScreen()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	COORD upperLeft = { 0, 0 };
	DWORD dwCharsWritten;
	FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
		&dwCharsWritten);
	SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
	static const char* term = getenv("TERM");
	if (term == nullptr || strcmp(term, "dumb") == 0)
		cout << endl;
	else
	{
		static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
		cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
	}
}

#endif
#include <type_traits>
#include <cassert>

#define CHECKTYPE(c, f, r, a)  \
	static_assert(std::is_same<decltype(&c::f), r (c::*)a>::value, \
	   "FAILED: You changed the type of " #c "::" #f);  \
	[[gnu::unused]] r (c::* xxx##c##_##f) a = &c::f

void thisFunctionWillNeverBeCalled()
{
	// If the student deleted or changed the interfaces to the public
	// functions, this won't compile.  (This uses magic beyond the scope
	// of CS 31.)

	Rat r(static_cast<Arena*>(0), 1, 1);
	CHECKTYPE(Rat, row, int, () const);
	CHECKTYPE(Rat, col, int, () const);
	CHECKTYPE(Rat, isDead, bool, () const);
	CHECKTYPE(Rat, move, void, ());

	Player p(static_cast<Arena*>(0), 1, 1);
	CHECKTYPE(Player, row, int, () const);
	CHECKTYPE(Player, col, int, () const);
	CHECKTYPE(Player, isDead, bool, () const);
	CHECKTYPE(Player, dropPoisonPellet, string, ());
	CHECKTYPE(Player, move, string, (int));
	CHECKTYPE(Player, setDead, void, ());

	Arena a(1, 1);
	CHECKTYPE(Arena, rows, int, () const);
	CHECKTYPE(Arena, cols, int, () const);
	CHECKTYPE(Arena, player, Player*, () const);
	CHECKTYPE(Arena, ratCount, int, () const);
	CHECKTYPE(Arena, getCellStatus, int, (int, int) const);
	CHECKTYPE(Arena, numberOfRatsAt, int, (int, int) const);
	CHECKTYPE(Arena, display, void, (string) const);
	CHECKTYPE(Arena, setCellStatus, void, (int, int, int));
	CHECKTYPE(Arena, addRat, bool, (int, int));
	CHECKTYPE(Arena, addPlayer, bool, (int, int));
	CHECKTYPE(Arena, moveRats, void, ());

	Game g(1, 1, 1);
	CHECKTYPE(Game, play, void, ());
}

void findTheRat(const Arena& a, int& r, int& c)
{
	if (a.numberOfRatsAt(r - 1, c) == 1) r--;
	else if (a.numberOfRatsAt(r + 1, c) == 1) r++;
	else if (a.numberOfRatsAt(r, c - 1) == 1) c--;
	else if (a.numberOfRatsAt(r, c + 1) == 1) c++;
	else assert(false);
}

void doBasicTests()
{
	/*{
		Arena a(10, 20);
		a.addPlayer(9, 19);
		Player* pp = a.player();
		assert(pp->row() == 9 && pp->col() == 19 && !pp->isDead());
		assert(pp->move(EAST) == "Player moved east.");
		assert(pp->row() == 9 && pp->col() == 20 && !pp->isDead());
		assert(pp->move(EAST) == "Player couldn't move; player stands.");
		assert(pp->row() == 9 && pp->col() == 20 && !pp->isDead());
		pp->setDead();
		assert(pp->row() == 9 && pp->col() == 20 && pp->isDead());
	}*/
	{
		Arena a(10, 20);
		int r = 1;
		int c = 14;
		//a.setCellStatus(r - 1, c, HAS_POISON);
		//a.setCellStatus(r + 1, c, HAS_POISON);
		a.setCellStatus(r, c - 1, HAS_POISON);
		a.setCellStatus(r, c, HAS_POISON);
		//a.setCellStatus(r, c + 1, HAS_POISON);
		a.addRat(r+1, c);
		a.addRat(r, c + 2);
		a.addPlayer(r, c);
		
		/*for (int k = 0; k < 25; k++)
		{
			a.addRat(3, 9);
				
		}
		for (int k = 0; k < 3; k++)
			a.addRat(4, 10);
		for (int k = 0; k < 19; k++)
			a.addRat(4, 8);
		for (int k = 0; k < 22; k++)
			a.addRat(6, 10);*/
		//for (int m = 0; m < 20; m++)
		//	a.addRat(9, 20);
		//for (int k = 0; k < 20; k++)
		//{
		//	a.addRat(10, 18);
		//	//a.addRat(8, 17);
		//	//a.addRat(8, 20);
		//}
		int bestdir = 2;
		cerr << recommendMove(a, r, c, bestdir) << endl;
		cerr << bestdir << " is the best direction." << endl;
		/*assert(!a.player()->isDead());
		a.moveRats();
		assert(a.player()->isDead());*/
	}
	cout << "Passed all basic tests" << endl;
}