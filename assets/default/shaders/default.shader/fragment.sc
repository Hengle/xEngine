$input v_color0, v_texcoord0

#include "shaderlib.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
	vec2 uv = v_texcoord0;

	uv.y = 1.0 - uv.y;

    gl_FragColor = texture2D(s_texColor, uv);

	if (gl_FragColor.a * v_color0.a < 0.1) discard;

	gl_FragColor = gl_FragColor * v_color0;
}
