#include "openglwindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFramebufferObject>

#include <QtCore/QTimer>

class GlWindow : public OpenGLWindow {
	Q_OBJECT
	public:
		GlWindow ();
		~GlWindow ();

		void initialize (void);
		void render (void);

	private:
		/* renderScreen */
		QOpenGLShaderProgram * m_rs_program;
		void initRS (void);
		void renderRS (void);

		quint64 m_frame;
	
		QTimer m_timer;
		quint64 m_frame_offset;
		void timer_init (void);
	private slots:
		/* FIXME timing */
		void timer_end (void);
};

