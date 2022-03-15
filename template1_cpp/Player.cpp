#include "Player.h"

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

int max(int a, int b)
{
	if(a >= b)
		return a;
	else
		return b;
}

int Player::CheckTile(int x, int y)
{
	switch(map[x][y])
	{
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		return 2;
	case 3: case 4:
		return 3;
	}
	return 10;
}

int Player::CheckNextStep(int x, int y)
{
	int ld = CheckTile(x / tileSize, y / tileSize);
	int lu = CheckTile(x / tileSize, y / tileSize + 1);
	int rd = CheckTile(x / tileSize + 1, y / tileSize);
	int ru = CheckTile(x / tileSize + 1, y / tileSize + 1);
	if(x % tileSize == 0){
		if(y % tileSize == 0){
			return ld;
		}else{
			return max(ld, lu);
		}
	}else{
		if(y % tileSize == 0){
			return max(ld, rd);
		}else{
			return max(max(ld, lu),max(rd, ru));
		}
	}
}

int Player::ProcessInput(MovementDir dir)
{
	int move_dist = move_speed * 1;
	switch(dir)
	{
	case MovementDir::UP:
		switch(CheckNextStep(coords.x, coords.y + move_dist))
		{
		case 0:
			old_coords.y = coords.y;
			coords.y += move_dist;
			break;
		case 1:
			return 1;
		case 2:
			return 2;
		}
		break;
	case MovementDir::DOWN:
		switch(CheckNextStep(coords.x, coords.y - move_dist))
		{
		case 0:
			old_coords.y = coords.y;
			coords.y -= move_dist;
			break;
		case 1:
			return 1;
		case 2:
			return 2;
		}
		break;
	case MovementDir::LEFT:
		switch(CheckNextStep(coords.x - move_dist, coords.y))
		{
		case 0:
			old_coords.x = coords.x;
			coords.x -= move_dist;
			break;
		case 1:
			return 1;
		case 2:
			return 2;
		}
		break;
	case MovementDir::RIGHT:
		switch(CheckNextStep(coords.x + move_dist, coords.y))
		{
		case 0:
			old_coords.x = coords.x;
			coords.x += move_dist;
			break;
		case 1:
			return 1;
		case 2:
			return 2;
		}
		break;
	default:
		break;
	}
	return 0;
}

void Player::BreakWall(Image &screen, int x, int y)
{
	Image floor("./resources/floor.png");
	floor.Draw(screen, x*tileSize, y*tileSize);
	map[x][y] = 0;
}

void Player::ProcessAction(Image &screen)
{
	Point corrTile;
	if(coords.x % tileSize >= tileSize/2)
		corrTile.x = coords.x / tileSize +1;
	else
		corrTile.x = coords.x / tileSize;
	if(coords.y % tileSize >= tileSize/2)
		corrTile.y = coords.y / tileSize +1;
	else
		corrTile.y = coords.y / tileSize;
	if(coords.x % tileSize == 0)
	{
		if(map[coords.x / tileSize - 1][corrTile.y] == 4)
		{
			BreakWall(screen, coords.x / tileSize - 1, corrTile.y);
			return;
		}
		if(map[coords.x / tileSize + 1][corrTile.y] == 4)
		{
			BreakWall(screen, coords.x / tileSize + 1, corrTile.y);
			return;
		}
	}
	if(coords.y % tileSize == 0)
	{
		if(map[corrTile.x][coords.y / tileSize - 1] == 4)
		{
			BreakWall(screen, corrTile.x, coords.y / tileSize - 1);
			return;
		}
		if(map[corrTile.x][coords.y / tileSize + 1] == 4)
		{
			BreakWall(screen, corrTile.x, coords.y / tileSize + 1);
			return;
		}
	}
}


void Player::ExtractBackground(Image &screen)
{
	for(int y = coords.y; y < coords.y + tileSize; y++)
	{
		for(int x = coords.x; x < coords.x + tileSize; x++)
		{
			bg[x - coords.x][y - coords.y] = screen.GetPixel(x,y);
		}
	}
}

void Player::Draw(Image &screen)
{
	Image fox("./resources/fox.png");
	for(int y = old_coords.y; y < old_coords.y + tileSize; y++)
	{
		for(int x = old_coords.x; x < old_coords.x + tileSize; x++)
		{
			screen.PutPixel(x, y, bg[x - old_coords.x][y - old_coords.y]);
		}
	}
	old_coords = coords;
	ExtractBackground(screen);

	for(int y = coords.y; y < coords.y + tileSize; y++)
	{
		for(int x = coords.x; x < coords.x + tileSize; x++)
		{
			Pixel pix;
			pix = fox.GetPixel(x - coords.x, tileSize - (y - coords.y));
			if(pix.a != 0)
				screen.PutPixel(x, y, pix);
		}
	}
}
