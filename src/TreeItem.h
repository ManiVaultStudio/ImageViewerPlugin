#pragma once

#include <QModelIndex>

template<typename T>
class TreeItem
{
public:
	/** TODO */
	explicit TreeItem(T* parent = nullptr) :
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
	T* child(const int& row)
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
			return _parent->childIndex(const_cast<Layer*>(this));

		return 0;
	}

	/** TODO */
	T* parent()
	{
		return _parent;
	}

	int childIndex(T* layer) const
	{
		_children.indexOf(layer);
	}

private:
	QVector<T*>		_children;		/** TODO */
	T *				_parent;		/** TODO */
};