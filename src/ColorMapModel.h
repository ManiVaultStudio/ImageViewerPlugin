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
		ColorMap(const QString& name = "", const QString& resourcePath = "", const int& noDimensions = 1, const QImage& image = QImage()) :
			_name(name),
			_resourcePath(resourcePath),
			_noDimensions(noDimensions),
			_image(image)
		{
		}

		/** TODO */
		QString name() const { return _name; };

		/** TODO */
		QString resourcePath() const { return _resourcePath; };

		/** TODO */
		int noDimensions() const { return _noDimensions; };

		/** TODO */
		QImage image() const { return _image; };

	private:
		QString		_name;				/** TODO */
		QString		_resourcePath;		/** TODO */
		int			_noDimensions;		/** TODO */
		QImage		_image;				/** TODO */
	};

public:

	/** TODO */
	enum class Type {
		OneDimensional = 1,		/** TODO */
		TwoDimensional = 2		/** TODO */
	};

	/** TODO */
	enum class Column {
		Preview,			/** TODO */
		Name,				/** TODO */
		Image,				/** TODO */
		NoDimensions,		/** TODO */
		ResourcePath,		/** TODO */

		Start = Preview,
		End = ResourcePath
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

	//void add1dGradient(const QColor& begin, const QColor& end);


private:
	QVector<ColorMap>	_colorMaps;		/** TODO */
};