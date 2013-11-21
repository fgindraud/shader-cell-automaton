#include "openglwindow.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>

OpenGLWindow::OpenGLWindow (QWindow * parent) : QWindow (parent), 
	m_update_pending (false), m_animating (false), m_initialized (false),
	m_device (0)
{
	setSurfaceType (QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow () {
	if (m_device != 0)
		delete m_device;
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
		m_update_pending = true;
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

void OpenGLWindow::deferedInit (void) {
	m_context.setFormat (requestedFormat ());
	m_context.create ();

	m_context.makeCurrent (this);

	initializeOpenGLFunctions ();
	initialize ();

	m_initialized = true;
}

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

