#include "TreeItem.h"

TreeItem::TreeItem(TreeItem *parent) :
	_childItems(),
	_parentItem(parent)
{
}

TreeItem::~TreeItem()
{
	qDeleteAll(_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
	_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
	if (row < 0 || row >= _childItems.size())
		return nullptr;

	return _childItems.at(row);
}

int TreeItem::childCount() const
{
	return _childItems.count();
}

int TreeItem::row() const
{
	if (_parentItem)
		return _parentItem->_childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

TreeItem *TreeItem::parentItem()
{
	return _parentItem;
}