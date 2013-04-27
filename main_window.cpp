#include "main_window.hpp"

#include "AntWidget.hpp"

#include <QLabel>
#include <QPixmap>
#include <QWidget>
#include <QPainter>
#include <QGridLayout>
#include <QPushButton>
#include <QTimer>
#include <memory>

enum {
    PLAYER_COUNT = 4,
    FIELD_SIZE = 31,
    ANT_COUNT = 10
};

main_window::main_window(QWidget *parent)
  : QMainWindow(parent), manager(FIELD_SIZE, FIELD_SIZE, PLAYER_COUNT, ANT_COUNT)
{
    this->step = 0;
    ui.setupUi(this);
    QLabel *label1 = new QLabel("One");
    QLabel *label2 = new QLabel("Two");
    QLabel *label3 = new QLabel("Three");
    QLabel *label4 = new QLabel("Four");
    QPushButton *buttonStart = new QPushButton("start");
    QPushButton *buttonStop = new QPushButton("stop");
    QPushButton *buttonStep = new QPushButton("step");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(nextStep()));
    ant = new AntWidget();
    ant->addLabel(label1);
    ant->addLabel(label2);
    ant->addLabel(label3);
    ant->addLabel(label4);
    QGridLayout *layout = new QGridLayout;
    layout->setRowStretch(1, 100);
    layout->addWidget(label1, 0, 0, Qt::AlignLeft);
    layout->addWidget(label2, 0, 2, Qt::AlignRight);
    layout->addWidget(label3, 2, 0, Qt::AlignLeft);
    layout->addWidget(label4, 2, 2, Qt::AlignRight);
    layout->addWidget(ant, 1, 0, 1, 3, Qt::AlignCenter);
    layout->addWidget(buttonStart, 3, 0, Qt::AlignCenter);
    layout->addWidget(buttonStop, 3, 1, Qt::AlignCenter);
    layout->addWidget(buttonStep, 3, 2, Qt::AlignCenter);

    connect(buttonStep, SIGNAL(clicked()), this, SLOT(nextStep()));
    connect(buttonStart, SIGNAL(clicked()), this, SLOT(Start()));
    connect(buttonStop, SIGNAL(clicked()), this, SLOT(Stop()));

    using namespace antgui;

    gui = std::dynamic_pointer_cast<AntGuiQtImpl>(IAntGui::getGUI());
    if (gui)
    {
        gui->init(ant);
    }
    ant->setGui(gui);
    manager.setGui(gui);

    manager.setFoodGeneretor(make_shared<food_iterator>(FIELD_SIZE));

    this->centralWidget()->setLayout(layout);
}

void main_window::nextStep(void)
{
    ant->setUpdatesEnabled(false);
    manager.step(step++);
    ant->setUpdatesEnabled(true);
    ant->repaint();
}

void main_window::Start()
{
    timer->start(20);
}

void main_window::Stop()
{
    timer->stop();
}
