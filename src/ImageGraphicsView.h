#pragma once

#include "Common.h"

#include <QGraphicsView>

QT_FORWARD_DECLARE_CLASS(ImageViewerPlugin)
QT_FORWARD_DECLARE_CLASS(ImageWidget)

QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QContextMenuEvent)
QT_FORWARD_DECLARE_CLASS(QKeyEvent)
QT_FORWARD_DECLARE_CLASS(QMouseEvent)
QT_FORWARD_DECLARE_CLASS(QWheelEvent)
QT_FORWARD_DECLARE_CLASS(QGraphicsScene)

class ImageGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit ImageGraphicsView(ImageViewerPlugin* imageViewerPlugin, QWidget *parent = 0);

public:
	InteractionMode interactionMode() const;
	void setInteractionMode(const InteractionMode& interactionMode);
	void zoomToExtents();

private:
	QMenu* windowLevelMenu();
	QMenu* viewMenu();
	QMenu* selectionMenu();

	void contextMenuEvent(QContextMenuEvent* contextMenuEvent) Q_DECL_OVERRIDE;

	void onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage, const double& imageMin, const double& imageMax);

private:
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* wheelEvent);

private:
	ImageViewerPlugin*	_imageViewerPlugin;
	QGraphicsScene*		_graphicsScene;
	ImageWidget*		_imageWidget;
	InteractionMode		_interactionMode;
	QPoint				_mousePosition;
};