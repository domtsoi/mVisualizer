#version 410 core
out vec4 color;
in vec2 texCoord;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform float morphT;

void main()
{
	color = (texture(tex, texCoord) * (1 + morphT)) + (texture(tex2, texCoord) * (1 - morphT)) ;
    //color = (texture(tex, texCoord) * sin(morphT)) + (texture(tex2, texCoord) * cos(morphT)/2) ;
    //color = texture(tex, texCoord);
    //color = texture(tex2, texCoord);
	//color.a=1;
	 
}
