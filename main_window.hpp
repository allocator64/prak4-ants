#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include "ui_MainForm.h"

#include "AntManager.hpp"
#include "IAntGuiImpl.hpp"

#include <memory>
#include <QDialog>

class main_window : public QMainWindow
{
  Q_OBJECT
public:
    AntManager manager;

    QTimer *timer;

    int heightForWidth ( int w ) const
    {
        return w;
    }
    main_window(QWidget * parent = 0);
public slots:
    void nextStep(void);
    void Stop(void);
    void Start(void);
private:
    int step;
    Ui::MainWindow ui;
    std::shared_ptr<antgui::AntGuiQtImpl> gui;
    AntWidget *ant;
};


#endif //MAIN_WINDOW_HPP
