#ifndef WIDGET_H
#define WIDGET_H


#include <QWidget>
#include <QGLWidget>
#include <QVector>
#include <QVector3D>
#include <QFile>
#include <QTextStream>
#include <QGLFunctions>
#include <QDebug>

#include <QTimer>
#include <QTime>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGLShader>
#include <QGLShaderProgram>
#include <cmath>

namespace Ui {
    class Widget;
}

struct Vertex {
    QVector3D p;
    QVector3D n;
    Vertex() {}
    Vertex(const Vertex& v): p (v.p), n (v.n) {}
    Vertex(const QVector3D& pp, const QVector3D& nn): p (pp), n (nn) {}
    Vertex (float x, float y, float z) : p (x, y, z) {}

};

struct SubMesh
{
    QVector<Vertex> vertices;
    QString texture;
    void loadRawTriangles(QString fileName)
    {
        QFile file (fileName);
        QVector<float> floats;
        if (!file.open (QIODevice::ReadOnly))
            throw QString ("Can't open file ") + fileName;

        QTextStream in (&file);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList values = line.split (" ", QString::SkipEmptyParts);
            if (values.empty())
                continue;
            else if (values[0] == "#")
                continue;
            else if (values.size() == 9)
            {
                QTextStream stream (&line);
                for (int i = 0; i < 9; ++i)
                {
                    float f;
                    stream >> f;
                    floats.push_back (f);
                }
                QVector3D A(floats[0], floats[1], floats[2]);
                QVector3D B(floats[3], floats[4], floats[5]);
                QVector3D C(floats[6], floats[7], floats[8]);

                QVector3D N = QVector3D::crossProduct ((A-B),(A-C)).normalized();

                vertices.push_back (Vertex (A,N));
                vertices.push_back (Vertex (B,N));
                vertices.push_back (Vertex (C,N));
                floats.clear();

            } else qDebug() << values.size() << "Warning: bad line " << line;
        }
    }

    void draw(QGLFunctions* funcs, QGLShaderProgram *prog)
    {
        int qt_vertex = prog->attributeLocation ("qt_Vertex");
        int qt_normal = prog->attributeLocation ("qt_Normal");
        funcs->glEnableVertexAttribArray (qt_vertex);
        funcs->glEnableVertexAttribArray (qt_normal);
        funcs->glVertexAttribPointer (qt_vertex, 3, GL_FLOAT, false, 6*sizeof(float), &vertices[0].p);
        funcs->glVertexAttribPointer (qt_normal, 3, GL_FLOAT, false, 6*sizeof(float), &vertices[0].n);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
};

struct Mesh
{
    QVector<SubMesh> submeshes;
    QVector<QString> textures;
};

inline float clamp(float x, float a, float b)
{
    if (x < a) return a;
    if (x > b) return b;
    return x;
}

class Widget : public QGLWidget, public QGLFunctions
{
    Q_OBJECT
public:
    explicit Widget (QWidget *parent = 0);
    ~Widget();
    void initializeGL();
    void resizeGL(int w, int h) { glViewport (0, 0, w, h); }
    void paintGL();

    void keyPressEvent (QKeyEvent *e);
    void keyReleaseEvent (QKeyEvent *e);

    void captMouse();
    void move();
    void mousePressEvent(QMouseEvent *);
private:
    Ui::Widget *ui;

    SubMesh terrainMesh;
    QGLShaderProgram myShader, particle;
    int terrainTexture, partTexture;
    QVector3D pos;
    float yaw, pitch;

    bool u, d, l, r, f, b, down, jmp;
    float vel_y, accel_y;

    QTimer timer;
    bool capture;

    int frame;
    QVector3D partPos;
    double sz;

public slots:
    void onTimer();
    void posX_changed (double);
    void posY_changed (double);
    void posZ_changed (double);
 };

#endif // WIDGET_H
