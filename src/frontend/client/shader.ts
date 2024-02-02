export const vs = `
attribute vec2 a_texCoord;
varying vec2 v_texCoord;
void main(void) {
    v_texCoord = a_texCoord;
    gl_Position = vec4((a_texCoord.x * 2.0) - 1.0, 1.0 - (a_texCoord.y * 2.0), 0, 1);
}`

export const fs = `
precision mediump float;
uniform sampler2D sampler0;
varying vec2 v_texCoord;
void main(void) {
    gl_FragColor = vec4(
        texture2D(sampler0, v_texCoord).x,
        texture2D(sampler0, v_texCoord).y,
        texture2D(sampler0, v_texCoord).z,
        1.0
    );
}`