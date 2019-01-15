#version 410 core

layout(points) in;
layout(triangle_strip, max_vertices = 6) out;
uniform mat4 P;
uniform mat4 V;
uniform mat4 VRi;
uniform float bScale;
out vec2 texCoord;



void main()
{
    /*
	gl_Position = P * V * (gl_in[0].gl_Position + vec4(0,0,0,0));
	texCoord=vec2(0,0);
	EmitVertex();
    gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0,0.1,0,0));
	texCoord=vec2(0,1);
	EmitVertex();
	gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0.1,0.1,0,0));
	texCoord=vec2(1,1);
	EmitVertex();
	gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0,0,0,0));
	texCoord=vec2(0,0);
	EmitVertex();
    gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0.1,0.1,0,0));
	texCoord=vec2(1,1);
	EmitVertex();
	gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0.1,0,0,0));
	texCoord=vec2(1,0);
	EmitVertex();

    EndPrimitive();
    */
    
    //***TODO: When Scale is added, obj doesnt show up ***
    
    gl_Position = P * V * (gl_in[0].gl_Position + vec4(0,0,0,0));
    texCoord=vec2(0,0);
    EmitVertex();
    gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0,0.05 * bScale,0,0));
    texCoord=vec2(0,1);
    EmitVertex();
    gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0.05 * bScale, 0.05 * bScale,0,0));
    texCoord=vec2(1,1);
    EmitVertex();
    gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0,0,0,0));
    texCoord=vec2(0,0);
    EmitVertex();
    gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0.05 * bScale,0.05 * bScale,0,0));
    texCoord=vec2(1,1);
    EmitVertex();
    gl_Position = P * V * (gl_in[0].gl_Position + VRi * vec4(0.05 * bScale, 0, 0,0));
    texCoord=vec2(1,0);
    EmitVertex();
    
    EndPrimitive();
    
}
