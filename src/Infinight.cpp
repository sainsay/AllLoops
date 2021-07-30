#include "Infinight.hpp"

#include "plugin.hpp"

using namespace rack;



Infinight::Infinight()
{
	config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );
	configParam( MIX_PARAM, 0.f, 1.f, 0.5f, "Mix", "%", 0, 100 );
	configParam( REVERSE_BTN_PARAM, 0.0f, 1.0f, 0.0f, "Reverse" );	
	configParam( ARM_BTN_PARAM, 0.0f, 1.0f, 0.0f, "Arm/Disarm" );
	
	// Samplerate unknown. assume samplerate changed.
	SET_FLAG(Flags, InfinightFlags::SAMPLERATE_CHANGED);
}

void Infinight::process(const ProcessArgs& args)
{
	if (FLAG_IS_SET(Flags, InfinightFlags::SAMPLERATE_CHANGED))
	{
		for(auto& looper : Loopers)
		{
			looper.SetSampleRate( args.sampleRate );
		}
		CLEAR_FLAG(Flags, InfinightFlags::SAMPLERATE_CHANGED);
	}
	
	if(ArmTrigger.process(params[ARM_BTN_PARAM].getValue() > 0.0f) )
	{
		ArmState = !ArmState;
	}

	if (ReverseTrigger.process(params[REVERSE_BTN_PARAM].getValue() > 0.0f))
	{
		for(auto& looper: Loopers){
			looper.Reverse();
		}
	}

	float T1_InputSample = inputs[T1_INPUT].getVoltage();
	float T2_InputSample = inputs[T2_INPUT].getVoltage();

	sain::Looper::Sample wet;
	uint32_t ChannelNumber = 0;

	for(auto& looper : Loopers)
	{
		sain::Looper::Sample ToWrite;
		sain::Looper::Sample ToSend;

		ToSend += looper.ReadSample();
		wet += ToSend;

		outputs[T1_SEND].setVoltage(ToSend.T1, ChannelNumber);
		outputs[T2_SEND].setVoltage(ToSend.T2, ChannelNumber);

		ToWrite.T1 = inputs[T1_RETURN].isConnected() ? inputs[T1_RETURN].getPolyVoltage(ChannelNumber) : looper.ReadSample().T1;
		ToWrite.T2 = inputs[T2_RETURN].isConnected() ? inputs[T1_RETURN].getPolyVoltage(ChannelNumber) : looper.ReadSample().T2;

		//TODO: explore more complex algorithm sidechain compression or simple crossfade?
		// The Idea is to surpress current content to allow for new content to be introduced
		ToWrite += { T1_InputSample, T2_InputSample };
		
		looper.WriteSample( ToWrite  );
		
		looper.Process();

		++ChannelNumber;
	}

	float mix = params[MIX_PARAM].getValue();
	mix = clamp(mix, 0.f, 1.f);
	float T1_out = crossfade(T1_InputSample, wet.T1, mix);
	float T2_out = crossfade(T2_InputSample, wet.T2, mix);
	
	outputs[T1_OUTPUT].setVoltage(T1_out);
	outputs[T2_OUTPUT].setVoltage(T2_out);

	outputs[T1_SEND].setChannels(Loopers.size());
	outputs[T2_SEND].setChannels(Loopers.size());
}

void Infinight::onSampleRateChange()
{
	SET_FLAG(Flags, InfinightFlags::SAMPLERATE_CHANGED);
}

void Infinight::onReset()
{

}

void Infinight::onRandomize()
{

}

InfinightWidget::InfinightWidget(Infinight* module)
{
	setModule( module );
		setPanel( APP->window->loadSvg(
				asset::plugin( pluginInstance, "res/Infinight.svg" ) ) );

		addChild( createWidget<ScrewSilver>( 
				Vec( RACK_GRID_WIDTH, 0 ) ) );
		addChild( createWidget<ScrewSilver>(
				Vec( box.size.x - 2 * RACK_GRID_WIDTH, 0 ) ) );
		addChild( createWidget<ScrewSilver>(
				Vec( RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH ) ) );
		addChild( createWidget<ScrewSilver>(
				Vec( box.size.x - 2 * RACK_GRID_WIDTH,
						 RACK_GRID_HEIGHT - RACK_GRID_WIDTH ) ) );

		addInput( createInputCentered<PJ301MPort>(
				mm2px( Vec( 7.00, 20.00 ) ), module, Infinight::T1_INPUT ) );
		addInput( createInputCentered<PJ301MPort>(
				mm2px( Vec( 7.00, 32.00 ) ), module, Infinight::T2_INPUT ) );
		addInput( createInputCentered<PJ301MPort>(
				mm2px( Vec( 7.00, 92.00 ) ), module, Infinight::T1_RETURN ) );
		addInput( createInputCentered<PJ301MPort>(
				mm2px( Vec( 7.00, 104.00 ) ), module, Infinight::T2_RETURN ) );

		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 20.00 ) ), module, Infinight::T1_OUTPUT ) );
		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 32.00 ) ), module, Infinight::T2_OUTPUT ) );
		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 92.00 ) ), module, Infinight::T1_SEND ) );
		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 104.00 ) ), module, Infinight::T2_SEND ) );

		//addParam( createParamCentered<>())
}
