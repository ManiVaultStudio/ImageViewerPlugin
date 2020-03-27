#pragma once

#include <QModelIndex>

template<typename T>
class TreeItem
{
public:
	/** TODO */
	explicit TreeItem(TreeItem<T>* parent = nullptr) :
		_children(),
		_parent(parent)
	{
	}

	/** TODO */
	~TreeItem()
	{
		qDeleteAll(_children);
	}

	/** TODO */
	void appendChild(T* child)
	{
		_children.append(child);
	}

	/** TODO */
	TreeItem<T>* child(const int& row)
	{
		if (row < 0 || row >= _children.size())
			return nullptr;

		return _children.at(row);
	}

	/** TODO */
	int childCount() const
	{
		return _children.count();
	}

	/** TODO */
	int row() const
	{
		if (_parent)
			return _parent->childIndex(const_cast<T*>(this));

		return 0;
	}

	/** TODO */
	TreeItem<T>* parent()
	{
		return _parent;
	}

	int childIndex() const
	{
		if (_parent)
			return _parent->_children.indexOf(const_cast<TreeItem*>(this));

		return 0;
	}

protected:
	QVector<TreeItem<T>*>		_children;		/** TODO */
	TreeItem<T>*				_parent;		/** TODO */
};