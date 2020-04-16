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
		Point(const QVector3D& position, const float& radius, const QColor& color);

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
	 * @param node Node
	 * @param name Name of the prop
	 */
	PointsProp(Node* node, const QString& name);

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

	/** Returns whether the prop can be rendered */
	bool canRender() const override;

	/** Renders the prop */
	void render(const QMatrix4x4& nodeMVP, const float& opacity) override;

public:
	/** Set points */
	void setPoints(const QVector<Point>& points);

	/** Returns the number of segments */
	std::uint32_t noSegments() const;

	/**
	 * Sets the number of segments
	 * @param noSegments Number of segments
	 */
	void setNoSegments(const std::uint32_t& noSegments);

protected:
	QVector<Point>				_points;		/** Points */
	QOpenGLVertexArrayObject	_vao;			/** OpenGL Vertex Array Object (VAO) */
	QOpenGLBuffer				_vbo;			/** OpenGL Vertex Buffer Object (VBO) */
	std::uint32_t				_noSegments;	/** Number of segments */
};