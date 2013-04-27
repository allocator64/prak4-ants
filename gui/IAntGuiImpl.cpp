#include "IAntGuiImpl.hpp"

namespace antgui
{
    void AntGuiQtImpl::SetAnt(const Ant &ant)
    {
        auto it = antMap.find(ant.getPoint());
/*
        if (it != antMap.end() && it->second->getTeamId() != new_ant->getTeamId())
        {
            throw GUIException("Point is used by another team ants");
        }
*/
        if (it == antMap.end() || *(it->second) < ConcreteAnt(ant))
        {
            antMap.insert(std::make_pair(ant.getPoint(), std::make_shared<ConcreteAnt>(ant)));
        }
    }

    struct PaintFood
    {
        void operator() (const std::map<Point,std::shared_ptr<const Food> >::value_type& p)
        {
            QFoodPainter foodPainter(*p.second);
            foodPainter.draw();
            widget->drawObject(p.second->getPoint(), foodPainter.pixmap());
        }
        PaintFood(AntWidget * widget, AntGuiQtImpl& antGuiImpl) : widget(widget), antGuiImpl(antGuiImpl) {}
    private:
        AntWidget * widget;
        AntGuiQtImpl& antGuiImpl;
    };

    struct PaintAnt
    {
        void operator() (const map<Point, std::shared_ptr<const Ant> >::value_type& p)
        {
 //           const Ant* ant = p.second;
            QAntPainter *antPainter = QAntPainter::getAntPainter(*p.second);
            antPainter->draw(antGuiImpl.teamColor[p.second->getTeamId()]);
            widget->drawObject(p.second->getPoint(), antPainter->pixmap());
            delete antPainter;
        }
        PaintAnt(AntWidget * widget, AntGuiQtImpl& antGuiImpl) : widget(widget), antGuiImpl(antGuiImpl) {}
    private:
        AntWidget * widget;
        AntGuiQtImpl& antGuiImpl;
    };

    void AntGuiQtImpl::Paint()
    {
        paintLock.lock();
        widget->clear();
        PaintFood paintFood(widget, *this);
        PaintAnt paintAnt(widget, *this);
        for_each(foodMap.begin(), foodMap.end(), paintFood);
        for_each(antMap.begin(), antMap.end(), paintAnt);
        paintLock.unlock();
    }

    QAntPainter* QAntPainter::getAntPainter(const Ant& ant)
    {
        QAntPainter *res;
        if (ant.hasFood())
        {
            res = new QFoodAntPainter(ant);
        }
        else if (ant.isFrozen())
        {
            res = new QFrozenAntPainter(ant);
        }
        else
        {
            res = new QAntPainter(ant);
        }
        return res;
    }
}
