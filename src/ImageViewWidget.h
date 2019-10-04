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

QT_FORWARD_DECLARE_CLASS(QMenu)

class ImageViewWidget : public QWidget
{
	Q_OBJECT

public:
	ImageViewWidget(ImageViewerPlugin* imageViewerPlugin);

public:
	void onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage, const double& imageMin, const double& imageMax);

private:
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

	QMenu* contextMenu();
	QMenu* viewMenu();
	QMenu* selectionMenu();

private:
	ImageViewerPlugin*		_imageViewerPlugin;
	ImageView*				_imageView;
	QGraphicsScene*			_scene;
	ImageCanvasWidget*		_imageWidget;
};