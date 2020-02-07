#pragma once

#include "Shape.h"

#include <QVector>
#include <QVector2D>
#include <QVector3D>

/**
 * Two-dimensional polyline shape class
 * @author Thomas Kroes
 */
class DiskShape : public Shape
{
	Q_OBJECT

public:
	/** Constructor
	 * @param prop Parent prop
	 * @param name Name of the shape
	 */
	DiskShape(Prop* prop, const QString& name);

public: // Inherited

	/** Returns if the shape can be rendered */
	bool canRender() const override;

	/** Renders the shape */
	void render() override;

	/** Updates the shape */
	void set(const QVector3D& center, const float& radius);

public:
	/** Returns the disk center */
	QVector2D center() const;

	/** Set disk center */
	void setCenter(const QVector2D& center);

	/** Returns the disk radius */
	float radius();

	/** Sets the disk radius
	 * @param radius Disk radius
	 */
	void setRadius(const float& radius);
	
protected:
	QVector<QVector3D>		_vertices;	/** Radius of the disk */
};