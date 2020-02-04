#pragma once

#include "Prop.h"

class Actor;

/**
 * Selection lasso prop class
 * @author Thomas Kroes
 */
class SelectionLassoProp : public Prop
{
public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionLassoProp(Actor* actor, const QString& name);

protected:
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;
};