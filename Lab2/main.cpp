// Definicja u¿ycia sta³ych matematycznych z biblioteki matematycznej C++
#define _USE_MATH_DEFINES

// Do³¹czenie niezbêdnych bibliotek dla systemu Windows, OpenGL i innych funkcji
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

// Definicja typu punktu 3D
typedef float point3[3];

// Deklaracja zmiennych dla kolorów i liczby punktów (N)
point3* colours;
int N;

// Zdefiniowanie zmiennych dla rodzaju modelu, trybu widoku i parametrów transformacji
int model = 1;
bool tryb = true;  // tryb widoku
static GLfloat R = 10.0;  // odleg³oœæ od obserwatora do sceny
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };  // pozycja obserwatora
static GLfloat punkt_obserwacji[] = { 0.0, 0.0, 0.0 };  // punkt, na który jest skierowany wzrok obserwatora
static GLfloat theta_x = 0.0;  // k¹t obrotu wokó³ osi y
static GLfloat theta_y = 0.0;  // k¹t obrotu wokó³ osi x
static GLfloat theta_x1 = 0.0;  // k¹t obrotu wokó³ osi y dla trybu niezale¿nego od punktu obserwacji
static GLfloat theta_y1 = 0.0;  // k¹t obrotu wokó³ osi x dla trybu niezale¿nego od punktu obserwacji
static GLfloat pix2angle;  // wspó³czynnik przeliczeniowy pikseli na k¹ty
static GLint status = 0;  // status interakcji mysz¹
static int x_pos_old = 0;  // poprzednia pozycja myszy w osi x
static int delta_x = 0;  // ró¿nica pozycji myszy w osi x
static int y_pos_old = 0;  // poprzednia pozycja myszy w osi y
static int delta_y = 0;  // ró¿nica pozycji myszy w osi y
static int z_pos_old = 0;  // poprzednia pozycja myszy w osi z
static int delta_z = 0;  // ró¿nica pozycji myszy w osi z

// Funkcja obliczaj¹ca wspó³rzêdn¹ x sceny na podstawie azymutu i elewacji
float scenaX(float R, float azymut, float elewacja)
{
    return R * cos(azymut) * (float)cos(elewacja);
}

// Funkcja obliczaj¹ca wspó³rzêdn¹ y sceny na podstawie elewacji
float scenaY(float R, float elewacja)
{
    return R * sin(elewacja);
}

// Funkcja obliczaj¹ca wspó³rzêdn¹ z sceny na podstawie azymutu i elewacji
float scenaZ(float R, float azymut, float elewacja)
{
    return R * sin(azymut) * (float)cos(elewacja);
}

// Funkcja obs³uguj¹ca zdarzenia myszy
void Mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        x_pos_old = x;
        y_pos_old = y;
        status = 1;
    }
    else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        y_pos_old = y;
        status = 2;
    }
    else
        status = 0;
}

// Funkcja obs³uguj¹ca ruch myszy
void Motion(GLsizei x, GLsizei y)
{
    delta_x = x - x_pos_old;
    x_pos_old = x;
    delta_y = y - y_pos_old;
    y_pos_old = y;
    glutPostRedisplay();
}

// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(void)
{
    point3 x_min = { -5.0, 0.0, 0.0 };
    point3 x_max = { 5.0, 0.0, 0.0 };
    point3 y_min = { 0.0, -5.0, 0.0 };
    point3 y_max = { 0.0,  5.0, 0.0 };
    point3 z_min = { 0.0, 0.0, -5.0 };
    point3 z_max = { 0.0, 0.0,  5.0 };

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3fv(x_min);
    glVertex3fv(x_max);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3fv(y_min);
    glVertex3fv(y_max);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3fv(z_min);
    glVertex3fv(z_max);
    glEnd();
}

// Funkcja rysuj¹ca jajko
void Egg(int N)
{
    glColor3f(0.0f, 1.0f, 0.0f);
    point3* points = new point3[N * N];
    double u, v;
    int id = 0;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            u = i / ((double)(N - 1) * 2.0);
            v = j / ((double)N / 2.0);
            points[id][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
            points[id][1] = (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5);
            points[id][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);
            id++;
        }

    if (model == 1)
    {
        glBegin(GL_POINTS);
        for (int id = 0; id < N * N; id++)
            glVertex3d(points[id][0], points[id][1], points[id][2]);
        glEnd();
    }
    if (model == 2)
    {
        for (int i = 0; i < N * N; i += N)
        {
            for (int j = i; j < i + N - 1; j++)
            {
                glBegin(GL_LINES);
                glVertex3d(points[j][0], points[j][1], points[j][2]);
                glVertex3d(points[j + 1][0], points[j + 1][1], points[j + 1][2]);
                glEnd();
            }
            glBegin(GL_LINES);
            glVertex3d(points[i + N - 1][0], points[i + N - 1][1], points[i + N - 1][2]);
            glVertex3d(points[i][0], points[i][1], points[i][2]);
            glEnd();
        }

        for (int i = 0; i < N; i++)
        {
            for (int j = i; j < N * N - N; j += N)
            {
                glBegin(GL_LINES);
                glVertex3d(points[j][0], points[j][1], points[j][2]);
                glVertex3d(points[j + N][0], points[j + N][1], points[j + N][2]);
                glEnd();
            }
        }
    }
    if (model == 3)
    {
        int a, b, c, d;
        for (int i = 0; i < N; i++)
        {
            for (int j = i; j < N * N - N; j += N)
            {
                a = j;
                b = j + N;
                c = (j + 1) % (N * N);
                d = (j + 1) % (N * N) + N;

                glBegin(GL_TRIANGLES);
                glColor3d(colours[a][0], colours[a][1], colours[a][2]);
                glVertex3d(points[a][0], points[a][1], points[a][2]);
                glColor3d(colours[b][0], colours[b][1], colours[b][2]);
                glVertex3d(points[b][0], points[b][1], points[b][2]);
                glColor3d(colours[c][0], colours[c][1], colours[c][2]);
                glVertex3d(points[c][0], points[c][1], points[c][2]);
                glEnd();

                glBegin(GL_TRIANGLES);
                glColor3d(colours[b][0], colours[b][1], colours[b][2]);
                glVertex3d(points[b][0], points[b][1], points[b][2]);
                glColor3d(colours[c][0], colours[c][1], colours[c][2]);
                glVertex3d(points[c][0], points[c][1], points[c][2]);
                glColor3d(colours[d][0], colours[d][1], colours[d][2]);
                glVertex3d(points[d][0], points[d][1], points[d][2]);
                glEnd();
            }
        }
    }
    delete[] points;

    if (model == 4)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glutWireTeapot(3.0);
    }
}

