#include "openglwindow.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

/* Init */

OpenGLWindow::OpenGLWindow (QWindow * parent, bool want_gl_log) :
	QWindow (parent),
	m_animating (false), m_want_log (want_gl_log),
	m_update_pending (false), m_initialized (false),
	m_device (0), m_logger (0)
{
	setSurfaceType (QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow () {
	if (m_device != 0)
		delete m_device;
}

void OpenGLWindow::deferedInit (void) {
	m_context.setFormat (requestedFormat ());
	m_context.create ();

	m_context.makeCurrent (this);

	// Debug logger
	if (m_want_log) {
		if (m_context.hasExtension (QByteArrayLiteral ("GL_KHR_debug"))) {
			// Try enabling debug
			m_logger = new QOpenGLDebugLogger (this);
			QObject::connect (m_logger, SIGNAL (messageLogged (QOpenGLDebugMessage)),
					this, SLOT (onDebugMessage (QOpenGLDebugMessage)));

			if (m_logger->initialize ()) {
				m_logger->startLogging (QOpenGLDebugLogger::SynchronousLogging);
				m_logger->enableMessages ();
			} else {
				qFatal ("QOpenGLDebugLogger : cannot initialize !");
			}
		} else {
			qDebug () << "QOpenGLDebugLogger : no GL_KHR_debug extension, disabled";
		}
	}

	initializeOpenGLFunctions ();
	initialize ();

	m_initialized = true;
}

void OpenGLWindow::onDebugMessage (QOpenGLDebugMessage message) {
	qDebug () << message;
}

/* Over written stuff */

void OpenGLWindow::render (QPainter * painter) {
	Q_UNUSED (painter);
}

void OpenGLWindow::initialize (void) {
}

void OpenGLWindow::render (void) {
	if (!m_device)
		m_device = new QOpenGLPaintDevice;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_device->setSize (size ());

	QPainter painter (m_device);
	render (&painter);
}

/* Render-triggering events */

void OpenGLWindow::renderLater (void) {
	if (not m_update_pending) {
		m_update_pending = true; // Avoid multiple render requests
		QCoreApplication::postEvent (this, new QEvent (QEvent::UpdateRequest));
	}
}

bool OpenGLWindow::event (QEvent * event) {
	switch (event->type ()) {
		case QEvent::UpdateRequest:
			renderNow ();
			return true;
		default:
			return QWindow::event (event);
	}
}

void OpenGLWindow::exposeEvent (QExposeEvent * event) {
	Q_UNUSED (event);

	if (isExposed ())
		renderLater ();
}

void OpenGLWindow::resizeEvent (QResizeEvent *event) {
	Q_UNUSED (event);

	if (isExposed ())
		renderLater ();
}

/* Render gl stuff */

void OpenGLWindow::renderNow (void) {
	m_update_pending = false;

	if (not isExposed ())
		return;

	if (Q_LIKELY (m_initialized))
		m_context.makeCurrent (this);
	else
		deferedInit ();

	render ();

	m_context.swapBuffers (this);

	if (m_animating)
		renderLater ();
}

void OpenGLWindow::setAnimating (bool animating) {
	m_animating = animating;

	if (animating)
		renderLater ();
}

