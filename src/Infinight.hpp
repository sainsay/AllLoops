#pragma once
#include <rack.hpp>

#include "Util/Looper.hpp"

enum class InfinightFlags : int8_t 
{
	STATE_OK,
	SAMPLERATE_CHANGED
};

DEF_FLAG_OR_OP( InfinightFlags, int8_t )
DEF_FLAG_AND_OP( InfinightFlags, int8_t )
DEF_FLAG_ASSIGN_OR_OP( InfinightFlags, int8_t)
DEF_FLAG_ASSIGN_AND_OP( InfinightFlags, int8_t)
DEF_FLAG_NOT_OP( InfinightFlags, int8_t)

struct Infinight : rack::Module
{
	static const size_t LooperCount = 4;

	enum ParamIds
	{
		MIX_PARAM,
		IN_PARAM,
		OUT_PARAM,
		REVERSE_BTN_PARAM,
		ARM_BTN_PARAM,
		NUM_PARAMS
	};

	enum InputIds
	{
		T1_INPUT,
		T2_INPUT,
		T1_RETURN,
		T2_RETURN,
		NUM_INPUTS
	};

	enum OutputIds
	{
		T1_OUTPUT,
		T2_OUTPUT,
		T1_SEND,
		T2_SEND,
		NUM_OUTPUTS
	};

	enum LightIds
	{
		ARM_LIGHT,
		NUM_LIGHTS
	};

    std::array<sain::Looper, LooperCount> Loopers = {	
		sain::Looper( 2.48f ), 
		sain::Looper( 2.3f ),
		sain::Looper( 2.13f ), 
		sain::Looper( 1.91f ) };

	rack::dsp::BooleanTrigger ArmTrigger;
	rack::dsp::BooleanTrigger ReverseTrigger;

	rack::dsp::SlewLimiter ArmSlew;

	InfinightFlags Flags = InfinightFlags::STATE_OK;

	float samplerate;

	bool ArmState = false;
	bool PolyOut = false;
	bool Reverse = false;

	Infinight( );

	void process( const ProcessArgs &args ) override;

	void onSampleRateChange() override;

	void onReset() override;

	void onRandomize() override;

};

struct InfinightWidget : rack::ModuleWidget
{
	InfinightWidget( Infinight* module );

	void appendContextMenu(rack::Menu* menu) final;

};

