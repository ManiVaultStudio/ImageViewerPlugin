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
#include <QColor>
#include <QAction>
#include <QMenu>

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

	enum SelectionModifier
	{
		Replace,
		Add,
		Remove
	};

	SelectionType selectionType() const;
	void setSelectionType(const SelectionType& selectionType);
	SelectionModifier selectionModifier() const;
	void setSelectionModifier(const SelectionModifier& selectionModifier);
	void setBrushRadius(const float& brushRadius);

signals:
	void selectionTypeChanged();
	void selectionModifierChanged();
	void brushRadiusChanged();

public:
	void onDisplayImageIdsChanged();
	void onSelectedPointsChanged();
	void onCurrentDataSetNameChanged();

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
	void drawCircle(const QPointF & center, const float & radius, const int & noSegments = 30);
	void drawSelectionRectangle(const QPoint& start, const QPoint& end);
	void drawSelectionBrush();
	void drawTextureQuad(QOpenGLTexture& texture, const float& z);
	void drawSelectionGeometry();
	void pan(const QPointF& delta);
	void zoom(const float& factor);
	void zoomAt(const QPointF & position, const float & factor);
	void zoomExtents();
	void resetView();
	bool imageInitialized() const;
	QPoint screenToWorld(const QPoint& screen) const;
	QPoint worldToScreen(const QPoint& world) const;
	void updateSelection();
	void select(std::vector<unsigned int>& indices);
	
	void createActions();
	void createMenus();

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
	SelectionModifier	_selectionModifier;
	bool				_selectionRealtime;
	float				_brushRadius;
	float				_brushRadiusDelta;
	QColor				_selectionColor;
	QColor				_selectionGeometryColor;

	QAction*			_zoomToExtentsAction;
	QAction*			_rectangleSelectionAction;
	QAction*			_brushSelectionAction;
	QAction*			_freehandSelectionAction;
	QAction*			_clearSelectionAction;
	QMenu*				_contextMenu;
	QMenu*				_selectionMenu;
};
