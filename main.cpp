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
		QOpenGLShaderProgram * m_rs_program;
		int m_rs_pos;
		int m_rs_texture;
		int m_rs_window_size;
		void initRS (void);
		void renderRS (void);

		/* renderJacobi */
		QOpenGLShaderProgram * m_rj_program;
		QOpenGLFramebufferObject * m_rj_fbos[2];
		int m_rj_texture;
		int m_rj_vertex;
		int m_rj_target_fbo;
		void initRJ (void);
		void renderRJ (void);

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
	initRJ ();
	initRS ();
}

void GlWindow::render () {
	renderRJ ();
	renderRS ();
	++m_frame;
}

/* Render Jacobi */

void GlWindow::initRJ (void) {
	// Shaders
	m_rj_program = new QOpenGLShaderProgram (this);
	m_rj_program->addShaderFromSourceFile (QOpenGLShader::Vertex, "rj_vertex.shader");
	m_rj_program->addShaderFromSourceFile (QOpenGLShader::Fragment, "rj_fragment.shader");
	m_rj_program->link ();

	// Handles to I
	m_rj_vertex = m_rj_program->attributeLocation ("vertex2DCoord");
	m_rj_texture = m_rj_program->attributeLocation ("prevJacobi");
	
	// Init framebuffers
	m_rj_target_fbo = 1; // initial target is 1, so initial source is 0
	for (int i = 0; i < 2; ++i) {
		m_rj_fbos[i] = new QOpenGLFramebufferObject (m_render_size,
				QOpenGLFramebufferObject::NoAttachment,
				GL_TEXTURE_2D, // standard 2D texture, will use texel*() to have absolute coords
				GL_R32F); // one float32 per cell

		glBindTexture (GL_TEXTURE_2D, m_rj_fbos[i]->texture ());
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	/* Set initial texture */
	QOpenGLFramebufferObject * init_fbo = m_rj_fbos[0];

	// Draw
	quint8 * img_data = new quint8[m_render_size.width () * m_render_size.height ()];
	for (int y = 0; y < m_render_size.height (); ++y) {
		for (int x = 0; x < m_render_size.width (); ++x) {
			quint8 * cell = &img_data[x + y * m_render_size.width ()];
			if (y % 20 < 3) *cell = 255;
			else *cell = 0;
		}
	}

	// Write to texture
	glBindTexture (GL_TEXTURE_2D, init_fbo->texture ());
	glTexSubImage2D (GL_TEXTURE_2D, // target
			0, // lod (base)
			0, 0, m_render_size.width (), m_render_size.height (), // size
			GL_RED, GL_UNSIGNED_BYTE, img_data);
}

void GlWindow::renderRJ (void) {
	//GLfloat lastColor = 0.5f * (1.0f + sin (10.0f * m_frame / screen ()->refreshRate ()));
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
	m_rs_texture = m_rs_program->attributeLocation ("jacobi");
}

void GlWindow::renderRS (void) {
	glViewport(0, 0, width (), height ());

	glClear (GL_COLOR_BUFFER_BIT);

	m_rs_program->bind ();

	// texture
	m_rs_program->setUniformValue (m_rs_texture, m_rj_fbos[0]->texture ());

	m_rs_program->setUniformValue ("width", (GLfloat) width ());
	m_rs_program->setUniformValue ("height", (GLfloat) height ());

	static const GLfloat vertices[] = {
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, 1.0f
	};

	m_rs_program->setAttributeArray (m_rs_pos, vertices, 2);

	m_rs_program->enableAttributeArray (m_rs_pos);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	m_rs_program->disableAttributeArray (m_rs_pos);

	m_rs_program->release();
}

