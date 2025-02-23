#include "ProceduralSkybox2Shader.hpp"

namespace GFX
{
	//Source https://github.com/mrdoob/three.js/blob/dev/examples/jsm/objects/Sky.js
	static std::string vertexSource = R"(#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uSunPosition;
uniform float uRayleigh;
uniform float uTurbidity;
uniform float uMieCoefficient;
uniform vec3 uUp;

out vec3 oFragPosition;
out vec3 oSunDirection;
out float oSunfade;
out vec3 oBetaR;
out vec3 oBetaM;
out float oSunE;

// constants for atmospheric scattering
const float e = 2.71828182845904523536028747135266249775724709369995957;
const float pi = 3.141592653589793238462643383279502884197169;

// wavelength of used primaries, according to preetham
const vec3 lambda = vec3( 680E-9, 550E-9, 450E-9 );
// this pre-calculation replaces older TotaluRayleigh(vec3 lambda) function:
// (8.0 * pow(pi, 3.0) * pow(pow(n, 2.0) - 1.0, 2.0) * (6.0 + 3.0 * pn)) / (3.0 * N * pow(lambda, vec3(4.0)) * (6.0 - 7.0 * pn))
const vec3 totaluRayleigh = vec3( 5.804542996261093E-6, 1.3562911419845635E-5, 3.0265902468824876E-5 );

// mie stuff
// K coefficient for the primaries
const float v = 4.0;
const vec3 K = vec3( 0.686, 0.678, 0.666 );
// MieConst = pi * pow( ( 2.0 * pi ) / lambda, vec3( v - 2.0 ) ) * K
const vec3 MieConst = vec3( 1.8399918514433978E14, 2.7798023919660528E14, 4.0790479543861094E14 );

// earth shadow hack
// cutoffAngle = pi / 1.95;
const float cutoffAngle = 1.6110731556870734;
const float steepness = 1.5;
const float EE = 1000.0;

float sunIntensity( float zenithAngleCos ) {
	zenithAngleCos = clamp( zenithAngleCos, -1.0, 1.0 );
	return EE * max( 0.0, 1.0 - pow( e, -( ( cutoffAngle - acos( zenithAngleCos ) ) / steepness ) ) );
}

vec3 totalMie( float T ) {
	float c = ( 0.2 * T ) * 10E-18;
	return 0.434 * c * MieConst;
}

void main() {
    oFragPosition = vec3(uModel * vec4(aPosition, 1.0));
	gl_Position = uProjection * mat4(mat3(uView)) * vec4(aPosition, 1.0);
	gl_Position.z = gl_Position.w;

	oSunDirection = normalize( uSunPosition );

	oSunE = sunIntensity( dot( oSunDirection, uUp ) );

	oSunfade = 1.0 - clamp( 1.0 - exp( ( uSunPosition.y / 450000.0 ) ), 0.0, 1.0 );

	float uRayleighCoefficient = uRayleigh - ( 1.0 * ( 1.0 - oSunfade ) );

	// extinction (absorption + out scattering)
	// uRayleigh coefficients
	oBetaR = totaluRayleigh * uRayleighCoefficient;

	// mie coefficients
	oBetaM = totalMie( uTurbidity ) * uMieCoefficient;

})";

	static std::string fragmentSource = R"(#version 330 core

#include <Core>

uniform float uMieDirectionalG;
uniform vec3 uUp;
uniform float uExposure;

in vec3 oFragPosition;
in vec3 oSunDirection;
in float oSunfade;
in vec3 oBetaR;
in vec3 oBetaM;
in float oSunE;

out vec4 FragColor;

// constants for atmospheric scattering
const float pi = 3.141592653589793238462643383279502884197169;

const float n = 1.0003; // refractive index of air
const float N = 2.545E25; // number of molecules per unit volume for air at 288.15K and 1013mb (sea level -45 celsius)

