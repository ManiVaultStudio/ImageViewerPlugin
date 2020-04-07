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
}

void ColorMapComboBox::setModel(ColorMapModel* colorMapModel)
{
	_filteredColorMapModel.setSourceModel(colorMapModel);

	QComboBox::setModel(&_filteredColorMapModel);
}

ColorMapModel::Type ColorMapComboBox::type() const
{
	return _filteredColorMapModel.type();
}

void ColorMapComboBox::setType(const ColorMapModel::Type& type)
{
	_filteredColorMapModel.setType(type);

	switch (_filteredColorMapModel.type())
	{
		case ColorMapModel::Type::OneDimensional:
			setIconSize(QSize(65, 13));
			break;

		case ColorMapModel::Type::TwoDimensional:
			setIconSize(QSize(13, 13));
			break;

		default:
			break;
	}
}