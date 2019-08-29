#pragma once

#include <QOpenGLWidget>
#include <QOpenGLTexture>

#include <QMouseEvent>
#include <QColor>
#include <QAction>
#include <QMenu>

#include <memory>

#include "Common.h"

class ImageViewerPlugin;

class ImageViewerWidget : public QOpenGLWidget
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
	void setupTexture(QOpenGLTexture& texture);
	void setupTextures();
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
	void modifySelection(std::vector<unsigned int>& indices);
	void commitSelection();
	void resetTextureData(TextureData& textureData);
	void createActions();
	void createMenus();

	QMenu* contextMenu() const;
	
private:
	ImageViewerPlugin*			_imageViewerPlugin;
	TextureData					_imageTextureData;
	TextureData					_overlayTextureData;
	TextureData					_selectionTextureData;
	QOpenGLTexture				_imageTexture;
	QOpenGLTexture				_overlayTexture;
	QOpenGLTexture				_selectionTexture;
	QPoint						_initialMousePosition;
	QPoint						_mousePosition;
	QPointF						_pan;
	float						_zoom;
	float						_zoomSensitivity;
	int							_margin;
	bool						_selecting;
	SelectionType				_selectionType;
	SelectionModifier			_selectionModifier;
	bool						_selectionRealtime;
	float						_brushRadius;
	float						_brushRadiusDelta;
	QColor						_selectionColor;
	QColor						_selectionProxyColor;
	QColor						_selectionGeometryColor;
	std::vector<unsigned int>	_selection;
	QAction*					_zoomToExtentsAction;
	QAction*					_rectangleSelectionAction;
	QAction*					_brushSelectionAction;
	QAction*					_freehandSelectionAction;
	QAction*					_clearSelectionAction;
	QMenu*						_contextMenu;
	QMenu*						_selectionMenu;
};
