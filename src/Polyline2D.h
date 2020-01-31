#pragma once

#include "Shape.h"

#include <QVector2D>
#include <QVector3D>

/**
 * Two-dimensional polyline point class
 * @author Thomas Kroes
 */
struct PolylinePoint2D
{
	PolylinePoint2D();
	PolylinePoint2D(const QVector3D& position, const QVector2D& textureCoordinate, const float& lineWidth);

	/** Attributes */
	QVector3D				_position;						/** Point position */
	QVector2D				_textureCoordinate;				/** Texture coordinates at point (at the moment only the U component is used) */
	float					_lineWidth;						/** Line width at point */

	/** Attribute locations */
	static std::uint32_t	_positionAttribute;				/** Position attribute location */
	static std::uint32_t	_textureCoordinateAttribute;	/** Line width attribute location */
	static std::uint32_t	_lineWidthAttribute;			/** Line width attribute location */
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
	Polyline2D(Actor* actor, const QString& name, const float& z = 0.f, const bool& closed = true, const float& lineWidth = 1.f, const float& textureScale = 0.05f);

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

	/** Returns whether the shape can be rendered */
	bool canRender() const override;

	/**
	 * Set polyline points
	 * @param points Points in world coordinates
	 */
	void setPoints(QVector<PolylinePoint2D> points);

	/** Resets the polyline */
	virtual void reset();

protected:
	/** Adds the OpenGL shader programs that the shape needs */
	void addShaderPrograms();

	/** Adds the OpenGL vertex array objects that the shape needs */
	void addVAOs();

	/** Adds the OpenGL vertex buffer objects that the shape needs */
	void addVBOs();

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
	bool						_closed;				/** Whether to close the polyline or not */
	float						_lineWidth;				/** Line width in world space */
	float						_textureScale;			/** Texture scale */
	//std::uint32_t				_noPoints;				/** Number of points */
	QVector<PolylinePoint2D>	_points;				/** Points */
};