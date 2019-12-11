/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "LTexture.h";
#include "Dot.h";
#include "Client.h";


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

int main(int argc, char* args[]);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

Client *myClient;
Dot player;
Dot enemy1;
Dot enemy2;
//Dot dot;

//Scene textures
LTexture gDotTexture;

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

bool init()
{
	//Initialization flag
	bool success = true;
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main( int argc, char* args[] )
{

	//Init Client
	myClient = new Client("149.153.136.242", 1111); //Create client to connect to server 127.0.0.1 [localhost] on port 1111
	myClient->playerDot = &player;
	myClient->enemy1Dot = &enemy1;
	myClient->enemy2Dot = &enemy2;
	int timer = 0;
	//myClient = new Client("127.0.0.1", 1111);
	if (!myClient->Connect()) //If client fails to connect...
	{
		std::cout << "Failed to connect to server..." << std::endl;
		system("pause");
		return -1;
	}


	//Start up SDL and create window
	if(!init())
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//The dot that will be moving around on the screen

			player.Init(gRenderer);
			enemy1.Init(gRenderer);
			enemy2.Init(gRenderer);



			//dot.Init(gRenderer);

			std::string userinput;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					//Handle input for the dot
					player.handleEvent( e );
					//dot.SetPosition(myClient->getX(), myClient->getY());

				}


				//Move the dot
				//dot.move(SCREEN_HEIGHT, SCREEN_WIDTH);
				player.move(SCREEN_HEIGHT, SCREEN_WIDTH);
				
				//			//Send dot instead of position alone

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//dot.render(gRenderer);
				player.render(gRenderer);
				if (enemy1.isInit) enemy1.render(gRenderer);
				if (enemy2.isInit) enemy2.render(gRenderer);
				
				//Update screen
				SDL_RenderPresent( gRenderer );
				//Sleep(5000);
				myClient->SendDotString(player.GetDotAsString());
				//myClient->SendString(player.GetDotAsString());

				if (player.getID() == 0) {
					if (timer > 0) {
						timer -= 1;
						
					}else if (player.Checkcollision(enemy1.GetCenterX(), enemy1.GetCenterY())) {
						if (player.isChaser) {
						myClient->SendChaserMessage("id:1");
						timer = 200;
						} else {
							myClient->SendChaserMessage("id:0");
							timer = 200;
						}
					} else if(player.Checkcollision(enemy2.GetCenterX(), enemy2.GetCenterY())) {
						if (player.isChaser) {
							myClient->SendChaserMessage("id:2");
							timer = 200;
						} else {
							myClient->SendChaserMessage("id:0");
							timer = 200;
						}
					}
					else if (enemy1.Checkcollision(enemy2.GetCenterX(), enemy2.GetCenterY())) {
						if (enemy1.isChaser) {
							myClient->SendChaserMessage("id:2");
							timer = 200;
						} else {
							myClient->SendChaserMessage("id:1");
							timer = 200;
						}
					}
				}

		}
	}

	//Free resources and close SDL
	close();

	return 0;
}