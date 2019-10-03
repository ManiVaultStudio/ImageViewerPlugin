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

class ImageViewWidget : public QWidget
{
	Q_OBJECT

public:
	ImageViewWidget(ImageViewerPlugin* imageViewerPlugin);

	void SetTestImage();

public:
	void onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage);

private:
	ImageViewerPlugin*	_imageViewerPlugin;
	ImageView*			_imageView;
	QGraphicsScene*		_scene;
	QWidget*			myGLWidget;
};