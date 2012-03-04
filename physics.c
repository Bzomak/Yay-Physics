#include "physics.h"

int bodyTotal;
Body *bodyArray;
Hlu camera;
double steps = 2.0;
double speed = 0.0;
Vector cpos, target;
bool keyStates[256];
bool specialStates[256];
int viewDirection;

void timerFunc (int notUsed) {

	if (speed != 0.0) {
		Vector temp = smult (speed, &(camera.heading));
		cpos = vplus (&cpos, &temp);
		if (viewDirection == 1) target = vplus (&cpos, &temp);
		else if (viewDirection == 2) target = vminus (&cpos, &(camera.heading));
		else if (viewDirection == 3) target = vplus (&cpos, &(camera.left));
		else if (viewDirection == 4) target = vminus (&cpos, &(camera.left));
	}
	glutPostRedisplay ();
}

void reshape (int width, int height) {

	if (height == 0) height = 1;
	glViewport (0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 100.0f);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard (unsigned char key, int x, int y) {

	keyStates[key] = true;
}

void keyboardUp (unsigned char key, int x, int y) {

	keyStates[key] = false;
}

void keyboardOperations (void) {

	if (keyStates[27]) exit(0);
	if (keyStates['W'] || keyStates['w']) speed += 0.02;
	if (keyStates['S'] || keyStates['s']) {
		speed -= 0.02;
		if (speed < 0.01) speed = 0.0; 
	}
	if (keyStates['R'] || keyStates['r']) {
		steps = 2.0;
		speed = 0.0;
		initialiseSettings ();
		initialiseArray ();
	}
	if (keyStates['V'] || keyStates['v']) {
		initialiseSettings ();
	}
	if (keyStates['+']) steps *= STEP;
	if (keyStates['-']) {
		steps /= STEP;
		if(steps < 1) steps = 1;
	}
	if (keyStates[',']) {
		yawLeft (&camera);
		if (viewDirection == 1) target = vplus (&cpos, &(camera.heading));
		else if (viewDirection == 2) target = vminus (&cpos, &(camera.heading));
		else if (viewDirection == 3) target = vplus (&cpos, &(camera.left));
		else if (viewDirection == 4) target = vminus (&cpos, &(camera.left));
	}
	if (keyStates['.']) {
		yawRight (&camera);
		if (viewDirection == 1) target = vplus (&cpos, &(camera.heading));
		else if (viewDirection == 2) target = vminus (&cpos, &(camera.heading));
		else if (viewDirection == 3) target = vplus (&cpos, &(camera.left));
		else if (viewDirection == 4) target = vminus (&cpos, &(camera.left));
	}
	if (keyStates['1']) viewDirection = 1;
	if (keyStates['2']) viewDirection = 2;
	if (keyStates['3']) viewDirection = 3;
	if (keyStates['4']) viewDirection = 4;
}

void special (int key, int x, int y) {
	
	specialStates[key] = true;
}

void specialUp (int key, int x, int y) {

	specialStates[key] = false;
}

void specialOperations (void) {

	if (specialStates[GLUT_KEY_UP]) {
		pitchDown (&camera);
		if (viewDirection == 1) target = vplus (&cpos, &(camera.heading));
		else if (viewDirection == 2) target = vminus (&cpos, &(camera.heading));
	}
	if (specialStates[GLUT_KEY_DOWN]) {
		pitchUp (&camera);
		if (viewDirection == 1) target = vplus (&cpos, &(camera.heading));
		else if (viewDirection == 2) target = vminus (&cpos, &(camera.heading));
	}
	if (specialStates[GLUT_KEY_LEFT]) {
		rollLeft (&camera);
		if (viewDirection == 3) target = vplus (&cpos, &(camera.left));
		else if (viewDirection == 4) target = vminus (&cpos, &(camera.left));
	}
	if (specialStates[GLUT_KEY_RIGHT]) {
		rollRight (&camera);
		if (viewDirection == 3) target = vplus (&cpos, &(camera.left));
		else if (viewDirection == 4) target = vminus (&cpos, &(camera.left));
	}
}

void display (void) {

	keyboardOperations ();
	specialOperations ();
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();
	gluLookAt (cpos.x, cpos.y, cpos.z, target.x, target.y, target.z, camera.up.x, camera.up.y, camera.up.z);
	int k;
	Body *bp;
	Body *bBegin;
	Body *bEnd;
	glutTimerFunc (TIMERMSECS, timerFunc, 0);
	bBegin = bodyArray;
	bEnd = bodyArray + bodyTotal;
	for (k = 0; k < (int)steps; ++k) {
		for (bp = bBegin; bp != bEnd; ++bp) {
			bp->acceleration = move (bp, bodyArray, bodyTotal);
		}
		for (bp = bBegin; bp != bEnd; ++bp) {
			bp->velocity = vplus (&(bp->velocity), &(bp->acceleration));
			bp->position = vplus (&(bp->position), &(bp->velocity));
			bp->path.pos++;
			PATH_MOD (bp->path.pos);
			bp->path.point[bp->path.pos] = bp->position;
		}
	}
	for (bp = bBegin; bp != bEnd; ++bp) {
		drawBody (&(bp->position), sqrt (bp->mass), &(bp->colour));
		drawPath (bp);
	}
	glutSwapBuffers ();
}

void drawBody (Vector *pos, double r, Vector *c) {

	r /= 100;
	glPushMatrix ();
	glTranslated (pos->x, pos->y, pos->z);
	glScaled (r, r, r);
	glColor3dv ((double *)c);
	glCallList (1);
	glPopMatrix ();
}

void drawPath (Body *b) {

	int i, j;
	glBegin (GL_LINE_STRIP);
	for (i = 1; i < PATHLEN; ++i) {
		j = i + b->path.pos;
		PATH_MOD (j);
		glColor4f (b->colour.x, b->colour.y, b->colour.z, (double)i / (double)PATHLEN);
		glVertex3f (b->path.point[j].x, b->path.point[j].y, b->path.point[j].z);
	}
	glEnd (); 
}

void initialiseGL (void) {

	srand (time (NULL));
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glEnable (GL_DEPTH_TEST);
	glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
	glEnable (GL_BLEND);
	glCullFace (GL_BACK);
	glEnable (GL_CULL_FACE);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable (GL_COLOR_MATERIAL);
	float light_pos[] = {100, 100, 100, 1};
	float light_ambient[] = {0.3, 0.3, 0.3, 1};
	float light_diffuse[] = {0.1, 0.1, 0.1, 1};
	float light_specular[] = {0, 0, 0, 1};
	glLightfv (GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
	glMaterialf (GL_FRONT, GL_SHININESS, 128);
	glNewList (1, GL_COMPILE);
	glutSolidSphere (1, 15, 15);
	glEndList ();
}

void initialiseSettings (void) {

	camera = newHlu (newVector (0, 0, -1), newVector (-1, 0, 0), newVector (0, 1, 0));
	cpos = newVector (0, 0, 10);
	target = vplus (&cpos, &(camera.heading));
	viewDirection = 1;	
}

void initialiseArray (void) {

	int i;
	double posrep = 2.0 / RAND_MAX;
	double velrep = 0.004 / RAND_MAX;
	for (i = 0; i < bodyTotal; ++i) {
		bodyArray[i] = newBody (
			newVector (1.0 - posrep * rand (), 1.0 - posrep * rand (), 1.0 - posrep * rand ()),
			newVector (0.002 - velrep * rand (), 0.002 - velrep * rand (), 0.002 - velrep * rand ()),
			100.0 * rand() / RAND_MAX
		);
	}
}

int main (int argc, char **argv) {

	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (INIT_WIN_WIDTH, INIT_WIN_HEIGHT);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Physics Fork");
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc (keyboard);
	glutKeyboardUpFunc (keyboardUp);
	glutSpecialFunc (special);
	glutSpecialUpFunc (specialUp);
	if(argc != 2) {
		printf("Usage: %s numbodies; Continuing with 100\n",argv[0]);
		bodyTotal = 100;
	}
	else bodyTotal = atoi(argv[1]);
	bodyArray = malloc (bodyTotal * sizeof (Body));
	initialiseGL ();
	initialiseArray ();
 	initialiseSettings ();
	glutMainLoop ();
	return 0;
}
