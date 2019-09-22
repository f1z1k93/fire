uniform sampler2D sampler;
varying float alpha;


void main(void) {
    vec4 color = texture2D(sampler, gl_TexCoord[0].xy);
    gl_FragColor = vec4(color.xyz, color.a*alpha);
}
