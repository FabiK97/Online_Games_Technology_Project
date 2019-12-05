#include "Dot.h"

Dot::Dot(bool chaser)
{
	isChaser = chaser;
	//Initialize the offsets
	mPosX = 0;
	mPosY = 0;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;


	width = 10;
	height = 10;


}

Dot::~Dot()
{
	gBlackDotTexture.free();
	gRedDotTexture.free();
}

std::int32_t Dot::getColorInt32()
{
	return (std::int32_t)((color.r << 16) + (color.g << 8) + (color.b << 0));
}

void Dot::SetPosition(int x, int y)
{
	mPosX = x;
	mPosY = y;

	mCenterX = mPosX + (width / 2);
	mCenterY = mPosY + (height / 2);
}

//"dot.bmp"
void Dot::Init(SDL_Renderer *gRenderer)
{
	
	if (!gRedDotTexture.loadFromFile("ASSETS/chaserdot.bmp", gRenderer))
	{
		printf("Failed to load dot texture!\n");
	}
	
	if (!gBlackDotTexture.loadFromFile("ASSETS/dot.bmp", gRenderer))
	{
		printf("Failed to load dot texture!\n");
	}

}

void Dot::handleEvent(SDL_Event& e)
{
	if (isChaser)
	{
		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_w: mVelY -= DOT_VEL; break;
			case SDLK_s: mVelY += DOT_VEL; break;
			case SDLK_a: mVelX -= DOT_VEL; break;
			case SDLK_d: mVelX += DOT_VEL; break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_w: mVelY += DOT_VEL; break;
			case SDLK_s: mVelY -= DOT_VEL; break;
			case SDLK_a: mVelX += DOT_VEL; break;
			case SDLK_d: mVelX -= DOT_VEL; break;
			}
		}
	}
	else if (!isChaser)
	{
		//If a key was pressed
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: mVelY -= DOT_VEL; break;
			case SDLK_DOWN: mVelY += DOT_VEL; break;
			case SDLK_LEFT: mVelX -= DOT_VEL; break;
			case SDLK_RIGHT: mVelX += DOT_VEL; break;
			}
		}
		//If a key was released
		else if (e.type == SDL_KEYUP && e.key.repeat == 0)
		{
			//Adjust the velocity
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: mVelY += DOT_VEL; break;
			case SDLK_DOWN: mVelY -= DOT_VEL; break;
			case SDLK_LEFT: mVelX += DOT_VEL; break;
			case SDLK_RIGHT: mVelX -= DOT_VEL; break;
			}
		}
	}
}

void Dot::move(int SCREEN_HEIGHT, int SCREEN_WIDTH)
{
	//Move the dot left or right
	mPosX += mVelX;

	//If the dot went too far to the left or right
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
	{
		//Move back
		mPosX -= mVelX;
	}

	//Move the dot up or down
	mPosY += mVelY;

	//If the dot went too far up or down
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
	{
		//Move back
		mPosY -= mVelY;
	}

	mCenterX = mPosX + (width / 2);
	mCenterY = mPosY + (height / 2);
}

void Dot::render(SDL_Renderer *gRenderer)
{
	//Show the dot
	if (isChaser) {
		gRedDotTexture.render(mPosX, mPosY, gRenderer);
	}
	else {
		gBlackDotTexture.render(mPosX, mPosY, gRenderer);
	}
}

std::string Dot::GetPosAsString()
{
	return std::string("x:"+ std::to_string(mPosX) + "," + "y:" + std::to_string(mPosY));
}

std::string Dot::GetDotAsString()
{
	if (isChaser) {
		return "id:" + std::to_string(id) + "," + GetPosAsString() + "," + "chaser:1";
	}
	else {
		return "id:" + std::to_string(id) + "," + GetPosAsString() + "," + "chaser:0";
	}
}

int Dot::GetCenterX()
{
	return mCenterX;
}

int Dot::GetCenterY()
{
	return mCenterY;
}

bool Dot::Checkcollision(int centerX, int centerY)
{
	int distance = sqrt(((mCenterX - centerX) * (mCenterX - centerX)) + ((mCenterY - centerY) * (mCenterY - centerY)));
	
	if (distance <= width)
	{
		return true;
	}
	else
	{
		return false;
	}
}