#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFramebufferObject>

#include <QtCore/QTimer>

class GlWindow : public OpenGLWindow {
	Q_OBJECT
	public:
		GlWindow (const QSize & render_size);
		~GlWindow ();

		void initialize (void);
		void render (void);

	private:
		/* renderScreen */
		QOpenGLShaderProgram * m_rs_program;
		void initRS (void);
		void renderRS (void);

		/* renderJacobi */
		QOpenGLShaderProgram * m_rj_program;
		QOpenGLFramebufferObject * m_rj_fbos[2];
		void initRJ (void);
		void renderRJ (void);

		QSize m_render_size;
		quint64 m_frame;
	
		QTimer m_timer;
		quint64 m_frame_offset;
		void timer_init (void);
	private slots:
		/* FIXME timing */
		void timer_end (void);
};

