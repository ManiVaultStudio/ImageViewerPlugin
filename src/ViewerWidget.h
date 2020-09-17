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

/**
 * Viewer widget class
 * 
 * This is an OpenGL widget class that renders images from a layer based system
 * 
 * @author Thomas Kroes
 */
class ViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public: // Construction/destruction
	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	ViewerWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~ViewerWidget() override;

public: // Events

	/**
	 * Widget event capture
	 *@param target Target object
	 *@param event Event that occurred
	 */
	bool eventFilter(QObject* target, QEvent* event) override;

protected: // OpenGL functions

	/** Initializes the OpenGL window */
	void initializeGL() override;

	/** Paints the OpenGL content */
	void paintGL() override;

	/**
	 * Invoked when the OpenGL viewport changes size
	 * @param w Width of the viewport
	 * @param h Height of the viewport
	 */
	void resizeGL(int w, int h) override;

	/**
	 * Draws a gradient background
	 * @param painter Pointer to painter
	 */
	void drawBackground(QPainter* painter);

private:

	/** Returns the context menu */
	QMenu* getContextMenu();

private:
	ImageViewerPlugin*						_imageViewerPlugin;			/** Pointer to the image viewer plugin */
	Renderer*								_renderer;					/** Pointer to the renderer which is attached to the viewer widget */
	std::unique_ptr<QOpenGLDebugLogger>		_openglDebugLogger;			/** OpenGL logger instance for debugging (only enabled in debug mode for performance reasons) */
	QRadialGradient							_backgroundGradient;		/** Viewport gradient background */
	int										_keys;						/** Currently pressed keyboard keys */
};