#pragma once

#include "Layer.h"

class QPainter;

/**
 * Root layer class
 *
 * All layer hierarchies consist of a root layer
 *
 * @author Thomas Kroes
 */
class RootLayer : public Layer
{
public:
	
	/** TODO */
	enum class Column {
		Start = ult(Layer::Column::End) + 1,
		End = Start
	};

public:

	/** TODO */
	RootLayer();

	/** TODO */
	void render(const QMatrix4x4& parentMVP);

	/**
	 * Paints the layer
	 * @param painter Pointer to painter
	 */
	void paint(QPainter* painter) override;

public: // Inherited MVC

	/** TODO */
	int columnCount() const override { return ult(Column::End) + 1; }

	/** TODO */
	Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, const int& role) const override;

	/** TODO */
	QModelIndexList setData(const QModelIndex& index, const QVariant& value, const int& role) override;

protected:

	/**
	* Returns the image size
	* @return Image size in variant form
	*/
	QSize imageSize() const override;
};