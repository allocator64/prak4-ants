#include "AntManager.hpp"
#include <memory.h>

//
//  AntManager
//

AntManager::AntManager(int height, int width, int teamCount, int maxAntCountPerTeam)
    :isInitialized(false), antCount(teamCount), antSpawn(teamCount, -1000), antRespawnPos(4),
      food(height, std::vector<int>(width, 0)),
      smell(height, std::vector<std::pair<int,int> >(width)),
      ants(height, std::vector< std::set<std::shared_ptr<AntObject> > >(width))
{
    this->height = height;
    this->width = width;
    this->teamCount = teamCount;
    this->maxAntCountPerTeam = maxAntCountPerTeam;

    antRespawnPos[0] = point(0, 0);
    antRespawnPos[1] = point(width - 1, 0);
    antRespawnPos[2] = point(0, height - 1);
    antRespawnPos[3] = point(width - 1, height - 1);
    antRespawnPos.resize(teamCount);
    antRespawnPos.shrink_to_fit();
}

void AntManager::step(int step)
{
    if(!isInitialized)
    {
        while(foodGenerator != food_iterator())
        {
            auto f = *foodGenerator;
            food[f->getPoint().y][f->getPoint().x] = f->getCount();
            foodGenerator++;
        }
        isInitialized = true;
    }
    gui->BeginPaint();

    for(int i = 0; i < height; ++i)
        for(int j = 0; j < width; ++j)
            gui->SetFood(antgui::ConcreteFood(antgui::Point(j, i), food[i][j]));

    for(int i = 0; i < height; ++i)
        for(int j = 0; j < width; ++j)
            for(auto &Ant : ants[i][j])
                gui->SetAnt(*(Ant->getGui()));

    gui->EndPaint();

    for(int i = 0; i < teamCount; ++i)
    {
        if(antCount[i] < maxAntCountPerTeam && step - antSpawn[i] > 10)
        {
            antCount[i]++;
            antSpawn[i] = step;

            ants[antRespawnPos[i].y][antRespawnPos[i].x].insert(std::shared_ptr<AntObject>(new AntObject(antRespawnPos[i], i)));
        }
    }


    for(int team = 0; team < teamCount; ++team)
    {
        for(int y = 0; y < height; ++y)
        {
            for(int x = 0; x < width; ++x)
            {
                auto safeIter = ants[y][x];
                for(auto & AntPtr : safeIter)
                {
                    if(AntPtr->getTeamId() == team && AntPtr->getFlag() == false)
                    {
                        std::cout << "Processing ant : team = " << team << std::endl;
                        std::shared_ptr<antlogic::IAntLogic> logic = antlogic::IAntLogic::GetAntLogic(AntPtr->getTeamId());
                        antlogic::AntSensor sensor[3][3];

                        for(int dy = -1; dy <= 1; ++dy)
                        {
                            for(int dx = -1; dx <= 1; ++dx)
                            {
                                int cx = x + dx;
                                int cy = y + dy;
                                point pos(cx, cy);

#define SENSOR sensor[dx+1][dy+1]

                                if(!inrange(cx, cy))
                                    SENSOR.isWall = true;
                                else
                                {
                                    if(food[cy][cx] > 0)
                                        SENSOR.isFood = true;
                                    auto it = std::find(antRespawnPos.begin(), antRespawnPos.end(), pos);
                                    if(it != antRespawnPos.end())
                                    {
                                        if(it - antRespawnPos.begin() == team)
                                           SENSOR.isMyHill = true;
                                        else
                                            SENSOR.isEnemyHill = true;
                                    }
                                    std::set<std::shared_ptr<AntObject> > Ants = ants[cy][cx];
                                    for(auto &Ant : Ants)
                                    {
                                        if(Ant->getTeamId() == AntPtr->getTeamId())
                                            SENSOR.isFriend = true;
                                        else
                                            SENSOR.isEnemy = true;
                                    }
                                    SENSOR.smell = smell[cy][cx].first;
                                    SENSOR.smellIntensity = smell[cy][cx].second;
                                }
                            }
                        }

                        for(int i = 0; i < 3; ++i)
                        {
                            for(int j = 0; j < 3; ++j)
                                std::cout << sensor[i][j].isWall << " ";
                            std::cout << std::endl;
                        }

                        antlogic::AntAction res = logic->GetAction(*AntPtr->getLogic(), sensor);
                        if(res.putSmell)
                        {
                            smell[y][x].first = res.smell;
                            smell[y][x].second = 100;
                        }
                        std::cout << "y=" << y << " x=" << x << " action = " << res.actionType << std::endl;
                        std::cout << "MEMORY : ";
                        for(int i = 0; i < antlogic::MAX_MEMORY; ++i)
                            std::cout << (int)AntPtr->getMemory()[i] << " ";
                        std::cout << std::endl;
                        switch(res.actionType)
                        {
                        case antlogic::AntActionType::MOVE_UP:
                            if(inrange(x, y - 1))
                            {
                                AntPtr->getP().y -=1;
                                ants[y - 1][x].insert(AntPtr);
                                ants[y][x].erase(AntPtr);
                            }
                            else
                                std::cout << "bad move" << std::endl;
                            break;
                        case antlogic::AntActionType::MOVE_LEFT:
                            if(inrange(x - 1, y))
                            {
                                AntPtr->getP().x -=1;
                                ants[y][x - 1].insert(AntPtr);
                                ants[y][x].erase(AntPtr);
                            }
                            else
                                std::cout << "bad move" << std::endl;
                            break;
                        case antlogic::AntActionType::MOVE_DOWN:
                            if(inrange(x, y + 1))
                            {
                                AntPtr->getP().y +=1;
                                ants[y + 1][x].insert(AntPtr);
                                ants[y][x].erase(AntPtr);
                            }
                            else
                                std::cout << "bad move" << std::endl;
                            break;
                        case antlogic::AntActionType::MOVE_RIGHT:
                            if(inrange(x + 1, y))
                            {
                                AntPtr->getP().x +=1;
                                ants[y][x + 1].insert(AntPtr);
                                ants[y][x].erase(AntPtr);
                            }
                            else
                                std::cout << "bad move" << std::endl;
                            break;
                        case antlogic::AntActionType::BITE_UP:
                            break;
                        case antlogic::AntActionType::BITE_LEFT:
                            break;
                        case antlogic::AntActionType::BITE_DOWN:
                            break;
                        case antlogic::AntActionType::BITE_RIGHT:
                            break;
                        case antlogic::AntActionType::GET:
                            AntPtr->chFood() = true;
                            food[y][x]--;
                            break;
                        case antlogic::AntActionType::PUT:
                            food[y][x]++;
                            AntPtr->chFood() = false;
                            break;
                        }

                        AntPtr->getFlag() = true;
                    }
                }
            }
        }
    }

    std::cout << "done\n";

    for(int i = 0; i < height; ++i)
    {
        for(int j = 0; j < width; ++j)
        {
            for(auto & AntPtr : ants[i][j])
            {
                AntPtr->update();
            }
        }
    }



    // scoreboard

    for (unsigned int t = 0; t < teamCount; t++)
    {
        gui->SetTeamScore(t, food[antRespawnPos[t].y][antRespawnPos[t].x]);
    }
}

void AntManager::setGui(std::shared_ptr<antgui::IAntGui> gui)
{
    this->gui = gui;
}

void AntManager::setFoodGeneretor(std::shared_ptr<food_iterator> it)
{
    foodGenerator = *it;
    isInitialized = false;
}
