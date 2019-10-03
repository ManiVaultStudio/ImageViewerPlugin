#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QGraphicsSceneWheelEvent>

#include "Common.h"

class ImageView;
class ImageViewerPlugin;
class ImageCanvasWidget;

class ImageViewWidget : public QWidget
{
	Q_OBJECT

public:
	ImageViewWidget(ImageViewerPlugin* imageViewerPlugin);

private:
	ImageViewerPlugin*	_imageViewerPlugin;
	ImageView*			_imageView;
	QGraphicsScene*		_scene;
	ImageCanvasWidget*			myGLWidget;
};