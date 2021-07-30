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
	enum ParamIds
	{
		MIX_PARAM,
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
		REVERSE_LIGHT,
		NUM_LIGHTS
	};

    std::array<sain::Looper, 4> Loopers = {	sain::Looper(2.5f), 
											sain::Looper(2.3f),
											sain::Looper(2.1f), 
											sain::Looper(1.9f)};

	rack::dsp::BooleanTrigger ArmTrigger;
	rack::dsp::BooleanTrigger ReverseTrigger;

	InfinightFlags Flags = InfinightFlags::STATE_OK;

	bool ArmState = true;

	Infinight( );

	void process( const ProcessArgs &args ) override;

	void onSampleRateChange() override;

	void onReset() override;

	void onRandomize() override;

};

struct InfinightWidget : rack::ModuleWidget
{
	InfinightWidget( Infinight* module );
};

