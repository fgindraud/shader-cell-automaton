#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLShaderProgram>

#include <QtGui/QScreen>
#include <QtGui/QMatrix4x4>
#include <QtCore/qmath.h>

class GlWindow : public OpenGLWindow {
	public:
		GlWindow ();

		void initialize (void);
		void render (void);

	private:
		int m_posAttr;
		int m_colAttr;
		int m_matrixUniform;

		QOpenGLShaderProgram * m_program;
		int m_frame;
};

	GlWindow::GlWindow () : m_program (0), m_frame (0) {
}

int main (int argc, char ** argv) {
	QGuiApplication app (argc, argv);

	QSurfaceFormat format;
	format.setSamples (4);

	GlWindow window;
	window.setFormat (format);
	window.resize (640, 480);
	window.show ();

	window.setAnimating (true);

	return app.exec ();
}

void GlWindow::initialize () {
	// Create shaders
	m_program = new QOpenGLShaderProgram (this);
	m_program->addShaderFromSourceFile (QOpenGLShader::Vertex, "vertex.shader");
	m_program->addShaderFromSourceFile (QOpenGLShader::Fragment, "fragment.shader");
	m_program->link ();

	// Get handles to I/Os
	m_posAttr = m_program->attributeLocation ("posAttr");
	m_colAttr = m_program->attributeLocation ("colAttr");
}

void GlWindow::render () {
	glViewport(0, 0, width (), height ());

	glClear (GL_COLOR_BUFFER_BIT);

	m_program->bind ();
	
	static const GLfloat vertices[] = {
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, 1.0f
	};

	static const GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};

	m_program->setAttributeArray (m_posAttr, vertices, 2);
	m_program->setAttributeArray (m_colAttr, colors, 3);

	m_program->enableAttributeArray (m_posAttr);
	m_program->enableAttributeArray (m_colAttr);

	glDrawArrays(GL_QUADS, 0, 4);

	m_program->disableAttributeArray (m_posAttr);
	m_program->disableAttributeArray (m_colAttr);

	m_program->release();

	++m_frame;
}