// Funkcja rysuj¹ca scenê
void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(viewer[0], viewer[1], viewer[2], punkt_obserwacji[0], punkt_obserwacji[1], punkt_obserwacji[2], 0.0, 1.0, 0.0);
    Axes();

    if (status == 1 && tryb)
    {
        theta_x += delta_x * pix2angle;
        theta_y += delta_y * pix2angle;
    }
    else if (status == 2 && tryb)
    {
        if (delta_y > 0)
        {
            viewer[2] += 0.1;
            punkt_obserwacji[2] += 0.25;
        }
        else
        {
            viewer[2] -= 0.1;
            punkt_obserwacji[2] -= 0.25;
        }
    }
    else if (status == 1 && !tryb)
    {
        theta_x1 += (delta_x * pix2angle / 100.0f);
        theta_y1 += (delta_y * pix2angle / 100.0f);
        viewer[0] = scenaX(R, theta_x1, theta_y1);
        viewer[1] = scenaY(R, theta_y1);
        viewer[2] = scenaZ(R, theta_x1, theta_y1);
    }
    else if (status == 2 && !tryb)
    {
        R += (delta_y * pix2angle) / 25.0f;
        if (R < 1.0f) R = 1.0f;
        viewer[0] = scenaX(R, theta_x1, theta_y1);
        viewer[1] = scenaY(R, theta_y1);
        viewer[2] = scenaZ(R, theta_x1, theta_y1);
    }

    glRotatef(theta_x, 0.0, 1.0, 0.0);
    glRotatef(theta_y, 1.0, 0.0, 0.0);
    glColor3f(1.0f, 1.0f, 1.0f);
    Egg(N);
    glutSwapBuffers();
}

// Funkcja obs³uguj¹ca klawisze
void keys(unsigned char key, int x, int y)
{
    if (key == 'p') model = 1;
    if (key == 'w') model = 2;
    if (key == 's') model = 3;
    if (key == 't') model = 4;
    if (key == 32) tryb = !tryb;
    RenderScene();
}


// Inicjalizacja t³a na czarny
void MyInit(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

// Funkcja zmieniaj¹ca rozmiar okna
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    // Wspó³czynnik przeliczeniowy pikseli na k¹ty dla ruchu mysz¹
    pix2angle = 360.0 / (float)horizontal;

    // Ustawienia rzutu perspektywicznego
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, 1.0, 1.0, 30.0);

    // Dopasowanie widoku do rozmiaru okna
    if (horizontal <= vertical)
        glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
    else
        glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Funkcja obs³uguj¹ca menu
void menu()
{
    cout << "Podaj wartosc liczby N: ";
    cin >> N;
    while (N < 0)
    {
        cout << "Podaj wartosc wieksza niz 0 ";
        cin >> N;
    }

    // Wyœwietlanie opcji w menu
    cout << " P: Jajko punktowe " << endl
         << " W: Jajko siatkowe" << endl
         << " S: Jajko trojkatne wielokolorowe" << endl
         << " T: Gotowy model czajnika" << endl
         << " Spacja: Zmiana obrotu";
    cout << endl;
}

// Funkcja g³ówna programu
int main(int argc, char* argv[])
{
    // Inicjalizacja GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInit(&argc, argv);
    glutCreateWindow("Rzutowanie perspektywiczne");

    // Wywo³anie menu
    menu();

    // Inicjalizacja kolorów
    colours = new point3[N * N];
    srand(time(NULL));
    for (int i = 0; i < N * N; i++)
    {
        colours[i][0] = (rand() % 101) * 0.01;
        colours[i][1] = (rand() % 101) * 0.01;
        colours[i][2] = (rand() % 101) * 0.01;
    }

    // Kopiowanie kolorów dla obramowania jajka
    for (int i = 0; i < N; i++)
    {
        colours[i][0] = colours[0][0];
        colours[i][1] = colours[0][1];
        colours[i][2] = colours[0][2];
        colours[N * N - 1 - i][0] = colours[N * N - 1][0];
        colours[N * N - 1 - i][1] = colours[N * N - 1][1];
        colours[N * N - 1 - i][2] = colours[N * N - 1][2];
    }

    // Ustawienia funkcji zwrotnej dla GLUT
    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    MyInit();

    // W³¹czenie obs³ugi g³êbokoœci
    glEnable(GL_DEPTH_TEST);

    // Ustawienia funkcji zwrotnej dla obs³ugi klawiatury
    glutKeyboardFunc(keys);

    // Uruchomienie g³ównej pêtli GLUT
    glutMainLoop();

    // Zwolnienie pamiêci po tablicy kolorów
    delete[] colours;

    return 0;
}

