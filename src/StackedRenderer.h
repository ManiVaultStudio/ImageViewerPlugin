#pragma once

#include <memory>

#include "Common.h"

#include "renderers/Renderer.h"

#include <QMatrix4x4>
#include <QMap>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>

/**
 * Stacked renderer class
 * This renderer class is used to render content in layers using OpenGL
 * @author Thomas Kroes
 */
class StackedRenderer : public QObject, public hdps::Renderer
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param depth Depth (layer) to render content at
	 */
	StackedRenderer(const float& depth);
	
public:
	/** Initializes the renderer */
	void init() override;

	/** Return whether the renderer is initialized */
	virtual bool isInitialized() const = 0;

public:
	/** Return the drawing depth */
	float depth() const;

	/**
	 * Set drawing depth
	 * @param depth Depth (layer) at which to draw
	 */
	void setDepth(const float& depth);

protected:
	float														_depth;					/** Depth (layer) at which to display the rendered content */
	QMap<QString, QSharedPointer<QOpenGLFramebufferObject>>		_fbos;					/** OpenGL frame buffer object map */
};