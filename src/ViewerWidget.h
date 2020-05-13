#pragma once

#include "Common.h"
#include "Renderer.h"

#include "ImageData/ImageData.h"

#include <memory>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QRadialGradient>

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

private: // Mouse events

	/**
	 * Invoked when the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePressEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void mouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void wheelEvent(QWheelEvent* wheelEvent) override;

private:

	/** Initializes OpenGL */
	void initializeGL() override;

	/** Paints the OpenGL content */
	void paintGL() override;
	
	/**
	 * Draws a gradient background
	 * @param painter Pointer to painter
	 */
	void drawBackground(QPainter* painter);

private:

	/** Returns the context menu */
	QMenu* contextMenu();

private:
	ImageViewerPlugin*						_imageViewerPlugin;			/** TODO */
	Renderer*								_renderer;					/** TODO */
	std::unique_ptr<QOpenGLDebugLogger>		_openglDebugLogger;			/** TODO */
	QRadialGradient							_backgroundGradient;		/** Viewport gradient background */
};