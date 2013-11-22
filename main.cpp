#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFramebufferObject>

#include <QtGui/QScreen>
#include <QtCore/qmath.h>

class GlWindow : public OpenGLWindow {
	public:
		GlWindow (const QSize & render_size);
		~GlWindow ();

		void initialize (void);
		void render (void);

	private:
		/* renderScreen */
		int m_rs_pos;
		int m_rs_color;
		QOpenGLShaderProgram * m_rs_program;
		void initRS (void);
		void renderRS (void);

		/* renderJacobi */
		/*QOpenGLShaderProgram * m_rj_program;
		QOpenGLFra
		int m_rj_texture;*/

		QSize m_render_size;
		int m_frame;
};

int main (int argc, char ** argv) {
	QGuiApplication app (argc, argv);

	GlWindow window (QSize (500, 500));
	window.resize (640, 480);
	window.show ();

	window.setAnimating (true);

	return app.exec ();
}

GlWindow::GlWindow (const QSize & render_size) : OpenGLWindow (0, true),
	m_render_size (render_size),
	m_frame (0)
{
}

GlWindow::~GlWindow () {
}

void GlWindow::initialize () {
	initRS ();
}

void GlWindow::render () {
	renderRS ();
	++m_frame;
}

/* Render for screen */

void GlWindow::initRS (void) {
	// Create shaders
	m_rs_program = new QOpenGLShaderProgram (this);
	m_rs_program->addShaderFromSourceFile (QOpenGLShader::Vertex, "rs_vertex.shader");
	m_rs_program->addShaderFromSourceFile (QOpenGLShader::Fragment, "rs_fragment.shader");
	m_rs_program->link ();

	// Get handles to I/Os
	m_rs_pos = m_rs_program->attributeLocation ("vertex2DCoord");
	m_rs_color = m_rs_program->attributeLocation ("vertexColor");
}

void GlWindow::renderRS (void) {
	glViewport(0, 0, width (), height ());

	glClear (GL_COLOR_BUFFER_BIT);

	m_rs_program->bind ();

	static const GLfloat vertices[] = {
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

	m_rs_program->setAttributeArray (m_rs_pos, vertices, 2);
	m_rs_program->setAttributeArray (m_rs_color, colors, 3);

	m_rs_program->enableAttributeArray (m_rs_pos);
	m_rs_program->enableAttributeArray (m_rs_color);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	m_rs_program->disableAttributeArray (m_rs_pos);
	m_rs_program->disableAttributeArray (m_rs_color);

	m_rs_program->release();
}

