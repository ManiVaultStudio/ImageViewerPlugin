#pragma once

#include "Common.h"
#include "Renderer.h"

#include "ImageData/ImageData.h"

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QColor>
#include <QRect>
#include <QOpenGLDebugLogger>

class QMenu;

class ImageViewerPlugin;
class LayersModel;

/**
 * Image viewer widget class
 * This widget displays high-dimensional image data
 * @author Thomas Kroes
 */
class ViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	/** Default constructor */
	ViewerWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~ViewerWidget() override;

public: // Selections

	/** Publish selection to HDPS */
	void publishSelection();

private:
	/** Initializes OpenGL */
	void initializeGL() override;

	/** Paints the OpenGL content */
	void paintGL() override;

private:
	/**
	 * Invoked when a key is pressed
	 * @param keyEvent Key event
	 */
	void keyPressEvent(QKeyEvent* keyEvent) override;

	/**
	 * Invoked when a key is released
	 * @param keyEvent Key event
	 */
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

	/**
	 * Invoked when the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePressEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void mouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void wheelEvent(QWheelEvent* wheelEvent) override;

private:

	/** TODO */
	LayersModel* layersModel();

	/** Returns the context menu */
	QMenu* contextMenu();

private:
	ImageViewerPlugin*						_imageViewerPlugin;			/** TODO */
	Renderer*								_renderer;					/** TODO */
	std::unique_ptr<QOpenGLDebugLogger>		_openglDebugLogger;			/** TODO */
};