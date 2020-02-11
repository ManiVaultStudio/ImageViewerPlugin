#include "CoordinateSelection.h"
#include "Renderer.h"

#include <QDebug>

CoordinateSelection::CoordinateSelection(Renderer* renderer) :
	_renderer(renderer),
	_selectionType(SelectionType::None),
	_selectionModifier(SelectionModifier::None),
	_indices()
{
}

CoordinateSelection::~CoordinateSelection() = default;

SelectionType CoordinateSelection::selectionType() const
{
	return _selectionType;
}

void CoordinateSelection::setSelectionType(const SelectionType& selectionType)
{
	if (selectionType == _selectionType)
		return;

	_selectionType = selectionType;

	qDebug() << "Set pixel selection type to" << selectionTypeName(_selectionType);

	emit selectionTypeChanged(_selectionType);
}

SelectionModifier CoordinateSelection::selectionModifier() const
{
	return _selectionModifier;
}

void CoordinateSelection::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	if (selectionModifier == _selectionModifier)
		return;

	_selectionModifier = selectionModifier;

	qDebug() << "Set pixel selection modifier to" << selectionModifierName(selectionModifier);

	emit selectionModifierChanged(_selectionModifier);
}

void CoordinateSelection::abortSelection()
{
	setSelectionType(SelectionType::None);
	
	emit selectionAborted();
}

void CoordinateSelection::setIndices(const QVector<std::uint32_t>& indices)
{
	if (indices == indices)
		return;

	_indices = indices;

	qDebug() << "Set" << QString::number(indices.size()) << "indices";

	emit indicesChanged(_indices);
}

/*
void CoordinateSelection::selectAll()
{
	qDebug() << "Select all";
}

void CoordinateSelection::selectNone()
{
	qDebug() << "Selection clear";

	setIndices(QVector<std::uint32_t>());
}

void CoordinateSelection::selectInvert()
{
	qDebug() << "Selection invert";
}
*/

void CoordinateSelection::onKeyPress(QKeyEvent* keyEvent)
{

}

void CoordinateSelection::onKeyRelease(QKeyEvent* keyEvent)
{

}

void CoordinateSelection::onMousePress(QMouseEvent* mouseEvent)
{

}

void CoordinateSelection::onMouseRelease(QMouseEvent* mouseEvent)
{

}

void CoordinateSelection::onMouseMove(QMouseEvent* mouseEvent)
{

}

void CoordinateSelection::onMouseWheel(QWheelEvent* wheelEvent)
{

}