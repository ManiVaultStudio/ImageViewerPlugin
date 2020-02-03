#pragma once

#include "Shape.h"

#include <QVector>
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
 * Two-dimensional polyline shape class
 * @author Thomas Kroes
 */
class Polyline2D : public Shape
{
	Q_OBJECT

public:
	/** Constructor
	 * @param prop Parent prop
	 * @param name Name of the shape
	 */
	Polyline2D(Prop* prop, const QString& name);

	/** Initialized the shape (must be called in appropriate OpenGL context) */
	void initialize() override;

	/** Returns the line width in world space */
	float lineWidth() const;

	/**
	 * Sets the line width
	 * @param lineWidth Line width in world space
	 */
	void setLineWidth(const float& lineWidth);

	/**
	 * Set polyline points
	 * @param points Points in world coordinates
	 */
	void setPoints(QVector<PolylinePoint2D> points = QVector<PolylinePoint2D>());

	/** Resets the polyline */
	virtual void reset();

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
	QVector<PolylinePoint2D>	_points;				/** Points */
};