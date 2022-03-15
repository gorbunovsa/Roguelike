#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Player
{
	explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};

	bool Moved() const;
	int ProcessInput(MovementDir dir);
	void Draw(Image &screen);
	void ExtractBackground(Image &screen);
	void FillMap(int x, int y, int v){ map[x][y] = v;}
	void Init(int x, int y){coords.x = x; coords.y = y; old_coords.x = x; old_coords.y = y;}
	int CheckTile(int x, int y);
	int CheckNextStep(int x, int y);
	void BreakWall(Image &screen, int x, int y);
	void ProcessAction(Image &screen);

private:
	Point coords {.x = 10, .y = 10};
	Point old_coords {.x = 10, .y = 10};
	Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
	Pixel bg[tileSize][tileSize];
	int map[lvlSize][lvlSize];
	int move_speed = 2;
};

#endif //MAIN_PLAYER_H
