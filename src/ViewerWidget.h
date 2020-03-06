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

class DatasetsModel;

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
	ViewerWidget(QWidget* parent, DatasetsModel* datasetsModel);

	/** Destructor */
	~ViewerWidget() override;

public:

	/** TODO */
	//void setDataset(ImageDataset* dataset);

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
	/** Returns the context menu */
	QMenu* contextMenu();

private:
	Renderer*								_renderer;					/** TODO */
	DatasetsModel*							_datasetsModel;				/** TODO */
	std::unique_ptr<QOpenGLDebugLogger>		_openglDebugLogger;			/** TODO */
};