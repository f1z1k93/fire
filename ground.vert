attribute highp vec3 qt_Vertex;
attribute highp vec3 qt_Normal;
varying highp vec3 position;
const highp vec3 lightdir = vec3(0.5, 0.5, 3.0);
varying highp float lightcolor;


void main(void)
{
    position = qt_Vertex.xyz;
    lightcolor = dot(normalize(qt_Normal), normalize(lightdir));
    gl_Position = gl_ModelViewProjectionMatrix*vec4(qt_Vertex,1.0);
}
