#version 410

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

// Uniforms
uniform sampler2D screen;  

// References
// https://www.shadertoy.com/view/slcSRM

vec2 hash(vec2 p) 
{ // https://compute.toys/view/15
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
	return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);
}

float noise(in vec2 p)
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor(p + (p.x + p.y) * K1);
    vec2  a = p - i + (i.x + i.y) * K2;
    float m = step(a.y, a.x); 
    vec2  o = vec2(m, 1.0 - m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0 * K2;
    vec3  h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c) ), 0.0);
	vec3  n = h * h * h * h * vec3(dot(a, hash(i + 0.0)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));
    return dot(n, vec3(70.0));
}

float simp(vec2 uv) 
{
    uv *= 5.0;
    mat2 m = mat2(1.6,  1.2, -1.2,  1.6);
	float f = 0.5000 * noise(uv); 
    uv = m * uv;
	f += 0.2500 * noise(uv); 
    uv = m * uv;
	f += 0.1250 * noise(uv); 
    uv = m * uv;
	f += 0.0625 * noise(uv); 
    uv = m * uv;
    f = 0.2 + 0.8 * f;
    return f;
}

vec4 bumpFromDepth(vec2 uv, vec2 resolution, float scale) 
{
  vec2 step = 1.0 / resolution;
    
  float height = simp(uv);
    
  vec2 dxy = height - vec2(
      simp(uv + vec2(step.x, 0.0)), 
      simp(uv + vec2(0.0, step.y))
  );
    
  return vec4(normalize(vec3(dxy * scale / step, 1.0)), height);
}

void main()
{
    vec2 screenRes = vec2(800.0, 600.0);
    // Normalized pixel coordinates (from 0 to 1)
    //vec2 uv = fragCoord/iResolution.xy;
    //vec4 offset = bumpFromDepth(uv + vec2(floor(iTime*4.0)/4.0), iResolution.xy, .1)/80.0;

    //fragColor = (texture(iChannel0, uv + offset.xy)*0.4) + (texture(iChannel0, uv)*0.6);
    //fragColor += length(bumpFromDepth(uv, iResolution.xy, .1))*0.25;

    vec4 offset = bumpFromDepth(vs_texcoord + vec2(floor(50) / 4.0), screenRes, 0.1) / 80.0;
    
    //vec3 color = texture(screen, vs_texcoord).rgb;
    //FragColor = vec4(color, 1.0);

    vec4 sample1 = texture(screen, vs_texcoord + offset.xy); 
    vec4 sample2 = texture(screen, vs_texcoord);

    vec3 object_color = vec3((sample1 * 0.4) + (sample2 * 0.6));

    FragColor = vec4(object_color, 1.0);// + length(bumpFromDepth(vs_texcoord, screenRes, 0.1) * 0.25);
}