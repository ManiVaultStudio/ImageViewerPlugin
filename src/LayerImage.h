#pragma once

#include <QImage>

/** TODO */
class LayerImage : public QObject
{
public:

	/** TODO */
	class Range
	{
	public:

		/** TODO */
		Range(const float& min = 0.0f, const float& max = 0.0f) :
			_min(std::min(min, max)),
			_max(std::max(min, max))
		{
		}

		/** TODO */
		float min() const { return _min; }

		/** TODO */
		void setMin(const float& min) { _min = std::min(min, _max); }

		/** TODO */
		float max() const { return _max; }

		/** TODO */
		void setMax(const float& max) { _max = std::max(_min, max); }

		/** TODO */
		void include(const float& value) {
			_min = std::min(_min, value);
			_max = std::max(_max, value);
		}

		/** TODO */
		void setFullRange() {
			_max = std::numeric_limits<float>::min();
			_min = std::numeric_limits<float>::max();
		}

		/** TODO */
		float length() const { return _max - _min; }

	private:
		float	_min;	/** TODO */
		float	_max;	/** TODO */
	};

	/** TODO */
	LayerImage(QObject* parent = nullptr, const float& window = 1.0f, const float& level = 0.5f);

	/** TODO */
	QImage image() const;

	/** TODO */
	void setImage(const QImage& image);

	/** TODO */
	Range imageRange() const;

	/** TODO */
	Range displayRange() const;

	/** TODO */
	float windowNormalized() const;

	/** TODO */
	void setWindowNormalized(const float& windowNormalized);

	/** TODO */
	float levelNormalized() const;

	/** TODO */
	void setLevelNormalized(const float& levelNormalized);

	/** TODO */
	float window() const;

	/** TODO */
	void setWindow(const float& window);

	/** TODO */
	float level() const;

	/** TODO */
	void setLevel(const float& level);

private:

	/** TODO */
	void computeImageRange();

	/** TODO */
	void computeDisplayRange();

public:
	QImage		_image;					/** TODO */
	Range		_imageRange;			/** TODO */
	Range		_displayRange;			/** TODO */
	float		_windowNormalized;		/** TODO */
	float		_levelNormalized;		/** TODO */
	float		_window;				/** TODO */
	float		_level;					/** TODO */
};

Q_DECLARE_METATYPE(LayerImage::Range);