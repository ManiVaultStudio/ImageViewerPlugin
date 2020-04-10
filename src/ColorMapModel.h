#pragma once

#include "ColorMap.h"

#include <QAbstractListModel>

/**
 * Color map model class
 *
 * Provides a central place where color maps are stored
 * This model can contain 1D and 2D color maps (perhaps in the future also 3D)
 *
 * @author Thomas Kroes
 */
class ColorMapModel : public QAbstractListModel
{
public:

	/**
	 * Color map model columns
	 * Defines the color map model columns
	 */
	enum class Column {
		Preview,					/** Preview */
		Name,						/** Name column */
		Image,						/** Image column */
		NoDimensions,				/** Dimensionality */
		ResourcePath,				/** Resource path */

		Start	= Preview,			/** Column start */
		End		= ResourcePath		/** Column End */
	};

public:
	/** Constructor */
	ColorMapModel(QObject* parent, const ColorMap::Type& type);

	/**
	 * Returns the the number of model columns
	 * @param parent Parent index
	 */
	int columnCount(const QModelIndex& parent) const override;

	/**
	 * Returns the number of color maps in the model
	 * @param parent Parent index
	 */
	int rowCount(const QModelIndex& parent /* = QModelIndex() */) const override;

	/**
	 * Returns model data for the given index
	 * @param index Index
	 * @param role The data role
	 */
	QVariant data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const override;

	/** Setups the model data (e.g. loads color maps from resources) */
	void setupModelData();

	/**
	 * Returns a color map given a row index
	 * @param row Row index
	 */
	const ColorMap* colorMap(const int& row) const;

private:
	QVector<ColorMap>	_colorMaps;		/** Color maps data */
};