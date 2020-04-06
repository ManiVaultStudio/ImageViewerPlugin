#pragma once

#include "Common.h"

#include <QAbstractListModel>
#include <QImage>

/** TODO */
class ColorMapModel : public QAbstractListModel
{
private:

	/** TODO */
	class ColorMap {
	public:
		/** TODO */
		ColorMap(const QString& name = "", const int& noDimensions = 1, const QImage& image = QImage()) :
			_name(name),
			_noDimensions(noDimensions),
			_image(image)
		{
		}

		/** TODO */
		QString name() const { return _name; };

		/** TODO */
		int noDimensions() const { return _noDimensions; };

		/** TODO */
		QImage image() const { return _image; };

	private:
		QString		_name;				/** TODO */
		int			_noDimensions;		/** TODO */
		QImage		_image;				/** TODO */
	};

public:

	/** TODO */
	enum class Type {
		OneDimensional,		/** TODO */
		TwoDimensional		/** TODO */
	};

	/** TODO */
	enum class Column {
		Preview,	/** TODO */
		Name,		/** TODO */
		Image,		/** TODO */

		Start = Preview,
		End = Image
	};

public:
	/** TODO */
	ColorMapModel(QObject* parent, const Type& type);

	/** TODO */
	int columnCount(const QModelIndex& parent) const override;

	/** TODO */
	int rowCount(const QModelIndex& parent /* = QModelIndex() */) const override;

	/** TODO */
	QVariant data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const override;

	/** TODO */
	void setupModelData();

private:
	QVector<ColorMap>	_colorMaps;		/** TODO */
};