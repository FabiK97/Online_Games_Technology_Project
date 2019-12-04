#pragma once
//#include "stdafx.h"
#include <stdio.h>
#include <string>


class Dot
{
public:
	//The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	//Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	Dot() {};
	//Initializes the variables
	Dot(bool chaser);
	~Dot();

	//Moves the dot
	void move(int SCREEN_HEIGHT, int SCREEN_WIDTH);

	
	std::string GetPosAsString();
	std::string GetDotAsString();

	bool Checkcollision(int centerX, int centerY);

	bool GetLocal() { return isLocalplayer; };

	int GetCenterX();
	int GetCenterY();

	int getID() {
		return id;
	};
	void setID(int id) { this->id = id; }
	void SetPosition(int x, int y);

private:
	int id;
	bool isLocalplayer;
	bool isChaser;


	//The X and Y offsets of the dot
	int mPosX, mPosY;

	int width, height;

	int mCenterX, mCenterY;
	//The velocity of the dot
	int mVelX, mVelY;

};