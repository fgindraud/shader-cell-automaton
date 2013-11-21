#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLShaderProgram>

#include <QtGui/QScreen>
#include <QtCore/qmath.h>

class GlWindow : public OpenGLWindow {
	public:
		GlWindow ();

		void initialize (void);
		void render (void);

	private:
		int m_posAttr;
		int m_colAttr;

		QOpenGLShaderProgram * m_program;
		int m_frame;
};

int main (int argc, char ** argv) {
	QGuiApplication app (argc, argv);

	GlWindow window;
	window.resize (640, 480);
	window.show ();

	window.setAnimating (true);

	return app.exec ();
}

GlWindow::GlWindow () : m_program (0), m_frame (0) {
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

	GLfloat vertices[] = {
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, 1.0f
	};

	GLfloat lastColor = 0.5f * (1.0f + sin (10.0f * m_frame / screen ()->refreshRate ()));

	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		lastColor, lastColor, lastColor
	};

	m_program->setAttributeArray (m_posAttr, vertices, 2);
	m_program->setAttributeArray (m_colAttr, colors, 3);

	m_program->enableAttributeArray (m_posAttr);
	m_program->enableAttributeArray (m_colAttr);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	m_program->disableAttributeArray (m_posAttr);
	m_program->disableAttributeArray (m_colAttr);

	m_program->release();

	++m_frame;
}
