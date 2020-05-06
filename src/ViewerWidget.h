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

protected:

	/**
	 * Handles paint events
	 * Initiated by calls to the update function
	 * @param paintEvent Pointer to the paint event
	 */
	void paintEvent(QPaintEvent* paintEvent);

private:

	/** Returns the context menu */
	QMenu* contextMenu();

private:
	ImageViewerPlugin*						_imageViewerPlugin;			/** TODO */
	Renderer*								_renderer;					/** TODO */
	std::unique_ptr<QOpenGLDebugLogger>		_openglDebugLogger;			/** TODO */
};