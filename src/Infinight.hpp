#pragma once
#include <rack.hpp>

#include "Util/Looper.hpp"

struct Infinight : rack::Module
{
	enum ParamIds
	{
		PITCH_PARAM,
		NUM_PARAMS
	};
	enum InputIds
	{
		L_INPUT,
		R_INPUT,
		L_LOOP_INPUT,
		R_LOOP_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
		L_OUTPUT,
		R_OUTPUT,
		L_LOOP_OUTPUT,
		R_LOOP_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		BLINK_LIGHT,
		NUM_LIGHTS
	};

    std::array<sain::Looper, 4> Loopers;

	Infinight( );
	void process( const ProcessArgs &args ) override;
};

struct InfinightWidget : rack::ModuleWidget
{
	InfinightWidget( Infinight* module );
};

