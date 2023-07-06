#pragma once
#include <vector>
#include "raylib.h"
#include "TileCoord.h"
#include <iostream>
#include <fstream>
#define MAP_WIDTH 15
#define MAP_HEIGHT 10


enum class Tile
{
	Floor = 0, // Floor tiles can be walked on
	Wall, // Wall tiles cannot be walked on
	Count // number of Tile types (i.e. 2)
};

class Tilemap
{
private:
	float tileSizeX = 64;
	float tileSizeY = 64;
	Tile tiles[MAP_WIDTH][MAP_HEIGHT];
public:
	const TileCoord NORTH = { 0, -1 };
	const TileCoord SOUTH = { 0,  1 };
	const TileCoord EAST = { 1,  0 };
	const TileCoord WEST = { -1,  0 };

	bool isTileTraversable(TileCoord tilePosition)
	{
		Tile type = tiles[tilePosition.x][tilePosition.y];
		if (type == Tile::Floor && tileInBounds(tilePosition)) return true;
		return false;

	}
	size_t GetMapWidth() // get number of columns in the grid
	{
		return MAP_WIDTH;
	}

	size_t GetMapHeight()  // get number of rows in the grid
	{
		return MAP_HEIGHT;
	}

	float GetTileWidth() // get number of columns in the grid
	{
		return tileSizeX;
	}

	float GetTileHeight()  // get number of rows in the grid
	{
		return tileSizeY;
	}
	Tile GetTile(TileCoord tilePos)
	{
		return tiles[tilePos.x][tilePos.y];
	}

	void SetTile(TileCoord tilePos, Tile value)
	{
		if (tileInBounds(tilePos))
			tiles[tilePos.x][tilePos.y] = value;
	}

	Vector2 GetScreenPosOfTile(TileCoord tilePosition)
	{
		return{ (float)tilePosition.x * tileSizeX, (float)tilePosition.y * tileSizeY };

	}
	Vector2 GetScreenPosOfTileV(Vector2 tilePosition)
	{
		return{ (float)tilePosition.x * tileSizeX, (float)tilePosition.y * tileSizeY };

	}
	TileCoord GetTileAtScreenPos(Vector2 tilePosition)
	{
		return{ tilePosition.x / tileSizeX, tilePosition.y / tileSizeY };
	}

	int GetCostForTile(TileCoord tilePositon)  // having this function makes it easier to change costs per tile the future
	{
		return 1;
	}
	//std::vector<TileCoord> GetAllTiles(); // return all tile positions

