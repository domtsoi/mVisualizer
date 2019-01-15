#version  410 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec4 vertPos2;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float morphT;

out vec3 pos;
//uniform float t;
void main()
{
    pos=vertPos.xyz;
    //gl_Position = M * vertPos;
    gl_Position = M * ((vertPos * morphT) + (vertPos2*(1-morphT)));
    //^^^Look at the mix function for interpolation aswell - Ask Austin Quick


}
