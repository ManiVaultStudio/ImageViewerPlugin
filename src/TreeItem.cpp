#include "TreeItem.h"
#include "Dataset.h"
#include "Prop.h"
#include "LayerActor.h"

#include <QDebug>
#include <QFont>

TreeItem::TreeItem(const QString& id, const QString& name, const int& flags) :
	_id(id),
	_name(name),
	_flags(flags),
	_children(),
	_parent(nullptr)
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

bool TreeItem::insertChild(const int& position, TreeItem* treeItem)
{
	if (position < 0 || position > _children.size())
		return false;

	treeItem->setParent(this);

	_children.insert(position, treeItem);

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

bool TreeItem::isRoot() const
{
	return _parent == nullptr;
}

bool TreeItem::isLeaf() const
{
	return !hasChildren();
}

TreeItem* TreeItem::rootItem()
{
	if (parent() == nullptr)
		return this;

	return _parent->rootItem();
}

void TreeItem::render()
{
	if (!_flags & ult(Flag::Enabled))
		return;

	qDebug() << "Render" << _id << _name;

	for (auto child : _children)
		child->render();

	/*
	if (_actor != nullptr)
		_actor->render();
	*/
}

QVariant TreeItem::id(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _id;

		case Qt::ToolTipRole:
			return QString("ID: %1").arg(_id);

		default:
			break;
	}

	return QVariant();
}

void TreeItem::setId(const QString& id)
{
	_id = id;
}

QVariant TreeItem::name(const int& role) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _name;

		case Qt::ToolTipRole:
			return QString("Name: %1").arg(_name);

		case Qt::CheckStateRole:
			return aggregatedCheckState();

		default:
			break;
	}

	return QVariant();
}

void TreeItem::setName(const QString& name)
{
	_name = name;
}

QVariant TreeItem::flag(const Flag& flag, const int& role) const
{
	const auto isFlagSet = _flags & static_cast<int>(flag);
	const auto flagString = isFlagSet ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return flagString;

		case Qt::EditRole:
			return isFlagSet;

		case Qt::ToolTipRole:
		{
			switch (flag)
			{
				case Flag::Enabled:
					return QString("Enabled: %1").arg(flagString);

				case Flag::Frozen:
					return QString("Frozen: %1").arg(flagString);

				case Flag::Removable:
					return QString("Removable: %1").arg(flagString);

				case Flag::Mask:
					return QString("Mask: %1").arg(flagString);

				case Flag::Renamable:
					return QString("Renamable: %1").arg(flagString);

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QVariant();
}

void TreeItem::setFlag(const Flag& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= static_cast<int>(flag);
	else
		_flags = _flags & ~static_cast<int>(flag);

	if (hasChildren()) {
		for (auto treeItem : _children) {
			auto layer = static_cast<TreeItem*>(treeItem);

			layer->setFlag(flag, enabled);
		}
	}
}

QVariant TreeItem::flags(const int& role) const
{
	const auto flagsString = QString("%1%").arg(QString::number(_flags));

	switch (role)
	{
		case Qt::DisplayRole:
			return flagsString;

		case Qt::EditRole:
			return _flags;

		case Qt::ToolTipRole:
			return QString("Flags: %1").arg(flagsString);

		default:
			break;
	}

	return QVariant();
}

void TreeItem::setFlags(const int& flags)
{
	_flags = flags;
}

Qt::CheckState TreeItem::aggregatedCheckState() const
{
	if (isLeaf())
		return flag(Flag::Enabled, Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;

	QSet<int> states;

	for (auto treeItem : _children) {
		auto layer = static_cast<TreeItem*>(treeItem);

		states.insert(layer->aggregatedCheckState());
	}

	if (states.count() > 1)
		return Qt::PartiallyChecked;

	return static_cast<Qt::CheckState>(*states.begin());
}