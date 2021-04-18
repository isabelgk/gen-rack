#include "plugin.hpp"
// #include "genRackExport.h"


/// Processing

struct GenRackExport : Module {
    CommonState *moduleState;
    long numins;
    long numouts;
	t_sample **inputBuffers;  // access like: buffer[sample #][numins]
	t_sample **outputBuffers;

	GenRackExport() {
		// Set default sample rate of 44100 Hz and vector size 1 (VCV uses single sample processing)
		// and update it later if needed
		moduleState = (CommonState *)genRackExport::create(44100, 1);
		genRackExport::reset(moduleState);

		numins = genRackExport::num_inputs();
		numouts = genRackExport::num_outputs();

		// Initialize sample buffers
		// TODO - do block sample processing
		allocBuffers(1);

		// Configure parameters
		config(genRackExport::num_params(), numins, numouts, 0);
		for (int i = 0; i < genRackExport::num_params(); i++) {
			std::string name = std::string(genRackExport::getparametername(moduleState, i));
			std::string units = std::string(genRackExport::getparameterunits(moduleState, i));
			float min = 0.0;
			float max = 1.0;
			if (genRackExport::getparameterhasminmax(moduleState, i)) {
				min = genRackExport::getparametermin(moduleState, i);
				max = genRackExport::getparametermax(moduleState, i);
			}
			configParam(i, min, max, min, name, units);
		}
	}

	~GenRackExport() {
		genRackExport::destroy(moduleState);
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
		genRackExport::perform(moduleState, inputBuffers, numins, outputBuffers, numouts, 1);

		// Send out
		for (int i = 0; i < numouts; i++) {
			outputs[i].setVoltage(outputBuffers[0][i] * 5.f, 0);
		}
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


struct GenRackExportWidget : ModuleWidget {
	Panel* panel;

	GenRackExportWidget(GenRackExport* module) {
		int unit = RACK_GRID_WIDTH;
		float port_rad = 15.79;  // pixels
	
		// Fit three params vertically on 3HP
		int num_params = genRackExport::num_params();
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
		int num_inputs = genRackExport::num_inputs() <= 2 ? genRackExport::num_inputs() : 2;
		int num_outputs = genRackExport::num_outputs() <= 2 ? genRackExport::num_outputs() : 2;
		float io_spacing = (box.size.x - unit) / (num_inputs + num_outputs);
		for (int i = 0; i < genRackExport::num_inputs(); i++) {
			addInput(createInputCentered<PJ301MPort>(Vec(margin + port_rad + io_spacing * i, box.size.y - unit * 2.5), module, i));
		}
		for (int i = 0; i < genRackExport::num_outputs(); i++) {
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

Model* modelGenRackExport = createModel<GenRackExport, GenRackExportWidget>("genRackExport");
