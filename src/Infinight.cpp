#include "Infinight.hpp"

#include "plugin.hpp"

using namespace rack;

Infinight::Infinight()
{
	config( NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS );
	configParam( PITCH_PARAM, 0.f, 1.f, 0.f, "" );	
	
}

void Infinight::process(const ProcessArgs& args)
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
				mm2px( Vec( 7.00, 20.00 ) ), module, Infinight::L_INPUT ) );
		addInput( createInputCentered<PJ301MPort>(
				mm2px( Vec( 7.00, 32.00 ) ), module, Infinight::R_INPUT ) );
		addInput( createInputCentered<PJ301MPort>(
				mm2px( Vec( 7.00, 92.00 ) ), module, Infinight::L_LOOP_INPUT ) );
		addInput( createInputCentered<PJ301MPort>(
				mm2px( Vec( 7.00, 104.00 ) ), module, Infinight::R_LOOP_INPUT ) );

		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 20.00 ) ), module, Infinight::L_OUTPUT ) );
		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 32.00 ) ), module, Infinight::R_OUTPUT ) );
		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 92.00 ) ), module, Infinight::L_LOOP_OUTPUT ) );
		addOutput( createOutputCentered<PJ301MPort>(
				mm2px( Vec( 24.00, 104.00 ) ), module, Infinight::R_LOOP_OUTPUT ) );
}
