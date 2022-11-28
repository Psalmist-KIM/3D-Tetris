#version 430

// declaration of attribute variables
in vec4 vPosition;
in vec4 vColor;
out vec4 fColor;

// set a uniform variable and its location statically
layout(location=1) uniform mat4 T;
layout(location=2) uniform int mode;
layout(location=3) uniform vec4 uColor;
layout(location=4) uniform mat4 M;
uniform mat4 P;
uniform mat4 V;

void main()
{
	gl_Position = P * V * M * T * vPosition;
	switch(mode)
	{
	case 0:
		fColor = vColor;
		break;

	case 1:
		fColor = uColor;
		break;
	}
}