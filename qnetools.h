#ifndef QNETOOLS_H
#define QNETOOLS_H

#include <QGraphicsPathItem>
#include <QGraphicsPixmapItem>
#include <QtXml/QDomDocument>
#include <QMainWindow>
#include <QtWidgets>

#include "qnetools.h"

class ToolItem : public QWidget
{
public:
  ToolItem(const QString &title, QWidget *item) : item(item) 
  {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(title));
    layout->addWidget(item);    
    setLayout(layout);

    item->setVisible(false);
  }

protected:
  void mousePressEvent(QMouseEvent *event) 
  {
    item->setVisible(!item->isVisible());
  }

private:
  QWidget *item;
};

class ToolBox : public QWidget
{
public:
  ToolBox() : layout(new QVBoxLayout) 
  {
    setLayout(layout);
  }

  void addItem(ToolItem *item) 
  {
    // Remove last spacer item if present.
    int count = layout->count();
    if (count > 1) 
	{
      layout->removeItem(layout->itemAt(count - 1));
    }

    // Add item and make sure it stretches the remaining space.
    layout->addWidget(item);    
    layout->addStretch();
  }

private:
  QVBoxLayout *layout;
};

#endif // QNETOOLS_H
