//#pragma once
//
//#include <QObject>
//#include <QMatrix4x4>
//#include <QMap>
//
//#include "Prop.h"
//
//#include <stdexcept>
//
///**
// * Renderable class
// *
// * Base class for renderable nodes
// *
// * @author Thomas Kroes
// */
//class Renderable
//{
//public: // Construction/destruction
//
//    /** Default constructor */
//    Renderable();
//
//    /** Destructor */
//    ~Renderable();
//
//public: // Rendering
//
//    /**
//     * Renders the prop
//     * @param parentMVP Parent model view projection matrix
//     */
//    virtual void render(const QMatrix4x4& parentMVP) = 0;
//
//public: // Matrix functions
//
//    /** Returns the model matrix */
//    QMatrix4x4 getModelMatrix() const;
//
//    /**
//     * Sets the model matrix
//     * @param modelMatrix Model matrix
//     */
//    void setModelMatrix(const QMatrix4x4& modelMatrix);
//
//    /** Returns the model-view matrix */
//    QMatrix4x4 getModelViewMatrix() const;
//
//    /** Returns the model-view-projection matrix */
//    QMatrix4x4 getModelViewProjectionMatrix() const;
//
//public: // Opacity
//
//    /** Returns the render opacity */
//    QVariant getOpacity(const int& role) const;
//
//    /** Sets the render opacity
//     * @param opacity Render opacity
//    */
//    void setOpacity(const float& opacity);
//
//    /** Returns the scale */
//    QVariant getScale(const int& role) const;
//
//    /** Sets the scale
//     * @param scale Scale
//    */
//    void setScale(const float& scale);
//
//protected: // Prop management
//
//    /**
//     * Adds a prop
//     * @param T Prop type
//     * @param Args Prop constructor arguments
//     */
//    template<typename T, typename ...Args>
//    void addProp(Args... args)
//    {
//        try {
//            auto newProp    = new T(args...);
//            auto prop       = dynamic_cast<Prop*>(newProp);
//            auto propName   = prop->name();
//
//            if (_props.contains(propName))
//                throw std::runtime_error(QString("%1 already exists").arg(propName).toLatin1());
//
//            _props.insert(propName, newProp);
//        }
//        catch (const std::exception& e)
//        {
//            throw std::runtime_error(QString("Unable to add prop: %1").arg(e.what()).toLatin1());
//        }
//    }
//
//    /**
//     * Remove a prop by name
//     * @param name Prop name
//     */
//    void removeProp(const QString& name)
//    {
//        try {
//            if (!_props.contains(name))
//                throw std::runtime_error(QString("%1 does not exist").arg(name).toLatin1());
//
//            _props.remove(name);
//        }
//        catch (const std::exception& e)
//        {
//            throw std::runtime_error(QString("Unable to remove prop: %1").arg(e.what()).toLatin1());
//        }
//    }
//
//    /**
//     * Retrieve a prop by name
//     * @param name Prop name
//     */
//    template<typename T>
//    const T* getPropByName(const QString& name) const
//    {
//        try {
//            if (!_props.contains(name))
//                throw std::runtime_error(QString("no prop named %1").arg(name).toLatin1());
//
//            return dynamic_cast<T*>(_props[name]);
//        }
//        catch (const std::exception& e)
//        {
//            throw std::runtime_error(QString("Unable to retrieve prop: %1").arg(e.what()).toLatin1());
//        }
//    }
//
//    /**
//     * Retrieve a prop by name
//     * @param name Prop name
//     */
//    template<typename T>
//    T* getPropByName(const QString& name)
//    {
//        const auto constThis = const_cast<const Renderable*>(this);
//        return const_cast<T*>(constThis->getPropByName<T>(name));
//    }
//    
//    /** Returns all props */
//    const QMap<QString, Prop*> getProps() const
//    {
//        return _props;
//    }
//
//public:
//    static Renderer* renderer;                  /** Static renderer instance */
//
//protected:
//    float                   _opacity;           /** Render opacity */
//    float                   _scale;             /** Scale */
//    QMatrix4x4              _modelMatrix;       /** Model matrix */
//    QMap<QString, Prop*>    _props;             /** Props map */
//};