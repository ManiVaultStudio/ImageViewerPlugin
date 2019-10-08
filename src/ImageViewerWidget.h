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

	/*
public:
	void onSelectionImageChanged(const QSize& imageSize, TextureData& selectionImage);
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onCurrentImageIdChanged(const std::int32_t& currentImageId);
	*/

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	

	/*
private:
	void drawQuad(const float& z);
	void drawCircle(const QPointF & center, const float & radius, const int & noSegments = 30);
	void drawSelectionRectangle(const QPoint& start, const QPoint& end);
	void drawSelectionBrush();
	void drawTextureQuad(QOpenGLTexture& texture, const float& z);
	void drawSelectionGeometry();
	
	void enableSelection(const bool& enable);

	bool imageInitialized();
	void updateSelection();
	void commitSelection();

	QPoint screenToWorld(const QPoint& screen) const;
	QPoint worldToScreen(const QPoint& world) const;
	*/

	/*
private:
	void setupTextures();
	void setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter = QOpenGLTexture::Filter::Linear);
	void resetTexture(const QString& textureName);
	*/

public:
	InteractionMode interactionMode() const;
	void setInteractionMode(const InteractionMode& interactionMode);
	SelectionType selectionType() const;
	void setSelectionType(const SelectionType& selectionType);
	SelectionModifier selectionModifier() const;
	void setSelectionModifier(const SelectionModifier& selectionModifier);
	float brushRadius() const;
	void setBrushRadius(const float& brushRadius);
	void zoomToExtents();

	double window() const;
	double level() const;
	void setWindowLevel(const double& window, const double& level);
	void resetWindowLevel();

private:
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE;

private:
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onDisplayImageChanged(std::vector<std::uint16_t>& displayImage, const QSize& imageSize, const double& imageMin, const double& imageMax);
	void onSelectionImageChanged(std::vector<std::uint8_t>& selectionImage, const QSize& imageSize);

signals:
	void selectionTypeChanged(const SelectionType& selectionType);
	void selectionModifierChanged(const SelectionModifier& selectionModifier);
	void brushRadiusChanged(const float& brushRadius);
	void windowLevelChanged(const float& window, const float& level);

private:
	ImageViewerPlugin*		_imageViewerPlugin;
	TextureMap				_textures;
	ShaderMap				_shaders;
	InteractionMode			_interactionMode;
	QPoint					_interactionStartMousePosition;
	QPoint					_lastMousePosition;
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
	float					_brushRadius;
	float					_brushRadiusDelta;
	bool					_selecting;
	SelectionType			_selectionType;
	SelectionModifier		_selectionModifier;
	double					_window;
	double					_level;
	QOpenGLShaderProgram*	_imageShaderProgram;
	QOpenGLShaderProgram*	_overlayShaderProgram;
	QOpenGLShaderProgram*	_selectionShaderProgram;
};
