#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

#include <QMouseEvent>
#include <QColor>
#include <QAction>
#include <QMenu>

#include <memory>

#include "Common.h"

class ImageViewerPlugin;

class ImageViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin);

	

	void modifySelection(const Indices& selectedPointIds, const std::int32_t& pixelOffset = 0);
	void clearSelection();



public:
	void onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage);
	void onSelectionImageChanged(const QSize& imageSize, TextureData& selectionImage);
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onCurrentImageIdChanged(const std::int32_t& currentImageId);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

	

private:
	void drawQuad(const float& z);
	void drawCircle(const QPointF & center, const float & radius, const int & noSegments = 30);
	void drawSelectionRectangle(const QPoint& start, const QPoint& end);
	void drawSelectionBrush();
	void drawTextureQuad(QOpenGLTexture& texture, const float& z);
	void drawSelectionGeometry();
	void drawInfo(QPainter* painter);
	
	void enableSelection(const bool& enable);

	bool imageInitialized();
	void updateSelection();
	void commitSelection();

	QPoint screenToWorld(const QPoint& screen) const;
	QPoint worldToScreen(const QPoint& world) const;


private:
	void setupTextures();
	void setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter = QOpenGLTexture::Filter::Linear);
	void resetTexture(const QString& textureName);

private:
	ImageViewerPlugin*		_imageViewerPlugin;
	TextureMap				_textures;
	ShaderMap				_shaders;
	
	QPoint					_mousePosition;
	QPointF					_pan;
	float					_zoom;
	float					_zoomSensitivity;
	int						_margin;
	bool					_selectionRealtime;
	QColor					_selectionColor;
	QColor					_selectionProxyColor;
	QColor					_selectionGeometryColor;
	Indices					_selectedPointIds;
	QAction*				_zoomToExtentsAction;
	QSize					_imageSize;
	QOpenGLBuffer			_vertexBuffer;
};
