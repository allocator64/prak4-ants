#ifndef ANTWIDGET_HPP
#define ANTWIDGET_HPP

#include "IAntGUI.hpp"

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QPen>
#include <QLabel>

#include <iostream>
#include <string>
#include <memory>
#include <vector>

class AntWidget : public QWidget
{
   Q_OBJECT


   private:
	  QPixmap pix_;
	  QPen pen_;
      std::vector<QLabel*> teamLabels;
      std::shared_ptr<antgui::IAntGui> gui;
   public:
      AntWidget(QWidget *parent=0);

      void addLabel(QLabel* label)
      {
          teamLabels.push_back(label);
      }

      QLabel& getLabel(int id)
      {
          return *teamLabels[id];
      }

	  void drawObject(const antgui::Point&, const QPixmap&);
	  inline QPixmap const& pixmap() const { return pix_;}
	  void clear()
	  {
          QPainter painter(this);
          painter.setPen(pen_);
	  }
      void setGui(std::shared_ptr<antgui::IAntGui> gui)
	  {
		  this->gui = gui;
	  }
   signals:
	  void antViewChanged();
   protected:
	  void paintEvent(QPaintEvent* ev);
};

#endif // ANT_WIDGET_HPP
