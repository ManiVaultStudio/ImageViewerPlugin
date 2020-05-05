#pragma once

#include "LayerNode.h"

/** TODO */
class RootLayer : public LayerNode
{
public:
	
	/** TODO */
	enum class Column {
		Start = ult(LayerNode::Column::End) + 1,
		End = Start
	};

public:

	/** TODO */
	RootLayer();

	/** TODO */
	void render(const QMatrix4x4& parentMVP);

public: // Inherited MVC

	/** TODO */
	int columnCount() const override { return ult(Column::End) + 1; }

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

public: // Mouse and keyboard event handlers

	/**
	 * Invoked when a mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePressEvent(QMouseEvent* mouseEvent) override;

	/**
	* Invoked when a mouse button is released
	* @param mouseEvent Mouse event
	*/
	void mouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/**
	* Invoked when the mouse pointer is moved
	* @param mouseEvent Mouse event
	*/
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;

	/**
	* Invoked when the mouse wheel is rotated
	* @param wheelEvent Mouse wheel event
	*/
	void mouseWheelEvent(QWheelEvent* wheelEvent, const QModelIndex& index) override;

	/**
	* Invoked when a key is pressed
	* @param keyEvent Key event
	*/
	void keyPressEvent(QKeyEvent* keyEvent, const QModelIndex& index) override;

	/**
	* Invoked when a key is released
	* @param keyEvent Key event
	*/
	void keyReleaseEvent(QKeyEvent* keyEvent, const QModelIndex& index) override;

protected:

	/**
	* Returns the image size
	* @return Image size in variant form
	*/
	QSize imageSize() const override;
};