#include "TreeItem.h"
#include "Dataset.h"

#include <QDebug>
#include <QFont>

TreeItem::TreeItem(TreeItem* parent /*= nullptr*/) :
	_children(),
	_parent(parent)
{
}

TreeItem::~TreeItem()
{
	qDeleteAll(_children);
}

TreeItem* TreeItem::child(const int& index)
{
	if (index < 0 || index >= _children.size())
		return nullptr;

	return _children.at(index);
}

int TreeItem::childCount() const
{
	return _children.count();
}

bool TreeItem::insertChild(const int& position, TreeItem* layer)
{
	if (position < 0 || position > _children.size())
		return false;

	layer->setParent(this);

	_children.insert(position, layer);

	return true;
}

TreeItem* TreeItem::parent()
{
	return _parent;
}

void TreeItem::setParent(TreeItem* parent)
{
	_parent = parent;
}

bool TreeItem::removeChild(const int& position, const bool& purge /*= true*/)
{
	if (position < 0 || position > _children.size())
		return false;

	if (purge)
		delete _children.at(position);

	_children.takeAt(position);

	return true;
}

int TreeItem::childIndex() const
{
	if (_parent)
		return _parent->_children.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

bool TreeItem::hasChildren() const
{
	return childCount() > 0;
}

bool TreeItem::isLeaf() const
{
	return !hasChildren();
}