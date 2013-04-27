#include "AntManager.hpp"
#include <memory.h>

//
//  AntManager
//

AntManager::AntManager(int height, int width, int teamCount, int maxAntCountPerTeam)
{
	this->height = height;
	this->width = width;
	this->teamCount = teamCount;
	this->maxAntCountPerTeam = maxAntCountPerTeam;
}

void AntManager::step(int step)
{
    gui->BeginPaint();

    //gui->SetAnt(ant);
    //gui->SetFood(food);

    gui->EndPaint();

    // scoreboard

    for (unsigned int t = 0; t < teamCount; t++)
    {
        gui->SetTeamScore(t, rand() % 100);

    }
}

void AntManager::setGui(std::shared_ptr<antgui::IAntGui> gui)
{
	this->gui = gui;
}

void AntManager::setFoodGeneretor(std::shared_ptr<food_iterator> it)
{

}