// optical length at zenith for molecules
const float rayleighZenithLength = 8.4E3;
const float mieZenithLength = 1.25E3;
// 66 arc seconds -> degrees, and the cosine of that
const float sunAngularDiameterCos = 0.999956676946448443553574619906976478926848692873900859324;

// 3.0 / ( 16.0 * pi )
const float THREE_OVER_SIXTEENPI = 0.05968310365946075;
// 1.0 / ( 4.0 * pi )
const float ONE_OVER_FOURPI = 0.07957747154594767;

float rayleighPhase( float cosTheta ) {
	return THREE_OVER_SIXTEENPI * ( 1.0 + pow( cosTheta, 2.0 ) );
}

float hgPhase( float cosTheta, float g ) {
	float g2 = pow( g, 2.0 );
	float inverse = 1.0 / pow( 1.0 - 2.0 * g * cosTheta + g2, 1.5 );
	return ONE_OVER_FOURPI * ( ( 1.0 - g2 ) * inverse );
}

void main() {
	//vec3 direction = normalize( oFragPosition - uCamera.position.xyz );
	vec3 direction = normalize( oFragPosition  );

	// optical length
	// cutoff angle at 90 to avoid singularity in next formula.
	float zenithAngle = acos( max( 0.0, dot( uUp, direction ) ) );
	float inverse = 1.0 / ( cos( zenithAngle ) + 0.15 * pow( 93.885 - ( ( zenithAngle * 180.0 ) / pi ), -1.253 ) );
	float sR = rayleighZenithLength * inverse;
	float sM = mieZenithLength * inverse;

	// combined extinction factor
	vec3 Fex = exp( -( oBetaR * sR + oBetaM * sM ) );

	// in scattering
	float cosTheta = dot( direction, oSunDirection );

	float rPhase = rayleighPhase( cosTheta * 0.5 + 0.5 );
	vec3 betaRTheta = oBetaR * rPhase;

	float mPhase = hgPhase( cosTheta, uMieDirectionalG );
	vec3 betaMTheta = oBetaM * mPhase;

	vec3 Lin = pow( oSunE * ( ( betaRTheta + betaMTheta ) / ( oBetaR + oBetaM ) ) * ( 1.0 - Fex ), vec3( 1.5 ) );
	Lin *= mix( vec3( 1.0 ), pow( oSunE * ( ( betaRTheta + betaMTheta ) / ( oBetaR + oBetaM ) ) * Fex, vec3( 1.0 / 2.0 ) ), clamp( pow( 1.0 - dot( uUp, oSunDirection ), 5.0 ), 0.0, 1.0 ) );

	// nightsky
	float theta = acos( direction.y ); // elevation --> y-axis, [-pi/2, pi/2]
	float phi = atan( direction.z, direction.x ); // azimuth --> x-axis [-pi/2, pi/2]
	vec2 uv = vec2( phi, theta ) / vec2( 2.0 * pi, pi ) + vec2( 0.5, 0.0 );
	vec3 L0 = vec3( 0.1 ) * Fex;

	// composition + solar disc
	float sundisk = smoothstep( sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta );
	L0 += ( oSunE * 19000.0 * Fex ) * sundisk;

	vec3 texColor = ( Lin + L0 ) * 0.04 + vec3( 0.0, 0.0003, 0.00075 );

	vec3 retColor = pow( texColor, vec3( 1.0 / ( 1.2 + ( 1.2 * oSunfade ) ) ) );

	vec4 outputColor = vec4(retColor, 1.0);
	outputColor = tone_map_exposure(outputColor, uExposure);
	FragColor = gamma_correction(outputColor);
})";

	Shader ProceduralSkybox2Shader::Create()
	{
		return Shader(vertexSource, fragmentSource);
	}

	std::string ProceduralSkybox2Shader::GetVertexSource()
	{
		return vertexSource;
	}

	std::string ProceduralSkybox2Shader::GetFragmentSource()
	{
		return fragmentSource;
	}
}