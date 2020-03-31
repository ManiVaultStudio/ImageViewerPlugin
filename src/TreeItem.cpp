#include "TreeItem.h"

#include <QDebug>

Layer::Layer(const QVector<QVariant>& data, Layer* parent)
	: itemData(data),
	parentItem(parent)
{
	/*
	_children(),
	_parent(nullptr),
	_dataset(dataset),
	_id(id),
	_name(name),
	_type(type),
	_flags(flags),
	_order(0),
	_opacity(1.0f),
	_colorMap(),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(),
	_level()
	*/
}

Layer::~Layer()
{
	qDeleteAll(childItems);
}

Layer *Layer::child(int number)
{
	if (number < 0 || number >= childItems.size())
		return nullptr;
	return childItems.at(number);
}

int Layer::childCount() const
{
	return childItems.count();
}

int Layer::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<Layer*>(this));
	return 0;
}

int Layer::columnCount() const
{
	return itemData.count();
}

QVariant Layer::data(int column) const
{
	if (column < 0 || column >= itemData.size())
		return QVariant();
	return itemData.at(column);
}

bool Layer::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		Layer *item = new Layer(data, this);
		childItems.insert(position, item);
	}

	return true;
}

bool Layer::insertChild(const int& position, Layer* layer)
{
	if (position < 0 || position > childItems.size())
		return false;

	layer->setParent(this);

	childItems.insert(position, layer);

	return true;
}

bool Layer::insertColumns(int position, int columns)
{
	if (position < 0 || position > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.insert(position, QVariant());

	for (Layer *child : qAsConst(childItems))
		child->insertColumns(position, columns);

	return true;
}

Layer *Layer::parent()
{
	return parentItem;
}

void Layer::setParent(Layer* parent)
{
	parentItem = parent;
}

bool Layer::removeChild(const int& position, const bool& purge /*= true*/)
{
	if (position < 0 || position > childItems.size())
		return false;

	if (purge)
		delete childItems.at(position);

	childItems.takeAt(position);

	return true;
}

bool Layer::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.remove(position);

	for (Layer *child : qAsConst(childItems))
		child->removeColumns(position, columns);

	return true;
}

bool Layer::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= itemData.size())
		return false;

	itemData[column] = value;
	return true;
}