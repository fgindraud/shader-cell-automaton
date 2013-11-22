#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>

#include <QtGui/QScreen>
#include <QtCore/qmath.h>

class GlWindow : public OpenGLWindow {
	public:
		GlWindow ();
		~GlWindow ();

		void initialize (void);
		void render (void);

	private:
		int m_posAttr;
		int m_colAttr;

		QOpenGLBuffer * m_vertex_pos_buffer;

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

GlWindow::GlWindow () : OpenGLWindow (0, true),
	m_vertex_pos_buffer (0),
	m_program (0), m_frame (0)
{
}

GlWindow::~GlWindow () {
	if (m_vertex_pos_buffer != 0)
		delete m_vertex_pos_buffer;
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

	// Fill vertex buffer
	static const GLfloat vertices[] = {
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, 1.0f
	};

	m_vertex_pos_buffer = new QOpenGLBuffer (QOpenGLBuffer::VertexBuffer);
	m_vertex_pos_buffer->create ();
	m_vertex_pos_buffer->setUsagePattern (QOpenGLBuffer::StaticDraw);
	m_vertex_pos_buffer->bind ();
	m_vertex_pos_buffer->allocate (vertices, 8 * sizeof (GLfloat));
}

void GlWindow::render () {
	glViewport(0, 0, width (), height ());

	glClear (GL_COLOR_BUFFER_BIT);

	m_program->bind ();

	GLfloat lastColor = 0.5f * (1.0f + sin (10.0f * m_frame / screen ()->refreshRate ()));

	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		lastColor, lastColor, lastColor
	};

	/* Only one buffer can be bound at a given time, and setAttrBuffer refers to this one.
	 */
	m_vertex_pos_buffer->bind ();
	m_program->setAttributeBuffer (m_posAttr, GL_FLOAT, 0, 2);
	m_vertex_pos_buffer->release ();

	// Stays dynamic
	m_program->setAttributeArray (m_colAttr, colors, 3);

	m_program->enableAttributeArray (m_posAttr);
	m_program->enableAttributeArray (m_colAttr);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	m_program->disableAttributeArray (m_posAttr);
	m_program->disableAttributeArray (m_colAttr);

	m_program->release();

	++m_frame;
}
