#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QGraphicsSceneWheelEvent>

#include "Common.h"

class ImageGraphicsView;
class ImageViewerPlugin;
class ImageWidget;

QT_FORWARD_DECLARE_CLASS(QMenu)

class ImageViewWidget : public QWidget
{
	Q_OBJECT

public:
	ImageViewWidget(ImageViewerPlugin* imageViewerPlugin);

private:
	ImageGraphicsView*	_imageGraphicsView;
};