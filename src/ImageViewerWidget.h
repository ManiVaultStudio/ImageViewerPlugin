#pragma once

#include "renderers/PointRenderer.h"
#include "renderers/DensityRenderer.h"
#include "renderers/SelectionRenderer.h"

#include "SelectionListener.h"

#include "graphics/BufferObject.h"
#include "graphics/Vector2f.h"
#include "graphics/Vector3f.h"
#include "graphics/Matrix3f.h"
#include "graphics/Selection.h"
#include "graphics/Shader.h"

#include "widgets/ColormapWidget.h"

#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QOpenGLFunctions_3_3_Core>

#include <QMouseEvent>
#include <memory>

using namespace hdps;
using namespace hdps::gui;

class ImageViewerPlugin;

class ImageViewerWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
	Q_OBJECT

public:
	ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin);

	enum SelectionType
	{
		Rectangle,
		Brush,
		Freehand
	};

public:
	void onDisplayImageIdsChanged();
	void onSelectedPointsChanged();

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

private:
	void setupTextures(const QSize& imageSize);
	void drawQuad(const float& z);
	void drawSelectionRectangle(const QPoint& start, const QPoint& end);
	void drawTextureQuad(QOpenGLTexture& texture, const float& z);
	void pan(const QPointF& delta);
	void zoom(const float& factor);
	void zoomAt(const QPointF & position, const float & factor);
	void zoomExtents();
	void resetView();
	bool imageInitialized() const;
	QPoint screenToWorld(const QPoint& screen) const;
	QPoint worldToScreen(const QPoint& world) const;
	void updateSelection();

private:
	ImageViewerPlugin*	_imageViewerPlugin;
	QOpenGLTexture		_texture;
	QOpenGLTexture		_selectionOverlayTexture;
	QPoint				_initialMousePosition;
	QPoint				_mousePosition;
	QPointF				_pan;
	float				_zoom;
	float				_zoomSensitivity;
	int					_margin;
	bool				_selecting;
	SelectionType		_selectionType;
	bool				_selectionRealtime;
};