	std::vector<TileCoord> GetAllTraversableTiles()
	{
		std::vector<TileCoord> traversable;
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				if (isTileTraversable({ x,y }))
				{
					traversable.push_back({ x,y });
				}
			}
		}
		return traversable;
	}

	void clearTiles(Tile type)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				tiles[x][y] = type;
			}
		}
	}


	void RandomizeTiles()
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				
				std::cout << "TILESPASS " << GetAdjacentTiles({ x,y }).size() << std::endl;
				if (GetAdjacentTiles({ x,y }).size() < 1)
				{
					tiles[x][y] = Tile::Floor;
				}
				else if (GetAdjacentTiles({ x,y }).size() > 3)
				{
					tiles[x][y] = Tile::Wall;
				}
				else tiles[x][y] = (Tile)(rand() % (int)(Tile::Count));
				tiles[x][y] = (Tile)(rand() % (int)(Tile::Count));
			}
		}
	}

	void loadTiles()
	{
		std::ifstream file;
		file.open("TileData.txt");
		TileCoord c;
		while (file)
		{
			file >> c.x >> c.y;
			std::cout << c.x << " " << c.y << std::endl;
			SetTile(c, Tile::Floor);
		}
		//file >> c;
		std::cout << c.x << " " << c.y << std::endl;

	}

	std::vector<TileCoord> GetAdjacentTiles(TileCoord tileposition)//returns the adjacent tiles as a tilcoord
	{
		std::vector<TileCoord> adjacentTiles;
		TileCoord N = tileposition + NORTH;
		TileCoord S = tileposition + SOUTH;
		TileCoord E = tileposition + EAST;
		TileCoord W = tileposition + WEST;
		if (isTileTraversable(N)) adjacentTiles.push_back(N);
		if (isTileTraversable(S)) adjacentTiles.push_back(S);
		if (isTileTraversable(E)) adjacentTiles.push_back(E);
		if (isTileTraversable(W)) adjacentTiles.push_back(W);
		return adjacentTiles;
	}
	std::vector<TileCoord> GetAdjacentWalls(TileCoord tileposition)//returns the adjacent tiles as a tilcoord
	{
		std::vector<TileCoord> adjacentTiles;
		TileCoord N = tileposition + NORTH;
		TileCoord S = tileposition + SOUTH;
		TileCoord E = tileposition + EAST;
		TileCoord W = tileposition + WEST;
		if (!(isTileTraversable(N)) && tileInBounds(N)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + NORTH));
		if (!(isTileTraversable(S)) && tileInBounds(S)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + SOUTH));
		if (!(isTileTraversable(E)) && tileInBounds(E)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + EAST));
		if (!(isTileTraversable(W)) && tileInBounds(W)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + WEST));
		return adjacentTiles;
	}
	std::vector<Vector2> GetAdjacentTilesV(TileCoord tileposition)//returns the adjacent tiles as a vector2
	{
		std::vector<Vector2> adjacentTiles;
		TileCoord N = tileposition + NORTH;
		TileCoord S = tileposition + SOUTH;
		TileCoord E = tileposition + EAST;
		TileCoord W = tileposition + WEST;
		if (isTileTraversable(N)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + NORTH));
		if (isTileTraversable(S)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + SOUTH));
		if (isTileTraversable(E)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + EAST));
		if (isTileTraversable(W)) adjacentTiles.push_back(GetScreenPosOfTile(tileposition + WEST));
		return adjacentTiles;
	}

	bool tileInBounds(TileCoord tilepos)//checks if a tile is out of the map bounds
	{
		if (tilepos.x < MAP_WIDTH && tilepos.x >= 0 && tilepos.y < MAP_HEIGHT && tilepos.y >= 0)
		{
			return true;
		}
		else return false;
	}
	bool tileInBoundsV(TileCoord tilepos)//checks if the tile is out of the map bounnds but as a vector. I forgot what I used this for but its unused now.
	{
		if (tilepos.x < MAP_WIDTH * tileSizeX && tilepos.x >= 0 && tilepos.y < MAP_HEIGHT * tileSizeY && tilepos.y >= 0)
		{
			return true;
		}
		else return false;
	}
	void Draw()
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				Color color{ 0,0,0,0 };
				Tile tile = tiles[x][y];
				Vector2 position = GetScreenPosOfTile({ x,y });

				if (!isTileTraversable({ x,y }))
				{
					color = DARKGRAY;
					DrawRectangleV({ position.x, position.y }, { tileSizeX, tileSizeY }, color);
				}
				DrawRectangleLinesEx({ position.x, position.y, tileSizeX,tileSizeY }, 1, BLACK);
			}
		}
	}
	void DrawPaths()
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				TileCoord tile = { x, y };
				if (isTileTraversable(tile))
				{
					std::vector<Vector2> adjacent = (GetAdjacentTilesV(tile));
					Vector2 offSet = { tileSizeX / 2,tileSizeY / 2 };
					DrawCircleV(GetScreenPosOfTile(tile) + offSet, { tileSizeX / 4 }, LIME);

					for (auto t : adjacent)
					{

						if (tileInBoundsV(t) && (!(t.x < GetScreenPosOfTile(tile).x) || !(t.x < GetScreenPosOfTile(tile).y)))
						{
							DrawLineV(t + offSet, GetScreenPosOfTile(tile) + offSet, LIME);
						}


					}

				}
			}
		}
	}

};