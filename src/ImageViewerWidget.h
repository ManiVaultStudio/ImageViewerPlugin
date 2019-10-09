#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <QMouseEvent>
#include <QColor>
#include <QAction>
#include <QMenu>

#include <memory>

#include "Image.h"
#include "Common.h"

class ImageViewerPlugin;

class ImageViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin);

public:
	InteractionMode interactionMode() const;
	void setInteractionMode(const InteractionMode& interactionMode);
	SelectionType selectionType() const;
	void setSelectionType(const SelectionType& selectionType);
	SelectionModifier selectionModifier() const;
	void setSelectionModifier(const SelectionModifier& selectionModifier);
	void setBrushRadius(const float& brushRadius);
	std::pair<double, double> windowLevel() const;
	void setWindowLevel(const double& window, const double& level);
	void resetWindowLevel();

	void modifySelection(const Indices& selectedPointIds, const std::int32_t& pixelOffset = 0);
	void clearSelection();

public:
	void onDisplayImageChanged(std::unique_ptr<Image<std::uint16_t>>& displayImage);
	void onSelectionImageChanged(std::unique_ptr<Image<std::uint8_t>>& selectionImage);
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onCurrentImageIdChanged(const std::int32_t& currentImageId);

private:
	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

private:
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE;

private:
	void drawCircle(const QPointF & center, const float & radius, const int & noSegments = 30);
	void drawSelectionRectangle(const QPoint& start, const QPoint& end);
	void drawSelectionBrush();
	void drawSelectionGeometry();

	void enableSelection(const bool& enable);

	void pan(const QPointF& delta);
	void zoom(const float& factor);
	void zoomAt(const QPointF & position, const float & factor);
	void zoomExtents();
	void resetView();

	bool initialized();
	void updateSelection();
	void commitSelection();

	QMenu* contextMenu();
	QMenu* viewMenu();
	QMenu* selectionMenu();

	QPoint screenToWorld(const QPoint& screen) const;
	QPoint worldToScreen(const QPoint& world) const;

private:
	void createImageQuad();
	void setupTextures();
	void setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter = QOpenGLTexture::Filter::Linear);

signals:
	void selectionTypeChanged();
	void selectionModifierChanged();
	void brushRadiusChanged();

private:
	ImageViewerPlugin*						_imageViewerPlugin;
	std::unique_ptr<Image<std::uint16_t>>	_displayImage;
	std::unique_ptr<Image<std::uint8_t>>	_selectionImage;
	TextureMap								_textures;
	ShaderMap								_shaders;
	InteractionMode							_interactionMode;
	QPoint									_initialMousePosition;
	QPoint									_mousePosition;
	QPointF									_pan;
	float									_zoom;
	float									_zoomSensitivity;
	int										_margin;
	bool									_selecting;
	SelectionType							_selectionType;
	SelectionModifier						_selectionModifier;
	bool									_selectionRealtime;
	float									_brushRadius;
	float									_brushRadiusDelta;
	QColor									_selectionColor;
	QColor									_selectionProxyColor;
	QColor									_selectionGeometryColor;
	Indices									_selectedPointIds;
	QAction*								_zoomToExtentsAction;
	QOpenGLShaderProgram					_imageShaderProgram;
	QOpenGLShaderProgram					_selectionShaderProgram;
	QOpenGLBuffer							_vertexBuffer;
	double									_window;
	double									_level;
	bool									_ignorePaintGL;
};
