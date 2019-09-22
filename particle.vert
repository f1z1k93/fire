varying float alpha;
uniform float point_size;
uniform vec3 center;
uniform float time;


float point_clamp (float d) {
    return clamp(d, 1.0, 500.0);
}
float atten (float d) {
    return 1.0/(0.001 + d*d);
}

vec3 calc_pos(vec3 initial, vec3 vel, float time) {
    return initial+vel*time + vec3(0.0, 0.0, 0.0)* time*time* 10.0;
}

vec3 random_vector (float param) {
    float res = sin(exp(param)) > 0 ? sin(exp(param)) : 1;
    return vec3(cos(param*param),sin(param*param),res);
}

void main(void)
{
    vec3 pos = random_vector(gl_Vertex.x)*0.1 + center;
    vec3 vel = random_vector(gl_Vertex.x*1.0)*1.0;

    float t = fract((time + gl_Vertex.x) / 100.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(calc_pos(pos, vel, t), 1.0);
    gl_PointSize = point_clamp(point_size * sqrt(atten(gl_Position.z))) * (1.0+t);
    alpha = 1.0 - t*t;

}
