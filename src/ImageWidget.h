#pragma once
/*
#include "Common.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QMatrix4x4>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class ImageWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit ImageWidget(QWidget *parent = 0);
	~ImageWidget();

	void setDisplayImage(std::vector<std::uint16_t>& image, const QSize& size, const double& imageMin, const double& imageMax);
	void setSelectionImage(std::vector<std::uint8_t>& selectionImage, const QSize& size);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;

public:
	double window() const;
	double level() const;
	void setWindowLevel(const double& window, const double& level);
	void resetWindowLevel();

	void update();

private:
	void makeObject();
	void setView(int w, int h);
	
	void computeWindowLevel(double& window, double& level);

public:
	SelectionType selectionType() const;
	void setSelectionType(const SelectionType& selectionType);
	SelectionModifier selectionModifier() const;
	void setSelectionModifier(const SelectionModifier& selectionModifier);
	void setBrushRadius(const float& brushRadius);

private:
	void mousePressEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	//void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* wheelEvent) Q_DECL_OVERRIDE;

signals:
	void selectionTypeChanged();
	void selectionModifierChanged();
	void rendered();

private:
	QPoint					_initialMousePosition;
	QPoint					_lastMousePosition;
	QOpenGLShaderProgram*	_imageShaderProgram;
	QOpenGLShaderProgram*	_overlayShaderProgram;
	QOpenGLShaderProgram*	_selectionShaderProgram;
	QOpenGLBuffer			_vbo;
	QOpenGLTexture			_imageTexture;
	QOpenGLTexture			_overlayTexture;
	QOpenGLTexture			_selectionTexture;
	float					_aspectRatio;
	bool					_selecting;
	SelectionType			_selectionType;
	SelectionModifier		_selectionModifier;
	float					_brushRadius;
	float					_brushRadiusDelta;
	double					_window;
	double					_level;
	double					_imageMin;
	double					_imageMax;
};
*/