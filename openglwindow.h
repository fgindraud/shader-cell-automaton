#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLDebugLogger>

class QPainter;
class QOpenGLPaintDevice;

class OpenGLWindow : public QWindow, protected QOpenGLFunctions {
	Q_OBJECT
	public:
		explicit OpenGLWindow (QWindow * parent = 0, bool want_gl_log = false);
		~OpenGLWindow ();

		virtual void render (QPainter * painter); // QPainter render callback, if used (needs an untouched render(void) func)
		virtual void render (void); // Render callback
		virtual void initialize (void); // Post GL context creation user init

		void setAnimating (bool animating);

	public slots:
		void renderLater (void);
		void renderNow (void);

	protected:
		bool event (QEvent * event);

		void exposeEvent (QExposeEvent * event);
		void resizeEvent (QResizeEvent * event);

	private:
		bool m_animating;
		bool m_want_log;
		
		bool m_update_pending;
		bool m_initialized;
		
		QOpenGLContext m_context;
		QOpenGLPaintDevice * m_device;
		QOpenGLDebugLogger * m_logger;

		void deferedInit (void);
	private slots:
		void onDebugMessage (QOpenGLDebugMessage message);
};

#endif
