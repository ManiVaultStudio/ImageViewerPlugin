#pragma once

#include "Prop.h"

class Actor;

/**
 * Polyline prop class
 * @author Thomas Kroes
 */
class PolylineProp : public Prop
{
	Q_OBJECT

public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	PolylineProp(Actor* actor, const QString& name);

public:
	/** Returns whether the prop can be rendered */
	bool canRender() const override;

	/** Returns the polyline points */
	QVector<QVector3D> points() const;

	/** Set polyline points */
	void setPoints(const QVector<QVector3D>& points);

	/** Returns whether the polyline is closed */
	bool closed();

	/** Sets whether the polyline is closed
	 * @param closed Closed
	 */
	void setClosed(const bool& closed);

	/** Returns the line width */
	float lineWidth();

	/** Sets the line width
	 * @param lineWidth Line width
	 */
	void setLineWidth(const float& lineWidth);

	/** Returns the line color */
	QColor lineColor();

	/** Sets the line color
	 * @param lineColor Line color
	 */
	void setLineColor(const QColor& lineColor);

protected:
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;

	/** Update shapes */
	void updateShapes() override;

	/** Update textures */
	void updateTextures() override;

protected:
	QVector<QVector3D>		_points;		/** Polyline points */
	bool					_closed;		/** Whether to close the polyline */
	float					_lineWidth;		/** Line width */
	QColor					_lineColor;		/** Line color */
};