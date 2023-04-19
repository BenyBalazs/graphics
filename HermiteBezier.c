#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#define WIDTH 640
#define HEIGHT 640
#define N 4
#define M 100
GLfloat t;
GLint i;
GLuint vao, vbo;
GLuint vao2, vbo2;
//pontok
GLfloat points[12] = { -0.75f, 0.5f, 0.0f, -0.25f, 0.25f, 0.0f, 0.25f, 0.75f, 0.0f,0.75f,-0.25f,0.0f };
// megfogott pont index (0,1,2) a -1 ha nincs semmi megfogva
GLint dragged = -1;
GLfloat HermiteCurve[3 * M];

//distance square 2 pont koordinátája.
//2 pont négyzetösszege ha kisebb mint a sugár négyzet akkor benne van a trashold-ba ezért elkapta a négyzetet
GLfloat dist2_2d(GLfloat P1x, GLfloat P1y, GLfloat P2x, GLfloat P2y) {

    GLfloat dx = P1x - P2x;
    GLfloat dy = P1y - P2y;
    return dx * dx + dy * dy;
}
/*
s a sugár négyzet
a koordináta rendszer 1-1(jobb lenti sarok) től -1-1(bal fenti sarok) ig van megadva de pixelben WIDTHxHEIGHT os
azért h tudjam hol az egér át kell konvertálni a widthx height rendszerből a -1 - 1 es rendszerbe
azért, hogy ez meglegyen a elosztom kettővel a width-et vagy heightot, hogy megkapjam mennyi 1 egység
utána ha 2 pont távolsága kisebb mint a sugár négyzet akkor az i-ed pontot megfogtuk
máskülönben nem fogtunk meg semmit
mindig az első pontot találja meg, tehát ha egymáson 2 pont van azt találja meg amelyik hamarabb indexen van
*/

GLint getActivePoint(GLfloat* p, GLfloat sensitivity, GLfloat x, GLfloat y) {

    GLfloat		s = sensitivity * sensitivity;
    GLfloat		xNorm = -1 + x / (WIDTH / 2);
    GLfloat		yNorm = -1 + (HEIGHT - y) / (HEIGHT / 2);

    for (GLint i = 0; i < 4; i++)
        if (dist2_2d(p[i * 3], p[i * 3 + 1], xNorm, yNorm) < s)
            return i;

    return -1;
}


/*
a második paraméter mindig az egér, a harmadik, az action
ha a gomb amit lenyomtam az a bal gomb és le lett nyomva akkor nézzünk tovább mindent, hogy mit fogtam meg ha megfogtam valamit
ha a bal gombbal történt valami és az a valami a felengedés volt akkor a dragged = -1 ami azt jelenti h nem fogunk semmit.
*/

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double	x, y;

        glfwGetCursorPos(window, &x, &y);
        dragged = getActivePoint(points, 0.1f, x, y);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragged = -1;
}

void hermit() {
    GLfloat step = 1.0f / (M - 1); // t \in [-1,2]
    HermiteCurve[0] = points[0];
    HermiteCurve[1] = points[1];
    HermiteCurve[2] = 0.0f;
    for (i = 1; i < (M - 1); i++) {
        t = i * step;
        HermiteCurve[i * 3] = points[0] * ((1-t) * (1 - t) * (1 - t)) + points[3] * 3*(((1-t)*(1-t))*t) + points[6] * 3*((1-t)*(t*t)) + points[9] * (t*t*t);
        HermiteCurve[i * 3 + 1] = points[1] * ((1 - t) * (1 - t) * (1 - t)) + points[4] * 3 * (((1 - t) * (1 - t)) * t) + points[7] * 3 * ((1 - t) * (t * t)) + points[10] * (t * t * t);
        HermiteCurve[i * 3 + 2] = 0.0f;
    }

    HermiteCurve[(3 * M) - 3] = points[9];
    HermiteCurve[(3 * M) - 2] = points[10];
    HermiteCurve[(3 * M) - 1] = 0.0f;
}
/*
x,y az egér poziciók

ha a dragged >=0, akkor megfogtunk valamit,
és amíg ez igaz addig a pontom koordinátáját felülírom a -1-1 1-1 koordináta rendszerben átszámított értékkel
és ha ez változott akkor ezt küldjük a vbo-nak (újra rendereljük)

*/

void cursorPosCallback(GLFWwindow* window, double x, double y) {

    if (dragged >= 0) {

        GLfloat		xNorm = -1 + x / (WIDTH / 2);
        GLfloat		yNorm = -1 + (HEIGHT - y) / (HEIGHT / 2);

        points[3 * dragged] = xNorm;  // x coord
        points[3 * dragged + 1] = yNorm;  // y coord

        hermit();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), points, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        glBindBuffer(GL_ARRAY_BUFFER, vbo2);
        glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}
int main() {
    GLFWwindow* window = NULL;
    const GLubyte* renderer;
    const GLubyte* version;

    hermit();

    const char* vertex_shader =
        "#version 330\n"
        "in vec3 vp;"
        "void main () {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

    const char* fragment_shader =
        "#version 330\n"
        "out vec4 frag_colour;"
        "void main () {"
        "  frag_colour = vec4(1.0, 1.0, 0.0, 1.0);"
        "}";

    GLuint vert_shader, frag_shader;
    GLuint shader_programme;

    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Drag&Drop", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);



    glGenBuffers(1, &vbo2);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, (3 * M) * sizeof(GLfloat), HermiteCurve, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), points, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vertex_shader, NULL);
    glCompileShader(vert_shader);

    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &fragment_shader, NULL);
    glCompileShader(frag_shader);

    shader_programme = glCreateProgram();
    glAttachShader(shader_programme, frag_shader);
    glAttachShader(shader_programme, vert_shader);
    glLinkProgram(shader_programme);
    //az ablakokra beadjuk a megfelelő függvényeket.

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    glPointSize(15.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_programme);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);

        glUseProgram(shader_programme);
        glBindVertexArray(vao2);
        glDrawArrays(GL_LINE_STRIP, 0, M);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
