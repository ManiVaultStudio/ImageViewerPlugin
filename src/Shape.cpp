#include "Shape.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QDebug>

#include "Renderer.h"

Shape::Shape(Actor* actor, const QString& name) :
	_actor(actor),
	_name(name),
	_visible(true),
	_initialized(false),
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

	bindOpenGLContext();
	
	for (auto key : _vaos.keys())
	{
		_vaos[key]->destroy();
		_vaos[key]->release();
	}

	for (auto key : _vbos.keys())
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

	bindOpenGLContext();

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

bool Shape::isVisible() const
{
	return _visible;
}

void Shape::setVisible(const bool& visible)
{
	if (visible == _visible)
		return;

	_visible = visible;

	qDebug() << (_visible ? "Show" : "Hide") << _name;

	emit visibilityChanged(_visible);

	emit changed(this);
}

void Shape::show()
{
	setVisible(true);
}

void Shape::hide()
{
	setVisible(false);
}

bool Shape::canRender() const
{
	return isInitialized() && isVisible();
}

void Shape::render()
{
}

bool Shape::bindShaderProgram(const QString& name)
{
	const auto bound = shaderProgram(name)->bind();

	this->configureShaderProgram(name);

	return bound;
}

void Shape::setShaderProgram(const QString& name, QSharedPointer<QOpenGLShaderProgram> shaderProgram)
{
	if (_shaderPrograms.contains(name)) {
		qDebug() << "Adding shader program to" << _name;
		_shaderPrograms.insert(name, shaderProgram);
	}
	else {
		qDebug() << "Overwriting shader program" << name << "in" << _name;
		_shaderPrograms[name] = shaderProgram;
	}
}

void Shape::setVAO(const QString& name, QSharedPointer<QOpenGLVertexArrayObject> vao)
{
	if (_vaos.contains(name)) {
		qDebug() << "Adding vertex array object to" << _name;
		_vaos.insert(name, vao);
	}
	else {
		qDebug() << "Overwriting vertex array object" << name << "in" << _name;
		_vaos[name] = vao;
	}
}

void Shape::setVBO(const QString& name, QSharedPointer<QOpenGLBuffer> vbo)
{
	if (_vbos.contains(name)) {
		qDebug() << "Adding vertex buffer object to" << _name;
		_vbos.insert(name, vbo);
	}
	else {
		qDebug() << "Overwriting vertex buffer object" << name << "in" << _name;
		_vbos[name] = vbo;
	}
}

void Shape::setTexture(const QString& name, QSharedPointer<QOpenGLTexture> texture)
{
	if (_textures.contains(name)) {
		qDebug() << "Adding texture to" << _name;
		_textures.insert(name, texture);
	}
	else {
		qDebug() << "Overwriting texture" << name << "in" << _name;
		_textures[name] = texture;
	}
}

void Shape::setFBO(const QString& name, QSharedPointer<QOpenGLFramebufferObject> fbo)
{
	if (_fbos.contains(name)) {
		qDebug() << "Adding frame buffer object to" << _name;
		_fbos.insert(name, fbo);
	}
	else {
		qDebug() << "Overwriting frame buffer object" << name << "in" << _name;
		_fbos[name] = fbo;
	}
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

void Shape::bindOpenGLContext()
{
	renderer()->bindOpenGLContext();
}

void Shape::releaseOpenGLContext()
{
	renderer()->releaseOpenGLContext();
}

QColor Shape::color() const
{
	return _color;
}

void Shape::setColor(const QColor& color)
{
	if (color == _color)
		return;

	qDebug() << "Set selection bounds shape color";

	_color = color;

	emit colorChanged(_color);
}

Renderer* Shape::renderer()
{
	return _actor->renderer();
}