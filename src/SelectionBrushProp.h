#pragma once

#include "PolylineProp.h"

#include <QPoint>

class Actor;

/**
 * Selection brush prop class
 * @author Thomas Kroes
 */
class SelectionBrushProp : public PolylineProp
{
public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionBrushProp(Actor* actor, const QString& name);

public:
	/** Sets the brush center
	 * @param brushCenter Brush center
	 */
	void setBrushCenter(const QPoint& brushCenter);

	/** Sets the brush radius
	 * @param brushRadius Brush radius
	 */
	void setBrushRadius(const float& brushRadius);

protected:
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;

	/** Update shapes */
	void updateShapes() override;

	/** Update textures */
	void updateTextures() override;

private:
	QPoint		_brushCenter;		/** Brush in screen space */
	float		_brushRadius;		/** Brush radius */
};