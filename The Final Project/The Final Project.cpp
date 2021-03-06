#include <iostream>
#include <string>
#include <stdafx.h>

using namespace std;

#include "olcConsoleGameEngine.h"


class Flappyb : public olcConsoleGameEngine
{
public:
	Flappyb()
	{
		m_sAppName = L"Blappy Fird";
	}




private:
	float fBirdPosition = 0.0f;
	float fBirdVelocity = 0.0f;
	float fBirdAcceleration = 0.0f;
	float fLevelPosition = 0.0f;
	float fGravity = 100.0f;


	float fSectionWidth; //defines the width of the section
	list<int> listSection;


	bool bHasCollided = false; //flag for has collided 
	bool bResetGame = false;

	//scoring variables, counting via flap count makes scoring easier :)
	int nAttemptCount = 0;
	int nFlapCount = 0;
	int nMaxFlapCount = 0;

protected:
	//called on by olcConsoleGameengine
	virtual bool OnUserCreate()
	{
		listSection = { 0, 0, 0, 0 };
		bResetGame = true;
		fSectionWidth = (float)ScreenWidth() / (float)(listSection.size() - 1);
		return true;
	}
	//called by olcConsoleGameEngine
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		if (bResetGame)
		{
			bHasCollided = false;
			bResetGame = false;
			listSection = { 0, 0, 0, 0 }; // sets difficulty
			fBirdAcceleration = 0.0f; //starts birds accel at 0 again
			fBirdVelocity = 0.0f; //starts birds velocity at 0 again
			fBirdPosition = ScreenHeight() / 2.0f; //centers the bird again
			nFlapCount = 0; //resets flap count
			nAttemptCount++; //adds one attempt mark after youve done goofed
		}


		// reinitiates game
		if (bHasCollided)
		{
			if (m_keys[VK_SPACE].bReleased) //nothing happens until you release space
				bResetGame = true;
		}
		else
		{

			if (m_keys[VK_SPACE].bPressed && fBirdVelocity >= fGravity / 10.0f) //staggers jumps to make game harder

			{		//forces the bird to go up because down is defined to be positive
				fBirdAcceleration = 0.0f;
				fBirdVelocity = -fGravity / 4.0f; //caps
				nFlapCount++; //adds the flapcount 
				if (nFlapCount > nMaxFlapCount)
					nMaxFlapCount = nFlapCount;
			}
			else
				fBirdAcceleration += fGravity * fElapsedTime;

			//puts constraint on acceleration and the gravity
			if (fBirdAcceleration >= fGravity)
				fBirdAcceleration = fGravity;

			fBirdVelocity += fBirdAcceleration * fElapsedTime;
			fBirdPosition += fBirdVelocity * fElapsedTime;
			fLevelPosition += 14.0f * fElapsedTime; //makes the bird travel smooth, increasing makes the level go faster


			if (fLevelPosition > fSectionWidth)
			{
				fLevelPosition -= fSectionWidth; //the speed of the platform may affect the level position of the step size 
				listSection.pop_front(); //pops platforms to the front of the list when we are done with them
				int i = rand() % (ScreenHeight() - 20); //randomizes the size of platforms
				if (i <= 10) i = 0; //gives the player a rest, randomly :)
				listSection.push_back(i);
			}



			// Le' Display
			Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

			//From here we draw the sections
			int nSection = 0; //lets us know where we are
			for (auto s : listSection) //a forloop that will draw things out
			{
				if (s != 0)
				{	//makes the top part of the platform
					Fill(nSection * fSectionWidth + 10 - fLevelPosition, ScreenHeight() - s, //gives the impression that the level is scrolling past the bird
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight(),
						PIXEL_SOLID, FG_GREEN); //this makes green platforms with an offset so the game isnt too hard :)

					Fill(nSection * fSectionWidth + 10 - fLevelPosition, 0,
						nSection * fSectionWidth + 15 - fLevelPosition, ScreenHeight() - s - 15,
						PIXEL_SOLID, FG_GREEN); //This makes the bottom part of the platform
				}
				nSection++;
			}
			int nBirdX = (int)(ScreenWidth() / 3.0f);

			// Collision Detection
			bHasCollided = fBirdPosition < 2 || fBirdPosition > ScreenHeight() - 2 || //prevents bird from hitting top/bottom of screen
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX].Char.UnicodeChar != L' ' ||
				m_bufScreen[(int)(fBirdPosition + 0) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ' || //for if the front of the bird collides
				m_bufScreen[(int)(fBirdPosition + 1) * ScreenWidth() + nBirdX + 6].Char.UnicodeChar != L' ';


			//Draw Bird
			if (fBirdVelocity > 0)
			{
				DrawString(nBirdX, fBirdPosition + 0, L"\\\\\\");
				DrawString(nBirdX, fBirdPosition + 1, L"\\\\\\=Q");
			}
			else
			{
				DrawString(nBirdX, fBirdPosition + 0, L"<///=Q");
				DrawString(nBirdX, fBirdPosition + 1, L"///");
			}

			//DrawString(1, 1, L"Attempt: " + to_wstring(nAttemptCount) + L"Score: " + to_wstring(nFlapCount) + L" High Score: " + to_wstring(nMaxFlapCount));
		}

		return true;

	}
};


int main()
{ //ACTUALLY makes the game
	Flappyb game;
	game.ConstructConsole(80, 48, 16, 16);
	game.Start();

	return 0;
}