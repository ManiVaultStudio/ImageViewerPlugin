#pragma once

#include "ImageRange.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>

class Dataset;
class Actor;

/**
 * Tree item class
 *
 * Tree item for storing hierarchical data
 *
 * @author Thomas Kroes
 */
class TreeItem : public QObject
{
public:
	/** TODO */
	enum class Flag {
		Enabled			= 0x01,		/** TODO */
		Frozen			= 0x02,		/** TODO */
		Removable		= 0x04,		/** TODO */
		Mask			= 0x08,		/** TODO */
		Renamable		= 0x20,		/** TODO */
		Renderable		= 0x40		/** TODO */
	};

	/** (Default) constructor */
	explicit TreeItem(const QString& id, const QString& name, const int& flags);

	/** Destructor */
	virtual ~TreeItem();

public:

	/**
	 * Returns a child item by index
	 * @param index Index of the child item
	 * @return Child at index
	 */
	TreeItem* child(const int& index);

	/** Returns the number of children */
	int childCount() const;

	/**
	 * Inserts a child at a position
	 * @param position Position to insert at
	 * @param treeItem Tree item to insert
	 * @return Whether the child was inserted
	 */
	bool insertChild(const int& position, TreeItem* treeItem);

	/** Returns the parent tree item (if it exists, else returns nullptr) */
	TreeItem* parent();

	/**
	 * Sets the parent of the tree item
	 * @param parent Parent tree item
	 */
	void setParent(TreeItem* parent);

	/**
	 * Removes a child item at position
	 * @param position Position of the child item
	 * @param purge Physically remove the child as well
	 * @return Whether the child was removed properly
	 */
	bool removeChild(const int& position, const bool& purge = true);

	/** Returns the child index w.r.t. its parent */
	int childIndex() const;

	/** Returns whether the tree item has children or not */
	bool hasChildren() const;

	/** Returns whether the tree item is the root item or not */
	bool isRoot() const;

	/** Returns whether the tree item is a leaf item or not */
	bool isLeaf() const;

	/** Returns the root item */
	TreeItem* rootItem();

	/** Returns the associated actor */
	Actor* actor();

	/** Render the prop and its children */
	virtual void render();

public: // Getters/setters

	/**
	 * Returns the tree item identifier
	 * @param role Data role
	 * @return Identifier
	 */
	QVariant id(const int& role) const;

	/**
	 * Sets the tree item identifier
	 * @param id Identifier
	 */
	void setId(const QString& id);

	/**
	 * Returns the tree item name
	 * @param role Data role
	 */
	QVariant name(const int& role) const;

	/**
	 * Sets the tree item name
	 * @param name Name
	 */
	void setName(const QString& name);

	/**
	 * Returns whether specified configuration flag is set or not
	 * @param flag Configuration flag
	 * @param role Data role
	 * @return Whether the configuration flag is set or not
	 */
	QVariant flag(const Flag& flag, const int& role) const;

	/**
	 * Sets the tree item configuration flag
	 * @param flag Configuration flag
	 * @param enabled Whether to enable/disable the configuration flag
	 */
	void setFlag(const Flag& flag, const bool& enabled = true);

	/**
	 * Returns the configuration flags
	 * @param role Data role
	 * @return Configuration flags
	 */
	QVariant flags(const int& role) const;

	/**
	 * Sets the tree item configuration flags
	 * @param flags Configuration flags
	 */
	void setFlags(const int& flags);

	/** Returns the aggregated check state of the children of the tree item */
	Qt::CheckState aggregatedCheckState() const;

protected:
	QString					_id;			/** Identifier (internal use) */
	QString					_name;			/** Name (GUI) */
	std::uint32_t			_flags;			/** Configuration flags */
	QVector<TreeItem*>		_children;		/** Child tree items */
	TreeItem*				_parent;		/** Parent tree item */
//	Actor*					_actor;			/** Actor for rendering the layer on the screen */
};