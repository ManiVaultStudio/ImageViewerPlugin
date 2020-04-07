#pragma once

#include "ColorMapModel.h"

#include <QComboBox>
#include <QSortFilterProxyModel>

class QListView;

/** TODO */
class ColorMapComboBox : public QComboBox
{
protected:
	class ColorMapFilterModel : public QSortFilterProxyModel {
	public:
		/** TODO */
		ColorMapFilterModel::ColorMapFilterModel(QObject *parent, const ColorMapModel::Type& type = ColorMapModel::Type::OneDimensional) :
			QSortFilterProxyModel(parent),
			_type(type)
		{
		}

		/** TODO */
		bool filterAcceptsRow(int row, const QModelIndex& parent) const
		{
			const auto index = sourceModel()->index(row, ult(ColorMapModel::Column::NoDimensions));
			return ult(_type) == sourceModel()->data(index, Qt::EditRole).toInt();
		}

		/** TODO */
		ColorMapModel::Type type() const {
			return _type;
		}

		/** TODO */
		void setType(const ColorMapModel::Type& type) {
			_type = type;
			invalidateFilter();
		}

	private:
		ColorMapModel::Type		_type;		/** TODO */
	};
public:
	/** TODO */
	ColorMapComboBox(QWidget* parent, const ColorMapModel::Type& type = ColorMapModel::Type::OneDimensional);

	/** TODO */
	void setModel(ColorMapModel* colorMapModel);

	/** TODO */
	ColorMapModel::Type type() const;

	/** TODO */
	void setType(const ColorMapModel::Type& type);

private:
	ColorMapFilterModel		_filteredColorMapModel;		/** TODO */
	QListView*				_view;						/** TODO */
};