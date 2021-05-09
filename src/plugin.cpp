#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;

    // GENRACK: add modules here
    p->addModel(modelGigaverb);
}