#pragma once

#include "Renderable.h"
#include "Range.h"

#include <QColor>
#include <QObject>
#include <QImage>
#include <QModelIndex>

class Dataset;
class Actor;

/**
 * Node class
 *
 * Node item for storing hierarchical data
 *
 * @author Thomas Kroes
 */
class Node : public QObject, public Renderable
{
public:
	/**
	 * Node flags
	 * Configuration bit flags for a node
	 */
	enum class Flag {
		Enabled			= 0x01,		/** Whether the node is enabled */
		Renamable		= 0x20,		/** Whether the node can be renamed */
		Renderable		= 0x40		/** Whether the node is renderable */
	};

	/**
	 * Constructor
	 * @param id Identifier for internal use
	 * @param name Name in the GUI
	 * @param flags Configuration bit flags
	 */
	explicit Node(const QString& id, const QString& name, const int& flags);

	/** Destructor */
	virtual ~Node();

public: // Hierarchy

	/**
	 * Returns a child node by index
	 * @param index Index of the child node
	 * @return Child at index
	 */
	Node* child(const int& index);

	/** Returns the number of children */
	int childCount() const;

	/**
	 * Inserts a child at a position
	 * @param position Position to insert at
	 * @param node Node to insert
	 * @return Whether the child was inserted
	 */
	bool insertChild(const int& position, Node* node);

	/** Returns the parent node (if it exists, else returns nullptr) */
	Node* parent();

	/**
	 * Sets the parent node
	 * @param parent Parent node
	 */
	void setParent(Node* parent);

	/**
	 * Removes a child node at position
	 * @param position Position of the child node
	 * @param purge Physically remove the child as well
	 * @return Whether the child was removed properly
	 */
	bool removeChild(const int& position, const bool& purge = true);

	/** Returns the child index w.r.t. its parent */
	int childIndex() const;

	/** Returns whether the node has children or not */
	bool hasChildren() const;

	/** Returns whether the node is the root node or not */
	bool isRoot() const;

	/** Returns whether the node is a leaf node or not */
	bool isLeaf() const;

	/** Returns the root node */
	Node* rootItem();

public: // Rendering

	/** Renders the node */
	void render(const QMatrix4x4& parentMVP) override;

	/** Computes the enveloping bounding rectangle of the node and its descendants */
	QRectF boundingRectangle() const;

public: // Getters/setters

	/**
	 * Returns the node identifier
	 * @param role Data role
	 * @return Identifier
	 */
	QVariant id(const int& role) const;

	/**
	 * Sets the node identifier
	 * @param id Identifier
	 */
	void setId(const QString& id);

	/**
	 * Returns the node name
	 * @param role Data role
	 */
	QVariant name(const int& role) const;

	/**
	 * Sets the node name
	 * @param name Name
	 */
	void setName(const QString& name);

	/**
	 * Returns whether specified configuration flag is set or not
	 * @param flag Configuration flag
	 * @return Whether the configuration flag is set or not
	 */
	bool isFlagSet(const Flag& flag) const;

	/**
	 * Returns whether specified configuration flag is set or not
	 * @param flag Configuration flag
	 * @param role Data role
	 * @return Whether the configuration flag is set or not
	 */
	QVariant flag(const Flag& flag, const int& role) const;

	/**
	 * Sets the node configuration flag
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
	 * Sets the node configuration flags
	 * @param flags Configuration flags
	 */
	void setFlags(const int& flags);

	/** Returns whether the node is enabled */
	bool isEnabled() const;

	/** Returns whether the node is renderable */
	bool isRenderable() const;

	/** Returns the aggregated check state of the children of the node */
	Qt::CheckState aggregatedCheckState() const;

protected:
	QString				_id;			/** Identifier (internal use) */
	QString				_name;			/** Name (GUI) */
	std::uint32_t		_flags;			/** Configuration flags */
	QVector<Node*>		_children;		/** Child tree items */
	Node*				_parent;		/** Parent node */
};