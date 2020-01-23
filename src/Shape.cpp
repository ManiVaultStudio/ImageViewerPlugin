#include "Shape.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QDebug>

Shape::Shape(const QString& name) :
	_name(name),
	_initialized(false),
	_enabled(),
	_shaderPrograms(),
	_vaos(),
	_vbos(),
	_textures()
{
}

Shape::~Shape() = default;

void Shape::destroy()
{
	qDebug() << "Destroy shape" << _name;

	for (auto key : _vaos.keys())
	{
		_vaos[key]->destroy();
	}

	for (auto key :_vbos.keys())
	{
		_vbos[key]->destroy();
	}

	for (auto key : _vaos.keys())
	{
		_textures[key]->destroy();
	}
}

void Shape::initialize()
{
	qDebug() << "Initialize the" << _name << "shape";

	addShaderPrograms();
	addVAOs();
	addVBOs();
	addTextures();
}

bool Shape::isInitialized() const
{
	return _initialized;
}

bool Shape::isEnabled() const
{
	return _enabled;
}

void Shape::setEnabled(const bool& enabled)
{
	if (enabled == enabled)
		return;

	_enabled = enabled;

	qDebug() << "Set " << _name << "shape" << (_enabled ? "enabled" : "disabled");

	emit enabledChanged(enabled);
}

bool Shape::canRender() const
{
	return isEnabled() && isInitialized();
}

void Shape::render()
{
	//log(__FUNCTION__);
}

void Shape::log(const QString& event) const
{
	qDebug() << event.toLatin1().data();
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

QSharedPointer<QOpenGLShaderProgram> Shape::shaderProgram(const QString& name)
{
	return _shaderPrograms[name];
}

QSharedPointer<QOpenGLVertexArrayObject> Shape::vao(const QString& name)
{
	return _vaos[name];
}

QSharedPointer<QOpenGLBuffer> Shape::vbo(const QString& name)
{
	return _vbos[name];
}

QSharedPointer<QOpenGLTexture> Shape::texture(const QString& name)
{
	return _textures[name];
}