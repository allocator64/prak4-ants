#ifndef ANTMANAGER_HPP
#define ANTMANAGER_HPP

#include "IAntGUI.hpp"
#include "IAntLogic.hpp"

#include <memory>

class food_iterator {
    int size;
public:
    food_iterator(int size) : size(size) {}
};

class AntManager
{
    int maxAntCountPerTeam;
    int teamCount;
    int height;
    int width;
    std::shared_ptr<antgui::IAntGui> gui;
public:
    AntManager(int height,
               int width,
               int teamCount,
               int maxAntCountPerTeam);

    void step(int iRun);
    void setGui(std::shared_ptr<antgui::IAntGui> gui);
    void setFoodGeneretor(std::shared_ptr<food_iterator> it);
};

#endif
