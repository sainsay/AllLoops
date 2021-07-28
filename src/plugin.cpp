#include "plugin.hpp"

#include "Infinight.hpp"

using namespace rack;

Plugin* pluginInstance;

Model* InfinightModel;

void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	InfinightModel = createModel<Infinight, InfinightWidget>( "Infinight" );
	p->addModel( InfinightModel );

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
