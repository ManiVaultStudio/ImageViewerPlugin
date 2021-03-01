#include "ColorMapComboBox.h"

#include <QDebug>
#include <QListView>
#include <QHeaderView>

ColorMapComboBox::ColorMapComboBox(QWidget* parent, const ColorMap::Type& type /*= ColorMapModel::Type::OneDimensional*/) :
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

ColorMap::Type ColorMapComboBox::getType() const
{
    return _filteredColorMapModel.type();
}

void ColorMapComboBox::setType(const ColorMap::Type& type)
{
    _filteredColorMapModel.setType(type);

    switch (_filteredColorMapModel.type())
    {
        case ColorMap::Type::OneDimensional:
            setIconSize(QSize(60, 12));
            break;

        case ColorMap::Type::ZeroDimensional:
        case ColorMap::Type::TwoDimensional:
            setIconSize(QSize(12, 12));
            break;

        default:
            break;
    }
}

QImage ColorMapComboBox::getCurrentImage() const
{
    return _filteredColorMapModel.data(_filteredColorMapModel.index(currentIndex(), ult(ColorMapModel::Column::Image)), Qt::EditRole).value<QImage>();
}