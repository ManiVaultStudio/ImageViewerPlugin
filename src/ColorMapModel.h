#pragma once

#include <QAbstractListModel>
#include <QImage>

/** TODO */
class ColorMapModel : public QAbstractListModel
{
	using ColorMap = QPair<QString, QImage>;

public:
	enum class Type {
		OneDimensional,		/** TODO */
		TwoDimensional,		/** TODO */
	};

public:
	/** TODO */
	ColorMapModel(QObject* parent, const Type& type = Type::OneDimensional);

	/** TODO */
	int rowCount(const QModelIndex &parent /* = QModelIndex() */) const override;

	/** TODO */
	void setupModelData();

private:
	Type					_type;			/** TODO */
	QVector<ColorMap>		_colorMaps;		/** TODO */
};