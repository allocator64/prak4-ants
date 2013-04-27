#ifndef ANTGUIIMPL_HPP
#define ANTGUIIMPL_HPP

#include "IAntGUI.hpp"
#include "AntWidget.hpp"

#include <QPainter>
#include <QMutex>
#include <QTextFormat>

#include <map>
#include <stack>
#include <iostream>
#include <memory>
#include <algorithm>

#define ANT_SIZE 15

using namespace std;

namespace antgui
{
    class AntGuiQtImpl;

    class ConcreteAnt : public Ant
    {
    private:
        bool food;
        bool frozen;
        Point point;
        int teamId;
    public:
        ConcreteAnt(bool food, bool frozen, Point &p, int teamId)
            : food(food), frozen(frozen), point(p), teamId(teamId) {}
        ConcreteAnt(const Ant &ant) {
            food = ant.hasFood();
            frozen = ant.isFrozen();
            point = ant.getPoint();
            teamId = ant.getTeamId();
        }

        bool hasFood() const { return food;}
        bool isFrozen() const { return  frozen;}
        Point getPoint() const {return point;}
        int getTeamId() const { return teamId;}

        virtual ~ConcreteAnt() {

        }
    };


    class ConcreteFood : public antgui::Food
    {
        Point p;
        int count;
    public:
        ConcreteFood() {}
        ConcreteFood(const Point &p, int count) : p(p), count(count) {}
        ConcreteFood(const Food &food) {
            p = food.getPoint();
            count = food.getCount();
        }

        virtual Point getPoint() const
        {
            return p;
        }

        virtual int getCount() const
        {
            return count;
        }

        virtual ~ConcreteFood()
        {

        }
    };

	class QFieldObject
	{
	protected:
		QPixmap pix;
		QFieldObject() : pix(ANT_SIZE, ANT_SIZE) {}
	public:
		const QPixmap& pixmap() const
		{
			return pix;
		}
	};

	class QFoodPainter : public QFieldObject
	{
		const Food &food;
	public:
		QFoodPainter(const Food &food) : food(food) {}
		virtual void draw()
		{
			QPainter painter(&pix);
            int color = 220 - (food.getCount()/5 > 220 ? 220: food.getCount()/5);
            painter.setBrush(QBrush(QColor(0, 0, 0)));
            painter.drawRect(0, 0, ANT_SIZE, ANT_SIZE);
            painter.setBrush(QBrush(QColor(color, color, color)));
			painter.drawEllipse(0, 0, ANT_SIZE, ANT_SIZE);
		}
	};

	class QAntPainter : public QFieldObject
	{
    protected:
		QAntPainter(const Ant &ant) : ant(ant) {}
		const Ant &ant;
	public:
		virtual void draw(const QColor& color)
		{
			QPainter painter(&pix);
			painter.fillRect(0, 0, ANT_SIZE, ANT_SIZE, color);
		}

        virtual ~QAntPainter() {}

		static QAntPainter* getAntPainter(const Ant& ant);
	};

	class QFrozenAntPainter : public QAntPainter
	{
	public:
		QFrozenAntPainter(const Ant &ant) : QAntPainter(ant) {}
		virtual void draw(const QColor& color)
		{
			QAntPainter::draw(color);
			QPainter painter(&pix);
			painter.drawLine(0, 0, ANT_SIZE, ANT_SIZE);
			painter.drawLine(0, ANT_SIZE, ANT_SIZE, 0);
		}

	};

	class QFoodAntPainter : public QAntPainter
	{
	public:
		QFoodAntPainter(const Ant &ant) : QAntPainter(ant) {}
		virtual void draw(const QColor& color)
		{
			QAntPainter::draw(color);
            QPainter painter(&pix);
			painter.setBrush(QBrush(Qt::white));
            painter.drawEllipse(1, 1, ANT_SIZE - 2, ANT_SIZE - 2);
		}
	};

	class AntGuiQtImpl : public IAntGui
	{
		AntWidget *widget;
		QMutex paintLock;
        map<Point, std::shared_ptr<const Food> > foodMap;
        map<Point, std::shared_ptr<const Ant> > antMap;
	public:
        map<int, QColor> teamColor;

        void SetTeamScore(int teamId, int score)
        {
            widget->getLabel(teamId).setText(QString("team: %1 score: %2").arg(teamId).arg(score));
        }

		AntGuiQtImpl()
		{
		}

		virtual void Clear()
		{
            antMap.clear();
            foodMap.clear();
		}

        void init(AntWidget* widget)
		{
			this->widget = widget;
			teamColor.insert(pair<int, QColor>(0, QColor(Qt::red)));
			teamColor.insert(pair<int, QColor>(1, QColor(Qt::green)));
			teamColor.insert(pair<int, QColor>(2, QColor(Qt::blue)));
			teamColor.insert(pair<int, QColor>(3, QColor(Qt::yellow)));
		}

        virtual void SetAnt(const Ant &ant);

        virtual void SetFood(const Food &food)
		{
            foodMap.insert(std::make_pair(food.getPoint(), std::make_shared<ConcreteFood>(food)));
		}

		virtual void Paint();

		virtual void BeginPaint()
		{
			paintLock.lock();
			Clear();
		}

		virtual void EndPaint()
		{
			paintLock.unlock();
		}
	};

}

#endif
