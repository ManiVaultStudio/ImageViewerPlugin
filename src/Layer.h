#pragma once

#include <QImage>

/** TODO */
class Layer : public QObject
{
public:

	/** TODO */
	enum Type : int {
		Image,			/** TODO */
		Selection,		/** TODO */
		Metadata		/** TODO */
	};

	/** TODO */
	enum Flags : int {
		Enabled		= 1ul << 0,		/** TODO */
		Fixed		= 1ul << 1		/** TODO */
	};

	/** TODO */
	Layer(QObject* parent);

	/** TODO */
	Layer(QObject* parent,const QString& name, const Type& type, const std::uint32_t& flags, const std::uint32_t& order, const float& opacity = 1.0f, const float& window = 1.0f, const float& level = 0.5f);

public: // Flags

	/** TODO */
	bool isFlagSet(const std::uint32_t& flag) const;

	/** TODO */
	void setFlag(const std::uint32_t& flag, const bool& enabled = true);

public:
	QString					_name;					/** TODO */
	Type					_type;					/** TODO */
	std::uint32_t			_flags;					/** TODO */
	std::uint32_t			_order;					/** TODO */
	float					_opacity;				/** TODO */
	QImage					_image;					/** TODO */
	QPair<float, float>		_imageRange;			/** TODO */
	QPair<float, float>		_displayRange;			/** TODO */
	float					_windowNormalized;		/** TODO */
	float					_levelNormalized;		/** TODO */
	float					_window;				/** TODO */
	float					_level;					/** TODO */
	QColor					_color;					/** TODO */

	friend class ImageDataset;
};

using Layers = QList<Layer*>;