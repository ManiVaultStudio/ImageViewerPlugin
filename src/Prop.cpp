//#include "Prop.h"
//#include "Renderer.h"
//#include "Shape.h"
//#include "Node.h"
//
//#include <QOpenGLShaderProgram>
//#include <QOpenGLTexture>
//#include <QOpenGLFramebufferObject>
//#include <QDebug>
//
//Prop::Prop(Renderable& renderable, const QString& name) :
//    QObject(reinterpret_cast<QObject*>(&renderable)),
//    _renderable(node),
//    _initialized(false),
//    _name(name),
//    _visible(true),
//    _modelMatrix(),
//    _shaderPrograms(),
//    _textures(),
//    _shapes()
//{
//}
//
//Prop::~Prop() = default;
//
//void Prop::initialize()
//{
//    //qDebug() << "Initialize" << fullName();
//
//    renderer->bindOpenGLContext();
//
//    for (auto shape : _shapes) {
//        shape->initialize();
//    }
//}
//
//void Prop::destroy()
//{
//    //qDebug() << "Destroy" << fullName();
//
//    renderer->bindOpenGLContext();
//
//    for (auto shape : _shapes) {
//        shape->destroy();
//    }
//}
//
//bool Prop::canRender() const
//{
//    return isInitialized() && isVisible();
//}
//
//void Prop::addShaderProgram(const QString& name)
//{
//    _shaderPrograms.insert(name, QSharedPointer<QOpenGLShaderProgram>::create());
//}
//
//QSharedPointer<QOpenGLShaderProgram> Prop::getShaderProgramByName(const QString& name)
//{
//    return _shaderPrograms.value(name);
//}
//
//void Prop::addTexture(const QString& name, const QOpenGLTexture::Target& target)
//{
//    _textures.insert(name, QSharedPointer<QOpenGLTexture>::create(target));
//}
//
//QSharedPointer<QOpenGLTexture>& Prop::getTextureByName(const QString& name)
//{
//    return _textures[name];
//}
//
//void Prop::render(const QMatrix4x4& nodeMVP, const float& opacity)
//{
//    //qDebug() << "Render" << fullName();
//}
//
//bool Prop::isInitialized() const
//{
//    return _initialized;
//}
//
//QString Prop::name() const
//{
//    return _name;
//}
//
//void Prop::setName(const QString& name)
//{
//    if (name == _name)
//        return;
//
//    const auto oldName = getFullName();
//
//    _name = name;
//
//    qDebug() << "Rename" << oldName << "to" << getFullName();
//}
//
//bool Prop::isVisible() const
//{
//    return _visible;
//}
//
//void Prop::setVisible(const bool& visible)
//{
//    if (visible == _visible)
//        return;
//
//    _visible = visible;
//
//    qDebug() << (_visible ? "Show" : "Hide") << getFullName();
//}
//
//void Prop::show()
//{
//    setVisible(true);
//}
//
//void Prop::hide()
//{
//    setVisible(false);
//}
//
//QString Prop::getFullName()
//{
//    return _name;
//}
//
//QMatrix4x4 Prop::getModelMatrix() const
//{
//    return _node->getModelMatrix() * _modelMatrix;
//}
//
//void Prop::setModelMatrix(const QMatrix4x4& modelMatrix)
//{
//    if (modelMatrix == _modelMatrix)
//        return;
//
//    _modelMatrix = modelMatrix;
//}