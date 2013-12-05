#include "main.h"

#include <cmath>

int main (int argc, char ** argv) {
	QGuiApplication app (argc, argv);

	GlWindow window; 
	window.resize (480, 480);
	window.show ();

	window.setAnimating (true);

	return app.exec ();
}

GlWindow::GlWindow () : OpenGLWindow (0, true),
	m_frame (0)
{
}

GlWindow::~GlWindow () {
}

void GlWindow::initialize () { initRS (); timer_init (); }
void GlWindow::render () { renderRS (); ++m_frame; }

/* timer stuff */
void GlWindow::timer_init (void) {
	connect (&m_timer, SIGNAL (timeout ()), this, SLOT (timer_end ()));
	m_timer.start (10000);
	m_frame_offset = 0;
}

void GlWindow::timer_end (void) {
	double fps_elapsed = static_cast< double > (m_frame - m_frame_offset) / 10.;
	qDebug () << "FPS" << fps_elapsed;
	m_frame_offset = m_frame;
}

/* Render for screen */

void GlWindow::initRS (void) {
	// Create shaders
	m_rs_program = new QOpenGLShaderProgram (this);
	m_rs_program->addShaderFromSourceFile (QOpenGLShader::Vertex, "screen.vertex.glsl");
	m_rs_program->addShaderFromSourceFile (QOpenGLShader::Fragment, "screen.fragment.glsl");
	m_rs_program->link ();

}

void GlWindow::renderRS (void) {
	glViewport(0, 0, width (), height ());

	glClear (GL_COLOR_BUFFER_BIT);

	m_rs_program->bind ();

	m_rs_program->setUniformValue ("width", GLfloat (width ()));
	m_rs_program->setUniformValue ("height", GLfloat (height ()));
	m_rs_program->setUniformValue ("var", GLfloat (m_frame % 1000));

	static const GLfloat vertices[] = {
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, 1.0f
	};
	
	int vertex_loc = m_rs_program->attributeLocation ("vertex_coords");

	m_rs_program->setAttributeArray (vertex_loc, vertices, 2);

	m_rs_program->enableAttributeArray (vertex_loc);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	m_rs_program->disableAttributeArray (vertex_loc);

	m_rs_program->release();
}

