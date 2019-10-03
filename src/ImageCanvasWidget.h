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

	void setImage(std::vector<std::uint16_t>& image, const QSize& size);
	void setClearColor(const QColor &color);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void resizeGL(int width, int height) Q_DECL_OVERRIDE;

	void mouseMoveEvent(QMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

public:
	void onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage, const double& imageMin, const double& imageMax);

public:
	double window() const;
	double level() const;
	void setWindowLevel(const double& window, const double& level);
	void resetWindowLevel();

signals:
	void windowLevelChanged(const float& window, const float& level);

private:
	void makeObject();
	void setView(int w, int h);
	
	void computeWindowLevel(double& window, double& level);

private:
	QColor clearColour;

	QPoint lastPos;

	QOpenGLShaderProgram *program;
	QOpenGLBuffer vbo;

	QOpenGLTexture _imageTexture;
	bool hasTexture;
	float _aspectRatio;
	QPoint				_mousePosition;
	double				_window;
	double				_level;
	double				_imageMin;
	double				_imageMax;
};