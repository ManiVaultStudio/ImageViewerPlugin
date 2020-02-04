#pragma once

#include "Prop.h"

class Actor;

/**
 * Selection brush prop class
 * @author Thomas Kroes
 */
class SelectionBrushProp : public Prop
{
public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionBrushProp(Actor* actor, const QString& name);

protected:
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;
};