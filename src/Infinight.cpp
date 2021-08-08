#include "Infinight.hpp"

#include "plugin.hpp"

#include <functional>
#include <numeric>

using namespace rack;



Infinight::Infinight()
{
	config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );
	configParam( MIX_PARAM, 0.f, 1.f, 0.5f, "Mix", "%", 0, 100 );
	configParam( IN_PARAM, 0.f, 1.f, 1.0f, "Volume In", "%", 0, 100 );
	configParam( OUT_PARAM, 0.f, 1.f, 1.0f, "Volume Out", "%", 0, 100 );
	configParam( REVERSE_BTN_PARAM, 0.0f, 1.0f, 0.0f, "Reverse" );	
	configParam( ARM_BTN_PARAM, 0.0f, 1.0f, 0.0f, "Arm/Disarm" );
	
	ArmSlew.setRiseFall( 50.0f, 50.0f );

	// Samplerate unknown. assume samplerate changed.
	SET_FLAG(Flags, InfinightFlags::SAMPLERATE_CHANGED);
}

void Infinight::process(const ProcessArgs& args)
{
	if ( FLAG_IS_SET( Flags, InfinightFlags::SAMPLERATE_CHANGED ) )
	{
		samplerate = args.sampleRate;
		for( auto& looper : Loopers )
		{
			looper.SetSampleRate( args.sampleRate );
		}
		CLEAR_FLAG( Flags, InfinightFlags::SAMPLERATE_CHANGED );
	}
	
	if(ArmTrigger.process( params[ARM_BTN_PARAM].getValue() > 0.0f ) )
	{
		ArmState = !ArmState;
	}

	if ( ReverseTrigger.process( params[REVERSE_BTN_PARAM].getValue() > 0.0f ) )
	{
		for(auto& looper: Loopers){
			looper.Reverse();
		}
	}

	float T1_InputSample = inputs[T1_INPUT].getVoltage();
	float T2_InputSample = inputs[T2_INPUT].getVoltage();

	float inVolume = params[IN_PARAM].getValue();

	float armed = ArmSlew.process( args.sampleTime, ArmState ? 1.0f : 0.0f );

	T1_InputSample *= inVolume;
	T2_InputSample *= inVolume;

	sain::Looper::Sample dry{ T1_InputSample * armed, T2_InputSample * armed };
	

	uint32_t ChannelNumber = 0;

	std::array<sain::Looper::Sample, LooperCount> SendBuffer;
	SendBuffer.fill({});

	for( auto& looper : Loopers )
	{
		sain::Looper::Sample ToWrite;

		SendBuffer[ChannelNumber] = looper.ReadSample();

		outputs[T1_SEND].setVoltage(SendBuffer[ChannelNumber].T1, ChannelNumber);
		outputs[T2_SEND].setVoltage(SendBuffer[ChannelNumber].T2, ChannelNumber);

		ToWrite.T1 = inputs[T1_RETURN].isConnected() ? inputs[T1_RETURN].getPolyVoltage( ChannelNumber ) : looper.ReadSample().T1;
		ToWrite.T2 = inputs[T2_RETURN].isConnected() ? inputs[T2_RETURN].getPolyVoltage( ChannelNumber ) : looper.ReadSample().T2;

		//TODO: explore more complex algorithm sidechain compression or simple crossfade?
		// The Idea is to surpress current content to allow for new content to be introduced
		ToWrite += dry;
		
		looper.WriteSample( ToWrite  );
		
		looper.Process();

		++ChannelNumber;
	}

	float mix = params[MIX_PARAM].getValue();
	mix = clamp( mix, 0.f, 1.f );
	float outVolume = params[OUT_PARAM].getValue();

	if(PolyOut)
	{
		size_t i = 0;
		for( sain::Looper::Sample& wet : SendBuffer )
		{
			float T1_out = crossfade( T1_InputSample, wet.T1, mix );
			float T2_out = crossfade( T2_InputSample, wet.T2, mix );
			T1_out *= outVolume;
			T2_out *= outVolume;
			outputs[T1_OUTPUT].setVoltage( T1_out, i );
			outputs[T2_OUTPUT].setVoltage( T2_out, i );
			++i;
		}

		outputs[T1_OUTPUT].setChannels( LooperCount );
		outputs[T2_OUTPUT].setChannels( LooperCount );

	}
	else
	{
		sain::Looper::Sample wet;
		for( sain::Looper::Sample& sample : SendBuffer )
		{
			wet += sample;
		}

		float T1_out = crossfade( T1_InputSample, wet.T1, mix );
		float T2_out = crossfade( T2_InputSample, wet.T2, mix );
		T1_out *= outVolume;
		T2_out *= outVolume;
		outputs[T1_OUTPUT].setVoltage( T1_out );
		outputs[T2_OUTPUT].setVoltage( T2_out );
	}
	
	outputs[T1_SEND].setChannels( LooperCount );
	outputs[T2_SEND].setChannels( LooperCount );

	lights[ARM_LIGHT].setBrightness( armed );
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
	setPanel( APP->window->loadSvg( asset::plugin( pluginInstance, "res/Infinight.svg" ) ) );

	{ // screws
		addChild( createWidget<ScrewSilver>( 
			Vec( RACK_GRID_WIDTH, 0 ) ) );

		addChild( createWidget<ScrewSilver>( 
			Vec( box.size.x - 2 * RACK_GRID_WIDTH, 0 ) ) );

		addChild( createWidget<ScrewSilver>( 
			Vec( RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH ) ) );

		addChild( createWidget<ScrewSilver>( 
			Vec( box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH ) ) );
	}

	{ // Inputs
		auto AddInput = [this, module](rack::Vec pos, int id)
		{
			addInput( createInputCentered<PJ301MPort>( pos, module, id ) );
		};

		auto posInT1 = mm2px( Vec( 7.00, 33.80 ) );
		auto posInT2 = mm2px( Vec( 7.00, 45.85 ) );
		auto posReturnT1 = mm2px( Vec( 7.00, 96.90 ) );
		auto posReturnT2 = mm2px( Vec( 7.00, 108.90 ) );

		AddInput( posInT1, Infinight::T1_INPUT );
		AddInput( posInT2, Infinight::T2_INPUT );
		AddInput( posReturnT1, Infinight::T1_RETURN );
		AddInput( posReturnT2, Infinight::T2_RETURN );
	}

	{ // outputs
		auto AddOutput = [this, module](rack::Vec pos, int id)
		{
			addOutput( createOutputCentered<PJ301MPort>( pos, module, id ) );
		};

		auto posOutT1 = mm2px( Vec( 24.00, 33.80 ) );
		auto posOutT2 = mm2px( Vec( 24.00, 45.85 ) );
		auto posSendT1 = mm2px( Vec( 24.00, 96.90 ) );
		auto posSendT2 = mm2px( Vec( 24.00, 108.90 ) );

		AddOutput( posOutT1, Infinight::T1_OUTPUT );
		AddOutput( posOutT2, Infinight::T2_OUTPUT );
		AddOutput( posSendT1, Infinight::T1_SEND );
		AddOutput( posSendT2, Infinight::T2_SEND );
	}

	{ // params
		auto AddTrimpot = [this, module](rack::Vec pos, int id)
		{
			addParam( createParamCentered<Trimpot>( pos, module, id ) );
		};

		auto posParamIn = mm2px( Vec( 7.00, 22.40 ) );
		auto posParamOut =mm2px( Vec( 24.00, 22.40 ) );
		auto posParamMix = mm2px( Vec( 15.50, 18.80 ) );
		auto posParamArm = mm2px( Vec( 15.40, 72.10 ) );

		AddTrimpot( posParamIn, Infinight::IN_PARAM );
		AddTrimpot( posParamOut, Infinight::OUT_PARAM );
		AddTrimpot( posParamMix, Infinight::MIX_PARAM );
		
		addParam( createParamCentered<PB61303>(posParamArm, module, Infinight::ARM_BTN_PARAM ) );
	}

	{ // lights
		auto posLightArm = mm2px( Vec(15.40, 60.00 ) );

		addChild( createLightCentered<MediumLight<RedLight>>( posLightArm, module, Infinight::ARM_LIGHT ) );
	}

}

struct BoolMenuItem: MenuItem
{
	std::function<void()> Action;
	std::function<bool()> Check;

	BoolMenuItem( std::string label, std::function<void()> action, std::function<bool()> check ) :
		Action(action),
		Check(check)
	{
		text = label;
	}

	void onAction(const event::Action& a) override
	{
		Action();
	}

	void step() override
	{
		MenuItem::step();
		rightText = CHECKMARK(Check());
	}

};

void InfinightWidget::appendContextMenu(Menu* menu)
{
	auto* infinight = dynamic_cast<Infinight*>(module);

	menu->addChild( 
		new BoolMenuItem( 
			"Use polyphonic out", 
			[infinight](){ infinight->PolyOut = !infinight->PolyOut; }, 
			[infinight](){ return infinight->PolyOut; }) ); 

}

