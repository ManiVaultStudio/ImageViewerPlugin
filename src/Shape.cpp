#include "Shape.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QDebug>

#include "Renderer.h"

Shape::Shape(Renderer* renderer, const QString& name) :
	_renderer(renderer),
	_name(name),
	_active(false),
	_mouseEvents(static_cast<int>(MouseEvent::None)),
	_initialized(false),
	_enabled(true),
	_modelMatrix(),
	_shaderPrograms(),
	_vaos(),
	_vbos(),
	_textures(),
	_fbos()
{
}

Shape::~Shape() = default;

void Shape::destroy()
{
	qDebug() << "Destroy shape" << _name;

	for (auto key : _vaos.keys())
	{
		_vaos[key]->destroy();
		_vaos[key]->release();
	}

	for (auto key :_vbos.keys())
	{
		_vbos[key]->destroy();
		_vbos[key]->release();
	}

	for (auto key : _vaos.keys())
	{
		_textures[key]->destroy();
		_textures[key]->release();
	}

	for (auto key : _fbos.keys())
	{
		_fbos[key]->release();
	}
}

void Shape::initialize()
{
	qDebug() << "Initialize the" << _name << "shape";

	addShaderPrograms();
	addVAOs();
	addVBOs();
	addTextures();
	addFBOs();
}

bool Shape::isInitialized() const
{
	return _initialized;
}

QString Shape::name() const
{
	return _name;
}

void Shape::setName(const QString& name)
{
	if (name == _name)
		return;

	const auto oldName = _name;

	_name = name;

	qDebug() << "Rename" << oldName << "to" << _name;

	emit nameChanged(_name);

	emit changed(this);
}

bool Shape::isEnabled() const
{
	return _enabled;
}

void Shape::setEnabled(const bool& enabled)
{
	if (enabled == _enabled)
		return;

	_enabled = enabled;

	qDebug() << "Set " << _name << "shape" << (_enabled ? "enabled" : "disabled");

	emit enabledChanged(enabled);

	emit changed(this);
}

bool Shape::canRender() const
{
	return isEnabled() && isInitialized();
}

QMatrix4x4 Shape::modelMatrix() const
{
	return _modelMatrix;
}

void Shape::setModelMatrix(const QMatrix4x4& modelMatrix)
{
	if (modelMatrix == _modelMatrix)
		return;

	_modelMatrix = modelMatrix;

	//qDebug() << "Set model > view matrix for" << _name;

	emit modelMatrixChanged(_modelMatrix);
}

QVector3D Shape::translation() const
{
	const auto translationColumn = _modelMatrix.column(3);

	return QVector3D(translationColumn.x(), translationColumn.y(), translationColumn.z());
}

void Shape::setTranslation(const QVector3D& translation)
{
	_modelMatrix.setColumn(3, QVector4D(translation.x(), translation.y(), translation.z(), 1.f));
}

QMatrix4x4 Shape::modelViewMatrix() const
{
	return _renderer->viewMatrix() * _modelMatrix;
}

QMatrix4x4 Shape::modelViewProjectionMatrix() const
{
	return _renderer->projectionMatrix() * modelViewMatrix();
}

void Shape::render()
{
}

void Shape::log(const QString& event) const
{
	qDebug() << event.toLatin1().data();
}

bool Shape::bindShaderProgram(const QString& name)
{
	const auto bound = shaderProgram(name)->bind();

	this->configureShaderProgram(name);

	return bound;
}

void Shape::addShaderProgram(const QString& name, QSharedPointer<QOpenGLShaderProgram> shaderProgram)
{
	qDebug() << "Add OpenGL shader program to" << _name << "shape";

	_shaderPrograms.insert(name, shaderProgram);
}

void Shape::addVAO(const QString& name, QSharedPointer<QOpenGLVertexArrayObject> vao)
{
	qDebug() << "Add OpenGL VAO to" << _name << "shape";

	_vaos.insert(name, vao);
}

void Shape::addVBO(const QString& name, QSharedPointer<QOpenGLBuffer> vbo)
{
	qDebug() << "Add OpenGL VBO to" << _name << "shape";

	_vbos.insert(name, vbo);
}

void Shape::addTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture)
{
	qDebug() << "Add OpenGL texture to" << _name << "shape";

	_textures.insert(name, texture);
}

void Shape::addFBO(const QString& name, QSharedPointer<QOpenGLFramebufferObject> fbo)
{
	qDebug() << "Add OpenGL texture to" << _name << "shape";

	_fbos.insert(name, fbo);
}

void Shape::configureShaderProgram(const QString& name)
{
	qDebug() << "Configure shader program" << name << "for" << _name << "not implemented";
}

void Shape::update()
{
	qDebug() << "Updating" << _name;
}

QSharedPointer<QOpenGLShaderProgram> Shape::shaderProgram(const QString& name)
{
	return _shaderPrograms[name];
}

const QSharedPointer<QOpenGLShaderProgram> Shape::shaderProgram(const QString& name) const
{
	return _shaderPrograms[name];
}

QSharedPointer<QOpenGLVertexArrayObject> Shape::vao(const QString& name)
{
	return _vaos[name];
}

const QSharedPointer<QOpenGLVertexArrayObject> Shape::vao(const QString& name) const
{
	return _vaos[name];
}

QSharedPointer<QOpenGLBuffer> Shape::vbo(const QString& name)
{
	return _vbos[name];
}

const QSharedPointer<QOpenGLBuffer> Shape::vbo(const QString& name) const
{
	return _vbos[name];
}

QSharedPointer<QOpenGLTexture> Shape::texture(const QString& name)
{
	return _textures[name];
}

const QSharedPointer<QOpenGLTexture> Shape::texture(const QString& name) const
{
	return _textures[name];
}

QSharedPointer<QOpenGLFramebufferObject> Shape::fbo(const QString& name)
{
	return _fbos[name];
}

const QSharedPointer<QOpenGLFramebufferObject> Shape::fbo(const QString& name) const
{
	return _fbos[name];
}

void Shape::onMousePressEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse press event not implemented in" << _name;
}

void Shape::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse release event not implemented in" << _name;
}

void Shape::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse move event not implemented in" << _name;
}

void Shape::onMouseWheelEvent(QWheelEvent* wheelEvent)
{
	qDebug() << "Mouse wheel event not implemented in" << _name;
}

bool Shape::handlesMousePressEvents()
{
	return _mouseEvents & static_cast<int>(MouseEvent::Press);
}

bool Shape::handlesMouseReleaseEvents()
{
	return _mouseEvents & static_cast<int>(MouseEvent::Release);
}

bool Shape::handlesMouseMoveEvents()
{
	return _mouseEvents & static_cast<int>(MouseEvent::Move);
}

bool Shape::handlesMouseWheelEvents()
{
	return _mouseEvents & static_cast<int>(MouseEvent::Wheel);
}

bool Shape::isActive() const
{
	return _active;
}

void Shape::activate()
{
	if (!_enabled)
		return;

	_active = true;

	qDebug() << "Activated" << _name;
}

void Shape::deactivate()
{
	_active = false;

	qDebug() << "Deactivated" << _name;
}