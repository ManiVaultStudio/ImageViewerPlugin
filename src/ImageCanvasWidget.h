#pragma once

#include "Common.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class ImageCanvasWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit ImageCanvasWidget(QWidget *parent = 0);
	~ImageCanvasWidget();

	void setImage(std::vector<std::uint16_t>& image, const QSize& size, const double& imageMin, const double& imageMax);
	void setClearColor(const QColor &color);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;

public:
	

public:
	double window() const;
	double level() const;
	void setWindowLevel(const double& window, const double& level);
	void resetWindowLevel();

private:
	void makeObject();
	void setView(int w, int h);
	
	void computeWindowLevel(double& window, double& level);

public:
	InteractionMode interactionMode() const;
	void setInteractionMode(const InteractionMode& interactionMode);
	SelectionType selectionType() const;
	void setSelectionType(const SelectionType& selectionType);
	SelectionModifier selectionModifier() const;
	void setSelectionModifier(const SelectionModifier& selectionModifier);
	void setBrushRadius(const float& brushRadius);

private:
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE;

signals:
	void selectionTypeChanged();
	void selectionModifierChanged();
	void brushRadiusChanged();
	void windowLevelChanged(const float& window, const float& level);

private:
	QColor clearColour;

	QPoint lastPos;

	QOpenGLShaderProgram *program;
	QOpenGLBuffer vbo;

	QOpenGLTexture _imageTexture;
	bool hasTexture;
	float _aspectRatio;

	InteractionMode			_interactionMode;
	QPoint					_initialMousePosition;
	bool					_selecting;
	SelectionType			_selectionType;
	SelectionModifier		_selectionModifier;
	float					_brushRadius;
	float					_brushRadiusDelta;
	QPoint					_mousePosition;
	double					_window;
	double					_level;
	double					_imageMin;
	double					_imageMax;
};