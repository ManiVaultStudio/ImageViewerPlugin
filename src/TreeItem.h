#pragma once

#include <QList>
#include <QString>
#include <QImage>
#include <QModelIndex>

class TreeItem
{
public:
	/** TODO */
	explicit TreeItem(TreeItem *parentItem = nullptr);

	/** TODO */
	~TreeItem();

	/** TODO */
	void appendChild(TreeItem *child);

	/** TODO */
	TreeItem *child(int row);

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
	TreeItem *parentItem();

private:
	QVector<TreeItem*>	_childItems;		/** TODO */
	TreeItem *			_parentItem;		/** TODO */
};