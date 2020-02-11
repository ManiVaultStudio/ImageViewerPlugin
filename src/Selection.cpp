#include "Selection.h"
#include "Renderer.h"

#include <QDebug>

Selection::Selection(Renderer* renderer) :
	_renderer(renderer),
	_selectionType(SelectionType::None),
	_selectionModifier(SelectionModifier::None),
	_indices()
{
}

Selection::~Selection() = default;

SelectionType Selection::selectionType() const
{
	return _selectionType;
}

void Selection::setSelectionType(const SelectionType& selectionType)
{
	if (selectionType == _selectionType)
		return;

	_selectionType = selectionType;

	qDebug() << "Set pixel selection type to" << selectionTypeName(_selectionType);

	emit selectionTypeChanged(_selectionType);
}

SelectionModifier Selection::selectionModifier() const
{
	return _selectionModifier;
}

void Selection::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	if (selectionModifier == _selectionModifier)
		return;

	_selectionModifier = selectionModifier;

	qDebug() << "Set pixel selection modifier to" << selectionModifierName(selectionModifier);

	emit selectionModifierChanged(_selectionModifier);
}

void Selection::abortSelection()
{
	setSelectionType(SelectionType::None);
	
	emit selectionAborted();
}

void Selection::setIndices(const QVector<std::uint32_t>& indices)
{
	if (indices == indices)
		return;

	_indices = indices;

	qDebug() << "Set" << QString::number(indices.size()) << "indices";

	emit indicesChanged(_indices);
}

void Selection::selectAll()
{
	qDebug() << "Select all";
}

void Selection::selectNone()
{
	qDebug() << "Selection clear";

	setIndices(QVector<std::uint32_t>());
}

void Selection::selectInvert()
{
	qDebug() << "Selection invert";
}

void Selection::onKeyPress(QKeyEvent* keyEvent)
{

}

void Selection::onKeyRelease(QKeyEvent* keyEvent)
{

}

void Selection::onMousePress(QMouseEvent* mouseEvent)
{

}

void Selection::onMouseRelease(QMouseEvent* mouseEvent)
{

}

void Selection::onMouseMove(QMouseEvent* mouseEvent)
{

}

void Selection::onMouseWheel(QWheelEvent* wheelEvent)
{

}