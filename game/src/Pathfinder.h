#pragma once
#include "Tilemap.h"
#include <iostream>
#include <unordered_map>
#include <sstream>

class Pathfinder
{
public:
	Tilemap* map = nullptr;
	
private:
	std::unordered_map < TileCoord, float, std::hash<TileCoord>, std::equal_to<TileCoord> > unvisited;
	std::unordered_map < TileCoord, float, std::hash<TileCoord>, std::equal_to<TileCoord> > visited;
	std::unordered_map < TileCoord, TileCoord, std::hash<TileCoord>, std::equal_to<TileCoord> > cheapestEdgeTo;
	TileCoord startNode;
	TileCoord goalNode;
	TileCoord currentNode;
	Vector2 offSet;
public:
	Pathfinder() {};
	Pathfinder(Tilemap* levelToNavigate, TileCoord startTile, TileCoord endTile)
	{
		map = levelToNavigate;
		startNode = startTile;
		goalNode = endTile;
		currentNode = startNode;
		offSet = { map->GetTileWidth() / 2,map->GetTileHeight() / 2 };
		Restart();
	}

	void Restart()
	{
		visited.clear();
		cheapestEdgeTo.clear();
		unvisited.clear();
		for (TileCoord position : map->GetAllTraversableTiles())
		{
			unvisited[position] = INFINITY;
		}
		unvisited[startNode] = 0;

	}

	std::pair < TileCoord, float> GetLowestCostIn(std::unordered_map<TileCoord, float>set)
	{
		TileCoord cheapestPosition = { -1,-1 };
		float lowestCost = INFINITY;
		for (auto nodeValuePair : set)
		{

			TileCoord tile = nodeValuePair.first;
			float cost = nodeValuePair.second;
			if (cost < lowestCost)
			{
				cheapestPosition = tile;
				lowestCost = cost;
			}
		}
		 
		return { cheapestPosition,lowestCost };
	}
			
	bool IsVisited(TileCoord pos) const { return visited.count(pos); }
	bool IsSolved() const { return IsVisited(goalNode); }
	bool IsCompleted() { return IsVisited(goalNode) || GetLowestCostIn(unvisited).second == INFINITY; }

	float GetTotalCostToReach(TileCoord pos) { return unvisited[pos]; }
//	float GetTotalCostedToReach(TileCoord pos) { return visited[pos]; }
	void SetCostToReach(TileCoord pos, float newCost) { unvisited[pos] = newCost; }

	void ProcessNextIterationFunctional()
	{
		if (IsCompleted()) return;
		
		currentNode = GetLowestCostIn(unvisited).first;
		for (auto adjacent : map->GetAdjacentTiles(currentNode))
		{
			if (IsVisited(adjacent)) continue;

			float costThisWay = GetTotalCostToReach(currentNode) + map->GetCostForTile(adjacent);

			float oldCost = GetTotalCostToReach(adjacent);
			//std::cout << "costs " << oldCost << "|" << costThisWay << std::endl;
			if (costThisWay < oldCost)
			{
				SetCostToReach(adjacent, costThisWay);
				cheapestEdgeTo[adjacent] = currentNode;
									
			}
			

		}
	
		MoveToVistedSet(currentNode);
	}

	void MoveToVistedSet(TileCoord node)
	{
		visited[currentNode] = unvisited[currentNode];
		unvisited.erase(currentNode);
	}

	bool SolvePath()
	{
		Restart();
		while (!IsCompleted())
		{
			ProcessNextIterationFunctional();

		}
		return IsSolved();
	}

	std::list<TileCoord> GetSolution()
	{
	
		std::list<TileCoord> solution;
		auto currentNode = goalNode;
		// The lab had this function, but didnt have this if statement
		// I dont know if I forgot something but without this if statment, if the goal cannot be reached, the while statment will always be true
		// So it never ends.
		if (IsSolved()) 
		{
		while (currentNode != startNode)
		{
			 
			solution.push_front(currentNode);
			currentNode = cheapestEdgeTo[currentNode];
		}
		}
 
		return solution;
	}
	void drawCurrent()
	{
		//float coststh = GetTotalCostToReach(currentNode)   ;
		//std::cout << "TILE " << currentNode.x << "|" << currentNode.y << "|"  << std::endl;
		Vector2 offSet = { map->GetTileWidth() / 2,map->GetTileHeight() / 2 };
		for (auto c : cheapestEdgeTo)
		{
			DrawLineEx(map->GetScreenPosOfTile(c.first) + offSet, map->GetScreenPosOfTile(c.second) + offSet, offSet.x / 10, LIME);
			if (!IsVisited(c.first))
			{
				
				drawTile(c.first, 6, DARKGRAY, visited[c.second] + 1);
				
			}
		 

			
		 

		}
		for (auto v : visited)
		{
		drawTile(v.first, 6, GREEN, v.second);
		}

		drawTile(currentNode, 6, BLUE, visited[currentNode]);

	}
	void drawGoal()
	{
		 
		
		if (currentNode == goalNode) DrawText("Complete", map->GetScreenPosOfTile(goalNode).x+ map->GetTileWidth() / 20, map->GetScreenPosOfTile(goalNode).y+ map->GetTileWidth() / 4, map->GetTileWidth() / 5, BLACK);
		 drawTile(goalNode, 6, RED);
	}

	void drawCosts()
	{
		Vector2 offSet = { map->GetTileWidth() / 2,map->GetTileHeight() / 2 };
		for (auto c : cheapestEdgeTo)
		{
			DrawLineEx(map->GetScreenPosOfTile(c.first) + offSet, map->GetScreenPosOfTile(c.second) + offSet, offSet.x / 10, LIME);
			if (!IsVisited(c.first))
			{			
				drawTile(c.first, 6, DARKGRAY, GetTotalCostToReach(c.first));
			}
			 			

			
		}
		for (auto v : visited)
		{
			drawTile(v.first, 6, GREEN, v.second);
			 			
		}
	
	}
	void drawSolution()
	{
		TileCoord prev = startNode;
		for (auto p : GetSolution())
		{	 
			DrawLineEx(map->GetScreenPosOfTile(p) + offSet, map->GetScreenPosOfTile(prev) + offSet, offSet.x / 5, DARKBLUE);
			prev = p;
			
		}
	}
	//Functions that draw the pathfinding debug tiles
	// One draws just a square the other draws the square with the tile cost
	//
	void drawTile(TileCoord t, float line, Color c,float cost)
	{
		Rectangle rect = { map->GetScreenPosOfTile(t).x, map->GetScreenPosOfTile(t).y,map->GetTileWidth(),map->GetTileHeight() } ;
		DrawRectangleLinesEx(rect, line, c);
		// Converting the float into a char string
		std::stringstream s;
		s << cost;
		DrawText(s.str().c_str(), rect.x + (map->GetTileWidth() / 2), rect.y + (rect.width / 2), rect.height / 2, c);
	}
	void drawTile(TileCoord t, float line, Color c)
	{
		Rectangle rect = { map->GetScreenPosOfTile(t).x, map->GetScreenPosOfTile(t).y,map->GetTileWidth(),map->GetTileHeight() };
		DrawRectangleLinesEx(rect, line, c);
	}
	TileCoord getCurrent()
	{
		return currentNode;
	}

};