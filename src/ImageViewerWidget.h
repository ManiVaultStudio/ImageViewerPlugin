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
	enum RenderMode {
		SCATTERPLOT, DENSITY, LANDSCAPE
	};

	ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin);

public:
	void onDisplayImageIdsChanged();

protected:
	void initializeGL()         Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL()              Q_DECL_OVERRIDE;

	void mousePressEvent(QMouseEvent *event)   Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event)    Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	void setupTexture(const QSize& imageSize);

private:
	ImageViewerPlugin*	_imageViewerPlugin;
	QOpenGLTexture		_texture;
};
