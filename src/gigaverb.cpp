#include "plugin.hpp"
#include "gigaverb.h"


/// Processing

struct Gigaverb : Module {
	CommonState *moduleState;
	long numins;
	long numouts;
	t_sample **inputBuffers;  // access like: buffer[sample #][numins]
	t_sample **outputBuffers;

	Gigaverb() {
		// Set default sample rate of 44100 Hz and vector size 1 (VCV uses single sample processing)
		// and update it later if needed
		moduleState = (CommonState *)gigaverb::create(44100, 1);
		gigaverb::reset(moduleState);

		numins = gigaverb::num_inputs();
		numouts = gigaverb::num_outputs();

		// Initialize sample buffers
		// TODO - do block sample processing
		allocBuffers(1);

		// Configure parameters
		config(gigaverb::num_params(), numins, numouts, 0);
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
		inputBuffers = new t_sample *[numins];
		for (int i = 0; i < numins; i++) {
			inputBuffers[i] = new t_sample[bufferSize];
		}
		outputBuffers = new t_sample *[numouts];
		for (int i = 0; i < numouts; i++) {
			outputBuffers[i] = new t_sample[bufferSize];
		}
	}

	void deleteBuffers() {
		for (int i = 0; i < numins; i++) {
			delete[] inputBuffers[i];
		}
		delete[] inputBuffers;

		for (int i = 0; i < numouts; i++) {
			delete[] outputBuffers[i];
		}
		delete[] outputBuffers;
	}

	void process(const ProcessArgs& args) override {
		// Fill input buffers
		for (int i = 0; i < numins; i++) {
			if (inputs[i].isConnected()) {
				inputBuffers[0][i] = inputs[i].getVoltage(0) / 5.f;
			} else {
				inputBuffers[0][i] = 0.f;
			}
		}

		// Process
		gigaverb::perform(moduleState, inputBuffers, numins, outputBuffers, numouts, 1);

		// Send out
		for (int i = 0; i < numouts; i++) {
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
	Panel* panel;

	GigaverbWidget(Gigaverb* module) {
		int unit = RACK_GRID_WIDTH;
		float port_rad = 15.79;  // pixels
	
		// Fit three params vertically on 3HP
		int num_params = gigaverb::num_params();
		int hp = num_params;
		if (num_params % 3) {
			hp = int(num_params / 3) * 3;
			hp += 3;
		}

		// background/panel
		setModule(module);
		box.size = Vec(unit * hp, RACK_GRID_HEIGHT);
		panel = new Panel(55, 71, 79);
		addChild(panel);

		float margin = unit / 2;
		// float x_spacing = unit * (hp - 1) / 4;

		// screws
		addChild(createWidget<ScrewSilver>(Vec(unit, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
		addChild(createWidget<ScrewSilver>(Vec(unit, 365)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

		// title
		TextDisplay *title = new TextDisplay(box.size.x / 2, unit, box.size.x, "gigaverb");
		addChild(title);

		// input/output ports (for now, force to be <= 2)
		int num_inputs = gigaverb::num_inputs() <= 2 ? gigaverb::num_inputs() : 2;
		int num_outputs = gigaverb::num_outputs() <= 2 ? gigaverb::num_outputs() : 2;
		float io_spacing = (box.size.x - unit) / (num_inputs + num_outputs);
		for (int i = 0; i < gigaverb::num_inputs(); i++) {
			addInput(createInputCentered<PJ301MPort>(Vec(margin + port_rad + io_spacing * i, box.size.y - unit * 2.5), module, i));
		}
		for (int i = 0; i < gigaverb::num_outputs(); i++) {
			addOutput(createOutputCentered<PJ301MPort>(Vec(margin + port_rad + io_spacing * (i + num_inputs), box.size.y - unit * 2.5), module, i));
		}

		// parameters and CV inputs
	}

	void step() override {
		panel->box.size = box.size;
		ModuleWidget::step();
	}

};


/// Register the model

Model* modelGigaverb = createModel<Gigaverb, GigaverbWidget>("gigaverb");
