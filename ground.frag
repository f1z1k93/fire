varying highp vec3 position;
uniform sampler2D sampler;
varying highp float lightcolor;
const highp float gamma = 2.2;
const highp float invgamma = 1.0/2.2;

void main(void)
{
    highp vec3 diffColor = texture2D (sampler,position.xy).xyz;
    gl_FragColor = vec4(lightcolor*diffColor, 1.0);
}
