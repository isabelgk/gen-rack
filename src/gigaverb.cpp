#include "plugin.hpp"
#include "gigaverb.h"


/// Processing

struct Gigaverb : Module {
	CommonState *moduleState;
	t_sample **inputBuffers;  // access like: buffer[sample #][numInputs]
	t_sample **outputBuffers;

	int numParams;
	int numInputs;
	int numOutputs;


	Gigaverb() {
		// Set default sample rate of 44100 Hz and vector size 1 (VCV uses single sample processing)
		// and update it later if needed
		moduleState = (CommonState *)gigaverb::create(44100, 1);
		gigaverb::reset(moduleState);

		// Initialize sample buffers
		// TODO - do block sample processing
		allocBuffers(1);

		numParams = gigaverb::num_params();
		numInputs = gigaverb::num_inputs();
		numOutputs = gigaverb::num_outputs();

		// Configure parameters
		config(gigaverb::num_params(), numInputs, numOutputs, 0);
		for (int i = 0; i < gigaverb::num_params(); i++) {
			std::string name = std::string(gigaverb::getparametername(moduleState, i));
			std::string units = std::string(gigaverb::getparameterunits(moduleState, i));
			float min = 0.0;
			float max = 1.0;
			if (gigaverb::getparameterhasminmax(moduleState, i)) {
				min = gigaverb::getparametermin(moduleState, i);
				max = gigaverb::getparametermax(moduleState, i);
			}
			configParam(i, min, max, min, name, units);
		}
	}

	~Gigaverb() {
		gigaverb::destroy(moduleState);
		deleteBuffers();
	}

	void allocBuffers(long bufferSize) {
		inputBuffers = new t_sample *[numInputs];
		for (int i = 0; i < numInputs; i++) {
			inputBuffers[i] = new t_sample[bufferSize];
		}
		outputBuffers = new t_sample *[numOutputs];
		for (int i = 0; i < numOutputs; i++) {
			outputBuffers[i] = new t_sample[bufferSize];
		}
	}

	void deleteBuffers() {
		for (int i = 0; i < numInputs; i++) {
			delete[] inputBuffers[i];
		}
		delete[] inputBuffers;

		for (int i = 0; i < numOutputs; i++) {
			delete[] outputBuffers[i];
		}
		delete[] outputBuffers;
	}

	void process(const ProcessArgs& args) override {
		// Fill input buffers
		for (int i = 0; i < numInputs; i++) {
			if (inputs[i].isConnected()) {
				inputBuffers[0][i] = inputs[i].getVoltage(0) / 5.f;
			} else {
				inputBuffers[0][i] = 0.f;
			}
		}

		// Process
		gigaverb::perform(moduleState, inputBuffers, numInputs, outputBuffers, numOutputs, 1);

		// Send out
		for (int i = 0; i < numOutputs; i++) {
			outputs[i].setVoltage(outputBuffers[0][i] * 5.f, 0);
		}
	}
};


/// Custom widgets

struct Panel : Widget {
    NVGcolor color = nvgRGB(255, 255, 255);
	Panel(int r = 255, int g = 255, int b = 255) {
        color = nvgRGB(r, g, b);
	}

	void step() override {
		Widget::step();
	}

	void draw(const DrawArgs& args) override {
		nvgBeginPath(args.vg);
		nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
		nvgFillColor(args.vg, color);
		nvgFill(args.vg);
		Widget::draw(args);
	}
};

struct TextDisplay : TransparentWidget {
	std::shared_ptr<Font> font;
	float _x;
	float _y;
	float _w;
	const char* _text;
	NVGcolor _color;
	int _fs;

	TextDisplay(float x, float y, float w, const char* text, NVGcolor color = nvgRGB(236, 239, 241), int fs = 16) {
		_x = x;
		_y = y;
		_w = w;
		_text = text;
		_color = color;
		_fs = fs;
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Lato/Lato-Black.ttf"));
	}

	void draw (const DrawArgs &args) override {
		nvgBeginPath(args.vg);
		nvgFontFaceId(args.vg, font->handle);
		nvgFontSize(args.vg, _fs);
		nvgTextAlign(args.vg, NVG_ALIGN_TOP | NVG_ALIGN_CENTER);
		nvgFillColor(args.vg, _color);
		nvgText(args.vg, _x, _y, _text, NULL);

		float bounds[4];
		nvgTextBounds(args.vg, _x + _w/2, _y, _text, NULL, bounds);
	}
};


/// Main module UI

struct GigaverbWidget : ModuleWidget {
	// Each column of ports has a certain number of "cells" that contain a port and label. 
	int ports_per_col = 6;

	// Each column of params has a certain number of "cells" that contain a port, a label, and a knob.
	int params_per_col = 4;
	
	// Box off the actual section of "cells" with a margin
	int l_margin = RACK_GRID_WIDTH;
	int r_margin = RACK_GRID_WIDTH;
	int bot_margin = 2 * RACK_GRID_WIDTH;
	// The title and top margin together make up the top band of margin
	int top_margin = RACK_GRID_WIDTH;
	int h_title = 3 * RACK_GRID_WIDTH;
	// The height of the actual part that will contain ports and knobs
	int active_box_height = RACK_GRID_HEIGHT - bot_margin - h_title - top_margin;

	// A column will take up 3HP
	int w_col = 3 * RACK_GRID_WIDTH;

	// Offset from the top of a cell to the knobs, ports, and labels
	int port_center_offset = active_box_height / ports_per_col * (1 / 3);
	int label_port_offset = active_box_height / ports_per_col * (3 / 4);
	int param_knob_center_offset = active_box_height / params_per_col * (1 / 4);
	int param_port_center_offset = active_box_height / params_per_col * (1 / 2);
	int param_label_offset = active_box_height / params_per_col * (4 / 5);

	int module_hp;
	
	Panel* panel;

	GigaverbWidget(Gigaverb* module) {
		setModule(module);

		if (module) {
			module_hp = 2 + 3 * (std::ceil(module->numInputs / ports_per_col) 
						  + std::ceil(module->numOutputs / ports_per_col)
						  + std::ceil(module->numParams / params_per_col));

			TextDisplay *title = new TextDisplay(box.size.x / 2, top_margin, box.size.x, "gigaverb");
			addChild(title);

			box.size = Vec(RACK_GRID_WIDTH * module_hp, RACK_GRID_HEIGHT);
			panel = new Panel(55, 71, 79);
			addChild(panel);
		}

		// screws
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));
	}

	void step() override {
		panel->box.size = box.size;
		ModuleWidget::step();
	}

};


/// Register the model

Model* modelGigaverb = createModel<Gigaverb, GigaverbWidget>("gigaverb");
