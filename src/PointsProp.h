#pragma once

#include "Prop.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class Actor;

/**
 * Disk prop class
 * @author Thomas Kroes
 */
class PointsProp : public Prop
{
	Q_OBJECT

public:
	/**
	 * Polyline point class
	 * @author Thomas Kroes
	 */
	class Point
	{
	public:
		/** Default constructor */
		Point();

		/**
		 * Constructor
		 * @param position Point position in world coordinates
		 * @param radius Point radius
		 * @param color Point color
		 */
		Point(const QVector3D& position, const float& radius, const QVector4D& color);

		/** Attribute locations */
		static std::uint32_t	_positionAttribute;		/** Point position attribute location */
		static std::uint32_t	_radiusAttribute;		/** Point radius attribute location */
		static std::uint32_t	_colorAttribute;		/** Point color attribute location */

	private:
		QVector3D		_position;		/** Point position */
		float			_radius;		/** Point radius */
		QVector4D		_color;			/** Point color */
	};

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	PointsProp(Actor* actor, const QString& name);

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

	/** Returns whether the prop can be rendered */
	bool canRender() const override;

	/** Renders the prop */
	void render() override;

public:
	/** Set points */
	void setPoints(const QVector<Point>& points);

protected:
	QVector<Point>				_points;	/** Points */
	QOpenGLVertexArrayObject	_vao;		/** OpenGL Vertex Array Object (VAO) */
	QOpenGLBuffer				_vbo;		/** OpenGL Vertex Buffer Object (VBO) */
};