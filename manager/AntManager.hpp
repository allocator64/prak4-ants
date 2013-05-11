#ifndef ANTMANAGER_HPP
#define ANTMANAGER_HPP

#include "IAntGUI.hpp"
#include "IAntGuiImpl.hpp"
#include "IAntLogic.hpp"

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <cmath>


struct point
{
    int x, y;
public:
    point(int x = 0, int y = 0) : x(x), y(y){}
    point(const point &p) : x(p.x), y(p.y){}

    friend bool operator < (const point &a, const point &b)
    {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
    friend bool operator == (const point &a, const point &b)
    {
        return a.x == b.x && a.y == b.y;
    }
    point & operator += (const point &p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    point & operator -= (const point &p)
    {
        x *= p.x;
        y *= p.y;
        return *this;
    }

    friend point operator + (const point &a, const point &b)
    {
        point tmp = a;
        tmp += b;
        return tmp;
    }

    friend point operator - (const point &a, const point &b)
    {
        point tmp = a;
        tmp -= b;
        return tmp;
    }
};


class food_iterator: public std::iterator< std::input_iterator_tag, antgui::Food>
{
    int size;
    int x;
    std::vector<std::vector<int> > field;

    double f(int i, int dx)
    {
        return 1 - abs(i - dx / 2.0) / (dx / 2.0);
        //double arg = i - dx / 2.0;
        //double sigma2 = 5.0;
        //return 1/sqrt(2 * M_PI * sigma2) * exp(-(arg * arg)/(2 * sigma2));
    }

    void fillV(int x, int y, int dx, int dy, int n, int id = 0)
    {
        if(dx < 3 || dy < 3 || n < 0)
            return;

        for(int i = 0; i < dx; ++i)
        {
            field[y + dy / 2 - 1][x + i] += (n / 3.0) * f(i, dx);
            field[y + dy / 2][x + i] += n * f(i, dx);
            field[y + dy / 2 + 1][x + i] += (n / 3.0) * f(i, dx);
        }
        for(int j = 0; j < dy; ++j)
        {
            field[y + j][x + dx / 2 - 1] += (n / 3.0) * f(j, dy);
            field[y + j][x + dx / 2] += n * f(j, dy);
            field[y + j][x + dx / 2 + 1] += (n / 3.0) * f(j, dy);
        }

#define D_Q fillV(x, y, dx / 2, dy / 2, n - n / 3, 1);
#define D_R fillV(x + dx / 2 + 1, y, dx / 2, dy / 2, n - n / 3, 2);
#define D_T fillV(x + dx / 2 + 1, y + dy / 2 + 1, dx / 2, dy / 2, n - n / 3, 3);
#define D_S fillV(x, y + dy / 2 + 1, dx / 2, dy / 2, n - n / 3, 4);

        switch(id)
        {
        case 0:
            D_Q D_R D_T D_S
            break;
        case 1:
            D_R D_T D_S
            break;
        case 2:
            D_Q D_T D_S
            break;
        case 3:
            D_Q D_R D_S
            break;
        case 4:
            D_Q D_R D_T
            break;
        }
    }
public:
    food_iterator() : size(-1), x(0), field() {}
    food_iterator(int size) : size(size), x(0), field(size, std::vector<int>(size, 0))
    {
        fillV(0, 0, size, size, 1000);
    }
    food_iterator(const food_iterator & p) :size(p.size), x(p.x), field(p.field) {}

    food_iterator& operator++()
    {
        ++x;
        return *this;
    }
    food_iterator operator++(int)
    {
        food_iterator tmp(*this);
        operator++();
        return tmp;
    }
    friend bool operator == (const food_iterator &a, const food_iterator &b)
    {
        return
                (a.size == b.size && a.x == b.x) ||
                (a.x == a.size * a.size && b.size == -1) ||
                (b.x == b.size * b.size && a.size == -1);
    }
    friend bool operator != (const food_iterator &a, const food_iterator &b)
    {
        return !(a == b);
    }

    std::shared_ptr<antgui::Food> operator *()
    {
        int i = x / size;
        int j = x % size;
        return std::shared_ptr<antgui::Food> (new antgui::ConcreteFood(antgui::Point(i, j), field[i][j]));
    }
};

class AntObject : public antlogic::Ant
{
    char *_mem;
    bool _hasFood;
    int _teamId;
    int _stun;
    bool _flag;
    point _p;
public:
    AntObject(const point &p, int teamid)
        :_mem(nullptr), _hasFood(false), _teamId(teamid), _stun(0), _flag(false), _p(p)
    {
        _mem = new char[antlogic::MAX_MEMORY];
        memset(_mem, 0, antlogic::MAX_MEMORY);
    }

    virtual bool hasFood() const{
        return _hasFood;
    }
    bool & chFood()
    {
        return _hasFood;
    }

    virtual int getTeamId() const{
        return _teamId;
    }
    virtual char * getMemory() const{
        return _mem;
    }
    virtual bool isFrozen() const{
        return _stun != 0;
    }
    virtual antgui::Point getPoint() const{
        return antgui::Point(_p.x, _p.y);
    }

    point & getP()
    {
        return _p;
    }

    antlogic::Ant * getLogic()
    {
        return static_cast<antlogic::Ant *>(this);
    }
    std::shared_ptr<antgui::Ant> getGui() const
    {
        return std::shared_ptr<antgui::Ant>(new antgui::ConcreteAnt(hasFood(), isFrozen(), getPoint(), getTeamId()));
    }

    friend bool operator < (const AntObject &a, const AntObject & b)
    {
        return *(a.getGui()) < *(b.getGui());
    }

    bool & getFlag()
    {
        return _flag;
    }

    void stun()
    {
        _stun = 8;
    }
    void update()
    {
        if(_stun > 0)
            _stun--;
        _flag = false;
    }
};

class AntManager
{
    int maxAntCountPerTeam;
    int teamCount;
    int height;
    int width;
    bool isInitialized;
    std::vector<int> antCount;
    std::vector<int> antSpawn;
    std::vector<point> antRespawnPos;
    std::shared_ptr<antgui::IAntGui> gui;

    food_iterator foodGenerator;

    std::vector<std::vector<int> > food;
    std::vector<std::vector<std::pair<int,int> > > smell;
    std::vector<std::vector< std::set<std::shared_ptr<AntObject > > > > ants;

    bool inrange(int x, int y)
    {
        return x >= 0 && y >= 0 && x < width && y < height;
    }

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
