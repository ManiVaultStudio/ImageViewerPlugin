#include "ImageView.h"

ImageView::ImageView(QWidget *parent /*= 0*/) :
	QGraphicsView(parent)
{
	setDragMode(QGraphicsView::ScrollHandDrag);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}