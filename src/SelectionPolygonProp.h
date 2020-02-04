#pragma once

#include "Prop.h"

class Actor;

/**
 * Selection polygon prop class
 * @author Thomas Kroes
 */
class SelectionPolygonProp : public Prop
{
public:
	/** Constructor
	 * @param actor Parent actor
	 * @param name Name of the prop
	 */
	SelectionPolygonProp(Actor* actor, const QString& name);

protected:
	/** Initializes the prop */
	void initialize() override;

	/** Renders the prop */
	void render() override;
};