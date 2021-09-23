//#include "Renderable.h"
//#include "Renderer.h"
//
//#include <QDebug>
//
//Renderable::Renderable() :
//    _opacity(1.0f),
//    _scale(1.0f),
//    _modelMatrix(),
//    _props()
//{
//}
//
//Renderable::~Renderable() = default;
//
//QMatrix4x4 Renderable::getModelMatrix() const
//{
//    auto scaleMatrix = QMatrix4x4();
//
//    scaleMatrix.scale(_scale);
//
//    return _modelMatrix * scaleMatrix;
//}
//
//void Renderable::setModelMatrix(const QMatrix4x4& modelMatrix)
//{
//    if (modelMatrix == _modelMatrix)
//        return;
//
//    _modelMatrix = modelMatrix;
//}
//
//QMatrix4x4 Renderable::getModelViewMatrix() const
//{
//    return renderer->getViewMatrix() * _modelMatrix;
//}
//
//QMatrix4x4 Renderable::getModelViewProjectionMatrix() const
//{
//    return renderer->getProjectionMatrix() * getModelViewMatrix();
//}
//
//QVariant Renderable::getOpacity(const int& role) const
//{
//    const auto opacityString = QString("%1%").arg(QString::number(100.0f * _opacity, 'f', 1));
//
//    switch (role)
//    {
//    case Qt::DisplayRole:
//        return opacityString;
//
//    case Qt::EditRole:
//        return _opacity;
//
//    case Qt::ToolTipRole:
//        return QString("Opacity: %1").arg(opacityString);
//
//    default:
//        break;
//    }
//
//    return QVariant();
//}
//
//void Renderable::setOpacity(const float& opacity)
//{
//    _opacity = opacity;
//}
//
//QVariant Renderable::getScale(const int& role) const
//{
//    const auto scaleString = QString("%1%").arg(QString::number(100.0f * _scale, 'f', 1));
//
//    switch (role)
//    {
//    case Qt::DisplayRole:
//        return scaleString;
//
//    case Qt::EditRole:
//        return _scale;
//
//    case Qt::ToolTipRole:
//        return QString("Scale: %1").arg(scaleString);
//
//    default:
//        break;
//    }
//
//    return QVariant();
//}
//
//void Renderable::setScale(const float& scale)
//{
//    _scale = scale;
//}