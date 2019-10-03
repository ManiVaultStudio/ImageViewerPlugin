#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QKeyEvent>

class ImageView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit ImageView(QWidget *parent = 0);

protected Q_SLOTS:
	void wheelEvent(QWheelEvent *event)
	{
		if (event->delta() > 0)
			scale(1.25, 1.25);
		else
			scale(0.8, 0.8);
	}
};