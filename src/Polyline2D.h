#pragma once

#include "Shape.h"

/**
 * Two-dimensional polyline point class
 * @author Thomas Kroes
 */
struct PolylinePoint2D
{
	QVector3D				_position;				/** Point position */
	QVector3D				_lineWidth;				/** Line width at point */
	QVector2D				_texture;				/** Texture coordinates at point (at the moment only the u component is used) */

	/** Attribute locations */
	static std::uint32_t	_positionAttribute;		/** Position attribute location */
	static std::uint32_t	_lineWidthAttribute;	/** Line width attribute location */
	static std::uint32_t	_textureAttribute;		/** Line width attribute location */
};

/**
 * Two-dimensional polyline class
 * @author Thomas Kroes
 */
class Polyline2D : public Shape
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param name Name of the polyline
	 * @param z Position along the z-axis
	 * @param closed Whether to close the polyline or not
	 * @param lineWidth Line width
	 * @param textureScale Scale of the texture in the U direction
	 */
	Polyline2D(Renderer* renderer, const QString& name, const float& z = 0.f, const bool& closed = true, const float& lineWidth = 1.f, const float& textureScale = 0.05f);

	/** Initialized the shape (must be called in appropriate OpenGL context) */
	void initialize() override;

	/** Renders the polyline */
	void render() override;

	/** Returns the line width in world space */
	float lineWidth() const;

	/**
	 * Sets the line width
	 * @param lineWidth Line width in world space
	 */
	void setLineWidth(const float& lineWidth);

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/** Adds the OpenGL vertex array objects that the shape needs */
	void addVAOs();

	/** Adds the OpenGL vertex buffer objects that the shape needs */
	void addVBOs();

	/**
	 * Set polyline points
	 * @param points Points in world coordinates
	 */
	void setPoints(QVector<QVector2D> points);

	/**
	 * Configure an OpenGL shader program (right after the shader program is bound in the render function)
	 * @param name Name of the OpenGL shader program
	 */
	void configureShaderProgram(const QString& name) override;

	/**
	 * Determines whether the polyline is textured
	 * @return Whether the polyline is textured
	 */
	bool isTextured() const;

signals:
	/**
	 * Signals that the line width changed
	 * @param lineWidth Line width in world space
	 */
	void lineWidthChanged(const float& lineWidth);

protected:
	bool					_closed;				/** Whether to close the polyline or not */
	float					_lineWidth;				/** Line width in world space */
	float					_textureScale;			/** Texture scale */
	std::uint32_t			_noPoints;				/** Number of points */

	static std::uint32_t	_vertexAttribute;		/** Quad vertex attribute location */
};