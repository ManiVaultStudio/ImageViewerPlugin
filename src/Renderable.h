#pragma once

#include <QObject>
#include <QColor>
#include <QVector2D>
#include <QMatrix4x4>
#include <QMap>
#include <QSharedPointer>

#include "Prop.h"

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

/**
 * Render node class
 *
 * Contains props for rendering in OpenGL
 *
 * @author Thomas Kroes
 */
class Renderable
{
public:

	/**
	 * Mouse point class
	 * @author Thomas Kroes
	 */
	class MouseEvent
	{
	public:
		/** Default constructor */
		MouseEvent() {};

		/**
		 * Constructor
		 * @param screenPoint Point in screen coordinates [0..width, 0..height]
		 * @param worldPosition Position in world space
		 */
		MouseEvent(const QVector2D& screenPoint, const QVector3D& worldPosition = QVector3D()) :
			_screenPoint(screenPoint),
			_worldPosition(worldPosition)
		{
		}

		QVector2D screenPoint() const {
			return _screenPoint;
		}

		QVector3D worldPosition() const {
			return _worldPosition;
		}

	private:
		QVector2D		_screenPoint;		/** Point in screen coordinates */
		QVector3D		_worldPosition;		/** World position */
	};

public:
	/** Default constructor */
	Renderable();

	/** Destructor */
	~Renderable();

	/** Renders the node */
	virtual void render(const QMatrix4x4& parentMVP) = 0;

public: // Matrix functions

	/** Returns the model matrix */
	QMatrix4x4 modelMatrix() const;

	/**
	 * Sets the model matrix
	 * @param modelMatrix Model matrix
	 */
	void setModelMatrix(const QMatrix4x4& modelMatrix);

	/** Returns the model-view matrix (viewMatrix * actorModelMatrix) */
	QMatrix4x4 modelViewMatrix() const;

	/** Returns the model-view-projection matrix (projectionMatrix * viewMatrix * actorModelMatrix) */
	QMatrix4x4 modelViewProjectionMatrix() const;

public: // Opacity

	/** Returns the render opacity */
	QVariant opacity(const int& role) const;

	/** Sets the render opacity
	 * @param opacity Render opacity
	*/
	void setOpacity(const float& opacity);

	/** Returns the scale */
	QVariant scale(const int& role) const;

	/** Sets the scale
	 * @param scale Scale
	*/
	void setScale(const float& scale);

protected: // Prop management

	/** TODO */
	template<typename T, typename ...Args>
	void addProp(Args... args)
	{
		try {
			auto sharedProp = QSharedPointer<T>::create(args...);
			auto prop = dynamic_cast<Prop*>(sharedProp.get());
			auto propName = prop->name();

			if (_props.contains(propName))
				throw std::exception(QString("%1 already exists").arg(propName).toLatin1());

			_props.insert(propName, sharedProp);
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to add prop: %1").arg(e.what()).toLatin1());
		}
	}

	/** TODO */
	void removeProp(const QString& name)
	{
		try {
			if (!_props.contains(name))
				throw std::exception(QString("%1 does not exist").arg(name).toLatin1());

			_props.remove(name);
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to remove prop: %1").arg(e.what()).toLatin1());
		}
	}

	/** TODO */
	template<typename T>
	const T* propByName(const QString& name) const
	{
		try {
			if (!_props.contains(name))
				throw std::exception(QString("no prop named %1").arg(name).toLatin1());

			return dynamic_cast<T*>(_props[name].get());
		}
		catch (const std::exception& e)
		{
			throw std::exception(QString("Unable to retrieve prop: %1").arg(e.what()).toLatin1());
		}
	}

	/** TODO */
	template<typename T>
	T* propByName(const QString& name)
	{
		const auto constThis = const_cast<const Renderable*>(this);
		return const_cast<T*>(constThis->propByName<T>(name));
	}
	
	/** TODO */
	const QMap<QString, SharedProp> props() const
	{
		return _props;
	}

public:
	static Renderer* renderer;

protected:
	float						_opacity;			/** Render opacity */
	float						_scale;				/** Scale */
	QMatrix4x4					_modelMatrix;		/** Model matrix */
	QMap<QString, SharedProp>	_props;				/** Props map */
};