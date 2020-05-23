#include "World.h"
#include "Game.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>

World::World(sf::RenderWindow& w, Game* g_) : window(w)
{
	srand(time(0));
	window.setFramerateLimit(60);
	Setup();
}

World::~World()
{
}

void World::Setup()
{
	DrawHoverOutline();
	
	
	DrawGameLevel();
	{
		for (int i = 0; i < numTiles; i++)
		{
			for (int j = 0; j < numTiles; j++)
			{
				boardTilesArr[i][j].x = i;
				boardTilesArr[i][j].y = j;

				boardTilesArr[i][j].buildLevel = 0;
			}
			
		}
	}
	
	numPlayers = 2;
	
	//PlaceWorker();
	//if (areMaxWorkersPlaced)
	{
		currentPState = PlayerStates::PlaceWorker;
		//GameStates runningState = GameStates::PlayState;
	}
	
}


//Drawing the game level. 
void World::DrawGameLevel()
{
	window.clear();

	for (int x = 0; x < numTiles; x++)
	{
		for (int y = 0; y < numTiles; y++)
		{
			boardTilesArr[x][y].DrawTile(window);
		}
	}

	for (int w = 0; w < workers.size(); w++)
	{
		workers[w].Draw(window);
	}
	
}

void World::Update()
{
	DrawGameLevel();
	DrawHoverOutline();

	//Player turn
	playerTurnID = playerTurn % numPlayers; 


	//Possible Player states.
	switch (currentPState)
	{
	case PlayerStates::PlaceWorker:
		PlaceWorker();
		break;
	
	case PlayerStates::SelectWorkerState:
		SelectWorker(playerTurnID);
		break;

	case PlayerStates::MoveWorkerState:
		MoveWorker(playerTurnID);
		break;

	case PlayerStates::BuildState:
		Build();
		break;

	default:
		break;
	}
}





Tile* World::Hover()
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

	int A = 0;
	int B = 0;

	for (int a = 0; a < numTiles; a++)
	{
		for (int b = 0; b < numTiles; b++)
		{
			boardTilesArr[a][b].isOutlined = false;

			if (mousePosition.x >= boardTilesArr[a][b].x * 120 + 5)
			{
				if (a < 4)
				{
					if (mousePosition.x <= boardTilesArr[a + 1][b].x * 120 + 5)
					{
						A = a;
					}
				}
				else
				{
					A = a;
				}
			}

			if (mousePosition.y >= boardTilesArr[a][b].y * 120 + 5)
			{
				if (b < 4)
				{
					if (mousePosition.y <= boardTilesArr[a][b + 1].y * 120 + 5)
					{
						B = b;
					}
				}
				else
				{
					B = b;
				}
			}
		}
	}

	return &(boardTilesArr[A][B]);
}

void World::DrawHoverOutline()
{
	Tile* hoveredTile = Hover();
	hoveredTile->isOutlined = true;
}


void World::PlaceWorker()
{
	/*bool areMaxWorkersPlaced = false;*/

	playerTurnID = playerTurn % numPlayers;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isMouseClicked = false;
	}

	if ((!isMouseClicked) && (!sf::Mouse::isButtonPressed(sf::Mouse::Left)))
	{
		isMouseClicked = true;
		std::cout << "worker placed" << std::endl;
			hoveredTile = Hover();
			if (!isOccupied())
			{
				workers.push_back(Worker(hoveredTile->x, hoveredTile->y, playerTurnID));
				
				playerTurn++;
					
				isValidMove = true;
			}
			else
			{
				isValidMove = false;
			}
			
			if (workers.size() == (MAX_NUM_WORKERS))
			{
				areMaxWorkersPlaced = true;
			}
			
			if (areMaxWorkersPlaced)
			{
				std::cout << "Maximum Workers Placed" << std::endl;
				//GameStates runningState = GameStates::PlayState;
				currentPState = PlayerStates::SelectWorkerState;
			}
	}
	
}


void World::SelectWorker(int& player)
{
	bool hasChosenWorker = false;

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isMouseClicked = false;
	}

	if ((!isMouseClicked) && (sf::Mouse::isButtonPressed(sf::Mouse::Left)))
	{
		isMouseClicked = true;

		
		
		hoveredTile = Hover();

		for (int w = 0; w < workers.size(); w++)
		{
			if ((workers[w].wx == hoveredTile->x) && (workers[w].wy == hoveredTile->y))
			{
				if (player == workers[w].playerID) 
				{
					std::cout << "worker selected" << std::endl;
					isValidMove = true;
					hasChosenWorker = true;
					chosenWorker = &workers[w];
					break;
				}
				else
				{
					std::cout << "Not your builder" << std::endl;
					isValidMove = false;
				}
			}
		}

		if (hasChosenWorker)
		{
			currentPState = PlayerStates::MoveWorkerState;
		}
	}
}

void World::MoveWorker(int& player)
{
	bool playerMoved = false;

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isMouseClicked = false;
	}

	if ((!isMouseClicked) && (sf::Mouse::isButtonPressed(sf::Mouse::Left)))
	{
		isMouseClicked = true;

		std::cout << "MOVE" << std::endl;
		std::cout << playerTurnID << std::endl;

		hoveredTile = Hover();

		chosenWorker->MoveWorker(hoveredTile->x, hoveredTile->y);
		playerMoved = true;
	}

	if (playerMoved)
	{
		currentPState = PlayerStates::BuildState;
	}
}

void World::Build()
{
	bool hasBuiltOnTile = false;

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		isMouseClicked = false;
	}

	if ((!isMouseClicked) && (sf::Mouse::isButtonPressed(sf::Mouse::Left)))
	{
		isMouseClicked = true;

		std::cout << "BUILD" << std::endl;
		std::cout << playerTurnID << std::endl;

		hoveredTile = Hover();
	}

	if (hasBuiltOnTile)
	{
		playerTurn++;
		currentPState = PlayerStates::SelectWorkerState;
	}
}

void World::WStartGame()
{
	
	{
		DrawHoverOutline();


		DrawGameLevel();
		{
			for (int i = 0; i < numTiles; i++)
			{
				for (int j = 0; j < numTiles; j++)
				{
					boardTilesArr[i][j].x = i;
					boardTilesArr[i][j].y = j;

					boardTilesArr[i][j].buildLevel = 0;
				}

			}
		}

		numPlayers = 2;

		PlaceWorker();
		if (areMaxWorkersPlaced)
		{
			currentPState = SelectWorkerState;
			
		}

	}
}



bool World::isInRange()
{
	for (int xTiles = -1; xTiles < 2; xTiles++)
	{
		if (hoveredTile-> x == chosenWorker-> wx + xTiles)
		{
			for (int yTiles = -1; yTiles < 1; yTiles++)
			{
				if (hoveredTile->y == chosenWorker->wy + yTiles)
				{
					if (isOccupied())
					{
						return false;
					}
					return true;
				}
			}
		}
	}
}

bool World::isOccupied()
{
	for (int w = 0; w < workers.size(); w++)
	{
		if ((hoveredTile->x == workers[w].wx) && (hoveredTile->y == workers[w].wy))
		{
			return true;
		}
	}
	return false;
}
