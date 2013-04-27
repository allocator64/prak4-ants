#include "IAntLogic.hpp"

#include "IAntLogicImpl.hpp"

namespace antlogic
{
    std::shared_ptr<IAntLogic> IAntLogic::GetAntLogic(int teamId)
    {
        switch (teamId % 2)
        {
        case 0:
            return std::make_shared<AntLogicTeam01>();
        default:
            return std::make_shared<AntLogicTeam02>();
        }
    }
}

