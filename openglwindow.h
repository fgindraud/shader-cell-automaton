#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>

class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;

class OpenGLWindow : public QWindow, protected QOpenGLFunctions {
	Q_OBJECT
	public:
		explicit OpenGLWindow (QWindow * parent = 0);
		~OpenGLWindow ();

		virtual void render (QPainter * painter);
		virtual void render (void);

		virtual void initialize (void);

		void setAnimating (bool animating);

	public slots:
		void renderLater (void);
		void renderNow (void);

	protected:
		bool event (QEvent * event);

		void exposeEvent (QExposeEvent * event);
		void resizeEvent (QResizeEvent * event);

	private:
		bool m_update_pending;
		bool m_animating;
		bool m_initialized;
		QOpenGLContext m_context;

		QOpenGLPaintDevice * m_device;

		void deferedInit (void);
};

#endif
