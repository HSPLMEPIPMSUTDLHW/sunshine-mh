#pragma once
#include "TileCoord.h"
#include "raylib.h"
#include "rlImGui.h"
#include <iostream>
const unsigned int MAP_WIDTH = 28;
const unsigned int MAP_HEIGHT = 20;

/*
const TileCoord NORTH = { -1,0 };
const TileCoord SOUTH = { 1,0 };
const TileCoord EAST = { 0,1 };
const TileCoord WEST = { 0,-1};
*/
const TileCoord WEST = { -1,0 };
const TileCoord EAST = { 1,0 };
const TileCoord SOUTH = { 0,1 };
const TileCoord NORTH = { 0,-1};


enum Tile
{
	Floor = 0,
	Mountain,
	Water,
	Wall,
	Rock,
	Grass,
	Count
		
};

class Tilemap
{
public:
	float tileSizeX = 32;
	float tileSizeY = 32;
	Color tileColors[ (int)Tile::Count ];

	Tile tiles[MAP_WIDTH][MAP_HEIGHT];

	Tilemap()
	{
		tileColors[ Floor ] = BEIGE;
		tileColors[ Grass ] = GREEN;
		tileColors[ Water ] = BLUE;
		tileColors[ Wall ] = DARKGRAY;
		tileColors[Rock] = GRAY;
		tileColors[ Mountain ]  = LIGHTGRAY;
	}

	bool isTileTraversable(TileCoord tilePosition)
	{
		Tile type = tiles[tilePosition.x][tilePosition.y];
		if (type == Tile::Floor && !tileOOB(tilePosition)) return true;
		return false;
		 
	}

	Vector2 GetScreenPosOfTile(TileCoord tilePosition)
	{
		return{ (float)tilePosition.x * tileSizeX, (float)tilePosition.y * tileSizeY };

	}

	void RandomizeTiles()
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{ 
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				tiles[x][y] = (Tile)(rand() % (int)(Tile::Water));
			}
		}
	}

	std::vector<TileCoord> GetAdjacentTiles(TileCoord tileposition)
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
	std::vector<Vector2> GetAdjacentTilesV(TileCoord tileposition)
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

	bool tileOOB(TileCoord tilepos)
	{
		if (tilepos.x < MAP_WIDTH && tilepos.x >= 0 && tilepos.y < MAP_HEIGHT && tilepos.y >= 0)
		{
			return false;
		}
		else return true;
	}
	bool tileOOBV(TileCoord tilepos)
	{
		if (tilepos.x < MAP_WIDTH * tileSizeX && tilepos.x >= 0 && tilepos.y < MAP_HEIGHT * tileSizeY && tilepos.y >= 0)
		{
			return false;
		}
		else return true;
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
				
				color = tileColors[tile];
				DrawRectangleV(position, { (float)tileSizeX, (float)tileSizeY }, color );
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
					 
						if (!tileOOBV(t)&&(!(t.x < GetScreenPosOfTile(tile).x) || !(t.x < GetScreenPosOfTile(tile).y)))
						{
							DrawLineV(t + offSet, GetScreenPosOfTile(tile) + offSet, LIME);
						}
 
				
					}

				}
			}
		}
	}
};