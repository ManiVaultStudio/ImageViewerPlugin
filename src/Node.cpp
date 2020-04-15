#include "Node.h"
#include "Prop.h"

#include <QDebug>
#include <QFont>

Node::Node(const QString& id, const QString& name, const int& flags) :
	_id(id),
	_name(name),
	_flags(flags),
	_children(),
	_parent(nullptr)
{
}

Node::~Node()
{
	qDeleteAll(_children);
}

Node* Node::child(const int& index)
{
	if (index < 0 || index >= _children.size())
		return nullptr;

	return _children.at(index);
}

int Node::childCount() const
{
	return _children.count();
}

bool Node::insertChild(const int& position, Node* node)
{
	if (position < 0 || position > _children.size())
		return false;

	node->setParent(this);

	_children.insert(position, node);

	return true;
}

Node* Node::parent()
{
	return _parent;
}

void Node::setParent(Node* parent)
{
	_parent = parent;
}

bool Node::removeChild(const int& position, const bool& purge /*= true*/)
{
	if (position < 0 || position > _children.size())
		return false;

	if (purge)
		delete _children.at(position);

	_children.takeAt(position);

	return true;
}

int Node::childIndex() const
{
	if (_parent)
		return _parent->_children.indexOf(const_cast<Node*>(this));

	return 0;
}

bool Node::hasChildren() const
{
	return childCount() > 0;
}

bool Node::isRoot() const
{
	return _parent == nullptr;
}

bool Node::isLeaf() const
{
	return !hasChildren();
}

Node* Node::rootItem()
{
	if (parent() == nullptr)
		return this;

	return _parent->rootItem();
}

void Node::render()
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

QVariant Node::id(const int& role) const
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

void Node::setId(const QString& id)
{
	_id = id;
}

QVariant Node::name(const int& role) const
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

void Node::setName(const QString& name)
{
	_name = name;
}

bool Node::isFlagSet(const Flag& flag) const
{
	return _flags & static_cast<int>(flag);
}

QVariant Node::flag(const Flag& flag, const int& role) const
{
	const auto flagString = isFlagSet(flag) ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return flagString;

		case Qt::EditRole:
			return isFlagSet(flag);

		case Qt::ToolTipRole:
		{
			switch (flag)
			{
				case Flag::Enabled:
					return QString("Enabled: %1").arg(flagString);

				case Flag::Renamable:
					return QString("Renamable: %1").arg(flagString);

				case Flag::Renderable:
					return QString("Renderable: %1").arg(flagString);

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

void Node::setFlag(const Flag& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= static_cast<int>(flag);
	else
		_flags = _flags & ~static_cast<int>(flag);

	if (hasChildren()) {
		for (auto treeItem : _children) {
			auto layer = static_cast<Node*>(treeItem);

			layer->setFlag(flag, enabled);
		}
	}
}

QVariant Node::flags(const int& role) const
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

void Node::setFlags(const int& flags)
{
	_flags = flags;
}

bool Node::isEnabled() const
{
	return isFlagSet(Flag::Enabled);
}

bool Node::isRenderable() const
{
	return isFlagSet(Flag::Renderable);
}

Qt::CheckState Node::aggregatedCheckState() const
{
	if (isLeaf())
		return flag(Flag::Enabled, Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;

	QSet<int> states;

	for (auto treeItem : _children) {
		auto layer = static_cast<Node*>(treeItem);

		states.insert(layer->aggregatedCheckState());
	}

	if (states.count() > 1)
		return Qt::PartiallyChecked;

	return static_cast<Qt::CheckState>(*states.begin());
}