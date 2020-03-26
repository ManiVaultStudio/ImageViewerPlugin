#pragma once

#include "Common.h"

#include <QList>
#include <QString>
#include <QImage>
#include <QModelIndex>

class Item
{
public:
	/** TODO */
	explicit Item(Item *parentItem = nullptr);

	/** TODO */
	~Item();

	/** TODO */
	void appendChild(Item *child);

	/** TODO */
	Item *child(int row);

	/** TODO */
	int childCount() const;

	/** TODO */
	virtual int columnCount() const = 0;

	virtual QVariant headerData(const int& section, const Qt::Orientation& orientation, const int& role) const = 0;

	/** TODO */
	virtual Qt::ItemFlags flags(const int& column) const = 0;

	/** TODO */
	virtual QVariant data(const int& column, const int& role) const = 0;

	/** TODO */
	virtual void setData(const int& column, const QVariant& value, const int& role) = 0;

	/** TODO */
	int row() const;

	/** TODO */
	Item *parentItem();

private:
	QVector<Item*>	_childItems;		/** TODO */
	Item *			_parentItem;		/** TODO */
};