#include "ColorMapComboBox.h"
#include "ColorMapModel.h"

#include <QDebug>
#include <QListView>

ColorMapComboBox::ColorMapComboBox(QWidget* parent) :
	QComboBox(parent),
	_colorMapModel(nullptr),
	_view(new QListView(this))
{
	setView(_view);
}

void ColorMapComboBox::setModel(ColorMapModel* colorMapModel)
{
	_view->setModel(colorMapModel);	
}