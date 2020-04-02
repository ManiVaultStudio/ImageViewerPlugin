#pragma once

#include "ImageRange.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>

class Dataset;

class TreeItem
{
public:
	explicit TreeItem(TreeItem* parent = nullptr);
	virtual ~TreeItem();

public: // Tree item

	/** TODO */
	TreeItem* child(const int& index);

	/** TODO */
	int childCount() const;

	/** TODO */
	bool insertChild(const int& position, TreeItem* layer);

	/** TODO */
	TreeItem* parent();

	/** TODO */
	void setParent(TreeItem* parent);

	/** TODO */
	bool removeChild(const int& position, const bool& purge = true);

	/** TODO */
	int childIndex() const;

	/** TODO */
	bool hasChildren() const;

	/** TODO */
	bool isLeaf() const;

protected:
	QVector<TreeItem*>		_children;		/** TODO */
	TreeItem*				_parent;		/** TODO */
};