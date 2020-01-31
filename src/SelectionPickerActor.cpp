#include "SelectionPickerActor.h"
#include "SelectionImageQuad.h"

#include <QKeyEvent>
#include <QMenu>
#include <QDebug>

SelectionPickerActor::SelectionPickerActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_imageSize(),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_brushRadius(10.0f),
	_brushRadiusDelta(1.f)
{
	_registeredEvents = static_cast<int>(ActorEvent::MousePress) | static_cast<int>(ActorEvent::MouseRelease) | static_cast<int>(ActorEvent::MouseMove) | static_cast<int>(ActorEvent::MouseWheel);

//	connect(renderer(), &Renderer::brushRadiusChanged, this, &Brush::update);

//	addShape<SelectionImageQuad>("Quad");
}

QSize SelectionPickerActor::imageSize() const
{
	return _imageSize;
}

void SelectionPickerActor::setImageSize(const QSize& imageSize)
{
	if (imageSize == _imageSize)
		return;

	qDebug() << "Set image size";

	_imageSize = imageSize;

	emit imageSizeChanged(_imageSize);
}

SelectionType SelectionPickerActor::selectionType() const
{
	return _selectionType;
}

void SelectionPickerActor::setSelectionType(const SelectionType& selectionType)
{
	if (selectionType == _selectionType)
		return;

	/*
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			break;

		case SelectionType::Brush:
			actor<Brush>("Brush")->deactivate();
			actor<Brush>("Brush")->disable();
			break;

		case SelectionType::Lasso:
			break;

		case SelectionType::Polygon:
			break;

		default:
			break;
	}

	_selectionType = selectionType;

	qDebug() << "Set selection type to" << selectionTypeName(_selectionType);

	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			break;

		case SelectionType::Brush:
			shape<Brush>("Brush")->enable();
			shape<Brush>("Brush")->activate();
			break;

		case SelectionType::Lasso:
			break;

		case SelectionType::Polygon:
			break;

		default:
			break;
	}
	*/

	emit selectionTypeChanged(_selectionType);
}

SelectionModifier SelectionPickerActor::selectionModifier() const
{
	return _selectionModifier;
}

void SelectionPickerActor::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	if (selectionModifier == _selectionModifier)
		return;

	_selectionModifier = selectionModifier;

	qDebug() << "Set selection modifier to" << selectionModifierName(selectionModifier);

	emit selectionModifierChanged(_selectionModifier);
}

float SelectionPickerActor::brushRadius() const
{
	return _brushRadius;
}

void SelectionPickerActor::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 100000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius to" << QString::number(_brushRadius, 'f', 1);

	emit brushRadiusChanged(_brushRadius);
}

float SelectionPickerActor::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void SelectionPickerActor::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.1f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = boundBrushRadiusDelta;

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;

	emit brushRadiusDeltaChanged(_brushRadiusDelta);
}

void SelectionPickerActor::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void SelectionPickerActor::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
}

void SelectionPickerActor::onMousePressEvent(QMouseEvent* mouseEvent)
{

}

void SelectionPickerActor::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{

}

void SelectionPickerActor::onMouseMoveEvent(QMouseEvent* mouseEvent)
{

}

void SelectionPickerActor::onMouseWheelEvent(QWheelEvent* wheelEvent)
{
	/*
	if (selectionType() == SelectionType::Brush) {
	if (wheelEvent->delta() > 0) {
		brushSizeIncrease();
	}
	else {
		brushSizeDecrease();
	}
	}
	*/
}

void SelectionPickerActor::onKeyPressEvent(QKeyEvent* keyEvent)
{
	switch (keyEvent->key())
	{
		case Qt::Key::Key_R:
			setSelectionType(SelectionType::Rectangle);
			break;

		case Qt::Key::Key_B:
			setSelectionType(SelectionType::Brush);
			break;

		case Qt::Key::Key_L:
			setSelectionType(SelectionType::Lasso);
			break;

		case Qt::Key::Key_P:
			setSelectionType(SelectionType::Polygon);
			break;

		case Qt::Key::Key_Shift:
			setSelectionModifier(SelectionModifier::Add);
			break;

		case Qt::Key::Key_Control:
			setSelectionModifier(SelectionModifier::Remove);
			break;

		default:
			break;
	}
}

void SelectionPickerActor::onKeyReleaseEvent(QKeyEvent* keyEvent)
{
	switch (keyEvent->key())
	{
		case Qt::Key::Key_Shift:
		case Qt::Key::Key_Control:
		{
			setSelectionModifier(SelectionModifier::Replace);
			break;
		}

		default:
			break;
	}
}

QMenu* SelectionPickerActor::contextMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction		= new QAction("Rectangle");
	auto* brushSelectionAction			= new QAction("Brush");
	auto* lassoSelectionAction			= new QAction("Lasso", this);
	auto* polygonSelectionAction		= new QAction("Polygon", this);
	auto* selectNoneAction				= new QAction("Select none");
	auto* selectAllAction				= new QAction("Select all");
	auto* invertSelectionAction			= new QAction("Invert");

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Rectangle); });

	connect(brushSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Brush); });
	connect(lassoSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Lasso); });
	connect(polygonSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Polygon); });

	connect(selectAllAction, &QAction::triggered, this, &SelectionPickerActor::selectAll);
	connect(selectNoneAction, &QAction::triggered, this, &SelectionPickerActor::selectNone);
	connect(invertSelectionAction, &QAction::triggered, this, &SelectionPickerActor::invertSelection);

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);
	lassoSelectionAction->setCheckable(true);
	polygonSelectionAction->setCheckable(true);

	rectangleSelectionAction->setChecked(_selectionType == SelectionType::Rectangle);
	brushSelectionAction->setChecked(_selectionType == SelectionType::Brush);
	lassoSelectionAction->setChecked(_selectionType == SelectionType::Lasso);
	polygonSelectionAction->setChecked(_selectionType == SelectionType::Polygon);

	selectionMenu->addAction(rectangleSelectionAction);
	selectionMenu->addAction(brushSelectionAction);
	selectionMenu->addAction(lassoSelectionAction);
	selectionMenu->addAction(polygonSelectionAction);
	selectionMenu->addSeparator();
	selectionMenu->addAction(selectAllAction);
	selectionMenu->addAction(selectNoneAction);
	selectionMenu->addAction(invertSelectionAction);

	return selectionMenu;
}