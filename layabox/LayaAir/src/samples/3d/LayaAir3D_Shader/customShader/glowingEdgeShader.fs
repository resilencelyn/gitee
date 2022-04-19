#if defined(GL_FRAGMENT_PRECISION_HIGH)// 原来的写法会被我们自己的解析流程处理，而我们的解析是不认内置宏的，导致被删掉，所以改成 if defined 了
	precision highp float;
	precision highp int;
#else
	precision mediump float;
	precision mediump int;
#endif

#include "Lighting.glsl";

varying vec2 v_Texcoord;
uniform sampler2D u_texture;
uniform vec3 u_marginalColor;

varying vec3 v_Normal;

uniform DirectionLight u_SunLight;
uniform vec3 u_CameraPos;
varying vec3 v_PositionWorld;

void main()
{

	
	gl_FragColor=texture2D(u_texture,v_Texcoord);
	
	vec3 normal=normalize(v_Normal);
	vec3 toEyeDir = normalize(u_CameraPos-v_PositionWorld);
	float Rim = 1.0 - max(0.0,dot(toEyeDir, normal));

	vec3 lightColor = u_SunLight.color;
	vec3 Emissive = 2.0 * lightColor * u_marginalColor * pow(Rim,3.0);  
	
	gl_FragColor = texture2D(u_texture, v_Texcoord) + vec4(Emissive,1.0);
}

