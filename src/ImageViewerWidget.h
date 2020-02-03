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

/**
 * Image viewer widget class
 * This widget displays high-dimensional image data
 * @author Thomas Kroes
 */
class ImageViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to image viewer plugin
	 */
	ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~ImageViewerWidget() override;

public:
	/** Publish selection to HDPS */
	void publishSelection();

	/** Select all */
	void onSelectAll();

	/** Select none */
	void onSelectNone();

	/** Invert the selection */
	void onInvertSelection();

	/** Returns the renderer */
	QSharedPointer<Renderer> renderer();

public:
	/**
	 * Invoked when the renderer becomes dirty */
	void onRendererDirty();
	
private:
	/** Initializes OpenGL */
	void initializeGL() Q_DECL_OVERRIDE;

	/** Invoked when the OpenGL window changes size */
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;

	/** Paints the OpenGL content */
	void paintGL() Q_DECL_OVERRIDE;

private:
	/**
	 * Invoked when a key is pressed
	 * @param keyEvent Key event
	 */
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when a key is released
	 * @param keyEvent Key event
	 */
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

	/**
	 * Invoked when the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE;
	
private:
	/** Returns the context menu */
	QMenu* contextMenu();

private:
	ImageViewerPlugin*						_imageViewerPlugin;			/** Pointer to image viewer plugin */
	QPoint									_initialMousePosition;		/** Initial mouse position */
	QSharedPointer<Renderer>				_renderer;					/** Selection renderer */
	std::unique_ptr <QOpenGLDebugLogger>	_openglDebugLogger;			/** OpenGL debug logger (on in debug mode) */
};