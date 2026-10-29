
#ifndef RENDER_H
#define RENDER_H

/* The control thread: window, 1/30 s timer, OpenGL drawing. Never returns.
 * step   is called on this thread every 33 ms before the redraw.
 * status returns the text shown in the top-left corner. */
void render_run(int *argc, char **argv, void (*step)(void),
                const char *(*status)(void));

#endif
