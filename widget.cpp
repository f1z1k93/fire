#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "widget.h"
#include "ui_widget.h"
#include <stdio.h>

Widget::Widget (QWidget *parent): QGLWidget (parent), ui (new Ui::Widget)
{
    ui->setupUi(this);
    u = d = l = r = f = b = down = jmp = false;
    vel_y = 0, accel_y = 0.4;
    yaw = pitch = 0;
    pos = QVector3D (-3, -4, 5);
    partPos = QVector3D (15, 15, 2);
    frame = 0;
    setCursor (QCursor (Qt::BlankCursor));
    capture = true;

    sz = 0.5;
}

Widget::~Widget() {
    delete ui;
}

void Widget::initializeGL()
{
    timer.setInterval (10);
    timer.setSingleShot(false);
    timer.start();
    connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    initializeGLFunctions();

    printf("\n%s\n", glGetString(GL_VERSION));
    printf("%s\n", glGetString(GL_VENDOR));
    printf("%s\n", glGetString(GL_RENDERER));
    fflush (stdout);
    QImage part_img (":/match.png");
    for (int line = 0; line < part_img.height(); line++)
    {
        QRgb *pixel = (QRgb *)part_img.scanLine (line);
        for (int b = 0; b < part_img.width(); b++)
            if( 0 == qRed(pixel[b]) &&
                0 == qGreen(pixel[b]) &&
                0 == qBlue(pixel[b])
            ) {
                pixel[b] = 0x00ffffff;
            }
    }

    partTexture = bindTexture (part_img,
                         GL_TEXTURE_2D,
                         GL_RGBA,
                         QGLContext::DefaultBindOption);

    myShader.addShaderFromSourceFile(QGLShader::Vertex, ":/ground.vert");
    myShader.addShaderFromSourceFile(QGLShader::Fragment, ":/ground.frag");
    myShader.link();
    particle.addShaderFromSourceFile(QGLShader::Vertex,":/particle.vert");
    particle.addShaderFromSourceFile(QGLShader::Fragment,":/particle.frag");
    particle.link();

    terrainMesh.loadRawTriangles(":/terrain.raw");

    terrainTexture =bindTexture(QImage(":/terrain.jpg"));
}

void Widget::captMouse()
{
    if (isActiveWindow() and capture)
    {
        int a = parentWidget()->x() + width()/2 + 9;
        int b = parentWidget()->y() + height()/2 + 9;
        float dx = a - cursor().pos().x();
        float dy = b - cursor().pos().y();
        yaw += dx*0.01;
        pitch += dy*0.01;
        clamp (pitch, -M_PI/2, M_PI/2);
        cursor().setPos(a, b);
    }
}

void Widget::move()
{
    if (f) pos += QVector3D (cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), 0);
    if (b) pos -= QVector3D (cos(yaw)*cos(pitch), sin(yaw)*cos(pitch), 0);
    if (l) pos -= QVector3D (sin(yaw), -cos(yaw), 0);
    if (r) pos += QVector3D (sin(yaw), -cos(yaw), 0);
    if (u)
        if (!jmp)
        {
            vel_y = 3;
            jmp = true;
        }
    if (jmp)
    {
        vel_y -= accel_y;
        if (pos.z() + vel_y > 5)
            pos += QVector3D (0, 0, vel_y);
        else
        {
            vel_y = 0;
            jmp = false;
            pos.setZ(5);
        }
    }

    if (pos.z() <= 5 and not down)
        pos.setZ(5);
    if (d)
    {
        if (!down)
        {
            pos -= QVector3D (0, 0, 4.5);
            down = true;
        }
    } else down = false;
}

void Widget::paintGL()
{
    captMouse();
    move();

    frame++;
    glClearColor (0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1.0 * width()/height(), 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(pos.x(),pos.y(),pos.z(),
              pos.x()+cos(yaw)*cos(pitch),
              pos.y()+sin(yaw)*cos(pitch),
              pos.z()+sin(pitch),
              0,0,1);
    glEnable(GL_DEPTH_TEST);

    myShader.bind();
    myShader.setUniformValue("sampler", 0);

    glBindTexture(GL_TEXTURE_2D, terrainTexture);
    terrainMesh.draw (this, &myShader);
    myShader.release();

    particle.bind();
    particle.setUniformValue("sampler", 0);
    glBindTexture(GL_TEXTURE_2D, partTexture);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    particle.setUniformValue("point_size", 2*(GLfloat)sz * width());

    glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor3f(1,1,1);
    particle.setUniformValue("time", (GLfloat)frame);
    particle.setUniformValue("center", partPos);
    glBegin(GL_POINTS);
    for (int a = 0; a < 100; a++)
        glVertex2i(a, 0);
    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    particle.release();
}

void Widget::mousePressEvent(QMouseEvent *)
{
    this->setFocus();
    setCursor (QCursor (Qt::BlankCursor));
    capture = true;
}

void Widget::keyPressEvent (QKeyEvent *e)
{
    if(e->key()==Qt::Key_Up || e->key()==Qt::Key_W)
        f = true;
    else if(e->key()==Qt::Key_Down || e->key()==Qt::Key_S)
        b = true;
    else if(e->key()==Qt::Key_Left || e->key()==Qt::Key_A)
        l = true;
    else if(e->key()==Qt::Key_Right || e->key()==Qt::Key_D)
        r = true;
    else if(e->key()==Qt::Key_Z)
    {
        setCursor (QCursor (Qt::ArrowCursor));
        this->parentWidget()->setFocus();
        capture = false;
    } else if (e->key() == Qt::Key_Space)
        u = true;
    else if (e->key() == Qt::Key_Control)
        d = true;
    else if (e->key() == Qt::Key_R)
        pos = QVector3D (-3, -4, 5);
    else if (e->key() == Qt::Key_Q)
        parentWidget()->close();
}

void Widget::keyReleaseEvent (QKeyEvent *e)
{
    if (e->key() == Qt::Key_Up or e->key() == Qt::Key_W)
        f = false;
    else if (e->key() == Qt::Key_Down or e->key() == Qt::Key_S)
        b = false;
    else if (e->key() == Qt::Key_Left or e->key() == Qt::Key_A)
        l = false;
    else if (e->key() == Qt::Key_Right or e->key() == Qt::Key_D)
        r = false;
    else if (e->key() == Qt::Key_Space)
        u = false;
    else if (e->key() == Qt::Key_Control)
        d = false;
}

void Widget::onTimer(){
    updateGL();
}

void Widget::posX_changed (double x)
{
    partPos.setX(x);
    updateGL();
}

void Widget::posY_changed (double y)
{
    partPos.setY(y);
    updateGL();
}

void Widget::posZ_changed (double z)
{
    partPos.setZ(z);
    updateGL();
}
