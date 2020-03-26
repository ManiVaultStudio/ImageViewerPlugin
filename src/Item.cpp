#include "Item.h"

Item::Item(Item *parent) :
	_childItems(),
	_parentItem(parent)
{
}

Item::~Item()
{
	qDeleteAll(_childItems);
}

void Item::appendChild(Item *item)
{
	_childItems.append(item);
}

Item *Item::child(int row)
{
	if (row < 0 || row >= _childItems.size())
		return nullptr;

	return _childItems.at(row);
}

int Item::childCount() const
{
	return _childItems.count();
}

int Item::row() const
{
	if (_parentItem)
		return _parentItem->_childItems.indexOf(const_cast<Item*>(this));

	return 0;
}

Item *Item::parentItem()
{
	return _parentItem;
}