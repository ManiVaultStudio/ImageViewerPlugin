#pragma once

#include "Prop.h"

class Actor;
class DiskShape;

/**
 * Disk prop class
 * @author Thomas Kroes
 */
class DiskProp : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	DiskProp(Actor* actor, const QString& name);

protected: // Inherited

	/** Initializes the prop */
	void initialize() override;

	/** Returns whether the prop can be rendered */
	bool canRender() const override;

	/** Renders the prop */
	void render() override;

	/** Update shapes */
	void updateShapes() override;

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

	/** Returns the disk color */
	QColor color();

	/** Sets the disk color
	 * @param color Disk color
	 */
	void color(const QColor& color);

protected:
	QVector2D		_center;	/** Center of the disk */
	float			_radius;	/** Radius of the disk */
	QColor			_color;		/** Disk color */
};