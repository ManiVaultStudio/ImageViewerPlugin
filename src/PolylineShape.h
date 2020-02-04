#pragma once

#include "Shape.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>

/**
 * Two-dimensional polyline shape class
 * @author Thomas Kroes
 */
class PolylineShape : public Shape
{
	Q_OBJECT

public:
	/**
	 * Polyline point class
	 * @author Thomas Kroes
	 */
	struct Point
	{
		/** Default constructor */
		Point();

		/**
		 * Constructor
		 * @param position Position
		 * @param textureCoordinate Texture coordinate at polyline point
		 * @param lineWidth Line width at polyline point
		 */
		Point(const QVector3D& position, const QVector2D& textureCoordinate, const float& lineWidth);

		/** Attributes */
		QVector3D				_position;						/** Point position */
		QVector2D				_textureCoordinate;				/** Texture coordinates at point (at the moment only the U component is used) */
		float					_lineWidth;						/** Line width at point */

		/** Attribute locations */
		static std::uint32_t	_positionAttribute;				/** Position attribute location */
		static std::uint32_t	_textureCoordinateAttribute;	/** Line width attribute location */
		static std::uint32_t	_lineWidthAttribute;			/** Line width attribute location */
	};

public:
	/** Constructor
	 * @param prop Parent prop
	 * @param name Name of the shape
	 */
	PolylineShape(Prop* prop, const QString& name);

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
	void setPoints(QVector<Point> points = QVector<Point>());

protected:
	bool				_closed;				/** Whether to close the polyline or not */
	float				_textureScale;			/** Texture scale */
	QVector<Point>		_points;				/** Points */
};