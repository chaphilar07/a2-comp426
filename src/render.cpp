#include "render.h"
#include "grid.h"
#include <GL/freeglut.h>

static void (*step_fn)(void);
static const char *(*status_fn)(void);

static void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(-1, -1);
  glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, rgb); /* one pixel per cell */

  glColor3f(0, 0, 0);
  glRectf(-1.0f, 0.92f, 0.4f, 1.0f); /* black bar behind text */
  glColor3f(1, 1, 1);
  glRasterPos2f(-0.98f, 0.94f);
  glutBitmapString(GLUT_BITMAP_HELVETICA_18,
                   (const unsigned char *)status_fn());
  glutSwapBuffers();
}

static void tick(int) {
  glutTimerFunc(33, tick, 0); /* re-arm first so the period stays 33 ms */
  step_fn();
  glutPostRedisplay();
}

static void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glPixelZoom((float)w / W, (float)h / H);
}

void render_run(int *argc, char **argv, void (*step)(void),
                const char *(*status)(void)) {
  step_fn = step;
  status_fn = status;
  glutInit(argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(W, H);
  glutCreateWindow("COMP 426 A2 - cell growth with TBB");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(33, tick, 0);
  glutMainLoop();
}
