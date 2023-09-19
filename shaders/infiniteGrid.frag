#version 460 core

const float gridSize = 100000.f;
const float gridMinPixelsBetweenCells = 2.f;
const float gridCellSize = 2.5f;
const vec4 gridColorThick = vec4(1.f);
const vec4 gridColorThin = vec4(0.5f,0.5f,0.5f,1.f);

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 fragColor;

float log10(float x)
{
	return log(x) / log(10.f);
}

float satf(float x)
{
	return clamp(x, 0.f, 1.f);
}

vec2 satv(vec2 x)
{
	return clamp(x, vec2(0.f), vec2(1.f));
}

float max2(vec2 v)
{
	return max(v.x,v.y);
}

void main()
{
	vec2 dudv = vec2(
	length(vec2(dFdx(uv.x),dFdy(uv.x))),
	length(vec2(dFdx(uv.y),dFdy(uv.y)))
	);

	float lodLevel = max(0.f, log10((length(dudv) * gridMinPixelsBetweenCells) / gridCellSize) + 1.f);
	float lodFade = fract(lodLevel);

	float lod0 = gridCellSize * pow(10.f, floor(lodLevel + 0));
	float lod1 = gridCellSize * pow(10.f, floor(lodLevel + 1));
	float lod2 = gridCellSize * pow(10.f, floor(lodLevel + 2));

	dudv *= 4.f;

	float lod0a = max2(vec2(1.f) - abs(satv(mod(uv,lod0) / dudv) * 2.f - vec2(1.f)));
	float lod1a = max2(vec2(1.f) - abs(satv(mod(uv,lod1) / dudv) * 2.f - vec2(1.f)));
	float lod2a = max2(vec2(1.f) - abs(satv(mod(uv,lod2) / dudv) * 2.f - vec2(1.f)));

	vec4 c = lod2a > 0.f ? gridColorThick : lod1a > 0.f ? mix(gridColorThick, gridColorThin, lodFade) : gridColorThin;
	float opacityFalloff = (1.f - satf(length(uv) / gridSize));

	c.a *= lod2a > 0.f ? lod2a : lod1a > 0.f ? lod1a : (lod0a * (1.f - lodFade));
	c.a *= opacityFalloff;

	fragColor = c;
}