#include "ColorMapComboBox.h"

#include <QDebug>
#include <QListView>
#include <QHeaderView>

ColorMapComboBox::ColorMapComboBox(QWidget* parent, const ColorMapModel::Type& type /*= ColorMapModel::Type::OneDimensional*/) :
	QComboBox(parent),
	_filteredColorMapModel(this, type),
	_view(new QListView(this))
{
	setView(_view);

	//_view->setIndentation(0);
	
	switch (_filteredColorMapModel.type())
	{
		case ColorMapModel::Type::OneDimensional:
			setIconSize(QSize(100, 10));
			break;

		case ColorMapModel::Type::TwoDimensional:
			setIconSize(QSize(10, 10));
			break;

		default:
			break;
	}
	
	/*
	auto headerView = _view->header();

	headerView->setHidden(true);
	headerView->hideSection(ult(ColorMapModel::Column::Name));
	headerView->hideSection(ult(ColorMapModel::Column::Image));
	*/
}

void ColorMapComboBox::setModel(ColorMapModel* colorMapModel)
{
	_filteredColorMapModel.setSourceModel(colorMapModel);

	QComboBox::setModel(&_filteredColorMapModel);
}