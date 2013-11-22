#include "main.h"

int main (int argc, char ** argv) {
	QGuiApplication app (argc, argv);

	GlWindow window (QSize (5000, 5000));
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
	
	timer_init ();
}

void GlWindow::render () {
	renderRJ ();
	renderRS ();
	++m_frame;
}

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

/* Render Jacobi */

void GlWindow::initRJ (void) {
	// Shaders
	m_rj_program = new QOpenGLShaderProgram (this);
	m_rj_program->addShaderFromSourceFile (QOpenGLShader::Vertex, "rj_vertex.shader");
	m_rj_program->addShaderFromSourceFile (QOpenGLShader::Fragment, "rj_fragment.shader");
	m_rj_program->link ();

	m_rj_program->setUniformValue ("prev_jacobi", 0); // texture unit 0

	// Create a initial value map
	quint8 * img_data = new quint8[m_render_size.width () * m_render_size.height ()];
	for (int y = 0; y < m_render_size.height (); ++y) {
		for (int x = 0; x < m_render_size.width (); ++x) {
			quint8 * cell = &img_data[x + y * m_render_size.width ()];
			if (y % 20 < 3) *cell = 255;
			else *cell = 0;
		}
	}

	// Init framebuffers
	for (int i = 0; i < 2; ++i) {
		m_rj_fbos[i] = new QOpenGLFramebufferObject (m_render_size,
				QOpenGLFramebufferObject::NoAttachment,
				GL_TEXTURE_2D, // standard 2D texture, will use texel*() to have absolute coords
				GL_R32F); // one float32 per cell

		glBindTexture (GL_TEXTURE_2D, m_rj_fbos[i]->texture ());
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Put initial image
		glTexSubImage2D (GL_TEXTURE_2D, // target
				0, // lod (base)
				0, 0, m_render_size.width (), m_render_size.height (), // size
				GL_RED, GL_UNSIGNED_BYTE, img_data);
	}

	delete [] img_data;
}

void GlWindow::renderRJ (void) {
	glViewport(0, 0, m_render_size.width (), m_render_size.height ());

	glClear (GL_COLOR_BUFFER_BIT);

	m_rj_program->bind ();

	// origin texture (on unit 0)
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, m_rj_fbos[m_frame % 2]->texture ());

	// set target texture
	m_rj_fbos[1 - m_frame % 2]->bind ();

	static const GLfloat vertices[] = {
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f,
		-1.0f, 1.0f
	};

	int vertex_loc = m_rj_program->attributeLocation ("vertex_coords");

	m_rj_program->setAttributeArray (vertex_loc, vertices, 2);

	m_rj_program->enableAttributeArray (vertex_loc);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	m_rj_program->disableAttributeArray (vertex_loc);

	m_rj_program->release();

	// reset target
	QOpenGLFramebufferObject::bindDefault ();
}

/* Render for screen */

void GlWindow::initRS (void) {
	// Create shaders
	m_rs_program = new QOpenGLShaderProgram (this);
	m_rs_program->addShaderFromSourceFile (QOpenGLShader::Vertex, "rs_vertex.shader");
	m_rs_program->addShaderFromSourceFile (QOpenGLShader::Fragment, "rs_fragment.shader");
	m_rs_program->link ();

	m_rs_program->setUniformValue ("jacobi_texture", 0); // Texture unit 0
}

void GlWindow::renderRS (void) {
	glViewport(0, 0, width (), height ());

	glClear (GL_COLOR_BUFFER_BIT);

	m_rs_program->bind ();

	// texture on unit 0	
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, m_rj_fbos[1 - m_frame % 2]->texture ());

	m_rs_program->setUniformValue ("width", (GLfloat) width ());
	m_rs_program->setUniformValue ("height", (GLfloat) height ());

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

