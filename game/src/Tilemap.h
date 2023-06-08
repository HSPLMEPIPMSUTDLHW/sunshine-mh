#pragma once
#include "TileCoord.h"
#include "raylib.h"
#include "rlImGui.h"
const unsigned int MAP_WIDTH = 24;
const unsigned int MAP_HEIGHT = 32;



enum Tile
{
	Floor = 0,
	Grass,
	Water,
	Wall,
	Rock,
	Mountain,
	Count
		
};

class Tilemap
{
public:
	float tileSizeX = 32;
	float tileSizeY = 32;

	Tile tiles[MAP_WIDTH][MAP_HEIGHT];
	Color tileColors{ Tile::Count };
	Tilemap()
	{
		tileColors[ Floor ] = GRAY;
		tileColors[ Grass ] = GREEN;
		tileColors[ Water ] = BLUE;
		tileColors[ Wall ] = DARKGRAY;
		tileColors[ Mountain ]  = LIGHTGRAY;



	}

	bool isTileTraversable(TileCoord tilePosition)
	{
		Tile type = tile[tilePosition.x][tilePosition.y];
		 
	}

	Vector2 ScreenPosOfTile(TileCoord tilePosition)
	{
		return{ (float)tilePosition.x * tileSizeX, (float)tilePosition.y * tileSizeY };
	}
	/*
	void RandomizeTiles()
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				float roll = Random(0.0f, 1.0f);
			}
		}
	}
	*/

	void DrawTiles()
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				Vector2 position = ScreenPosOfTile({ x,y });
				Tile tile = tiles[x][y];
				Color color{ 0,0,0,0};
				DrawRectangleV(position, { (float)tileSizeX, (float)tileSizeY }, color );
			}
		}
	}
};