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

	/**
	 * Event filter
	 *@param target Target object
	 *@param event Event that occurred
	 */
	bool eventFilter(QObject* target, QEvent* event) override;

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
	int										_keys;						/** TODO */
};