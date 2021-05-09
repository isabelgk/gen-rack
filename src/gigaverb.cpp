#include "plugin.hpp"
#include "gigaverb.h"
#include <array>

using namespace gigaverb;


/// Processing

struct Gigaverb : Module {
	CommonState *moduleState;
	t_sample **inputBuffers;
	t_sample **outputBuffers;

	std::array<int, 10> validBufferSizes = { 1, 4, 16, 32, 64, 128, 256, 512, 1024, 2048 };
	int currentBufferSize = 256;
	int currentSampleRate = 44100;

	int numParams;
	int numInputs;
	int numOutputs;

	int count = 0;

	Gigaverb() {
		moduleState = (CommonState *)create(currentSampleRate, currentBufferSize);
		reset(moduleState);

		numParams = num_params();
		numInputs = num_inputs();
		numOutputs = num_outputs();

		// Initialize sample buffers
		inputBuffers = new t_sample *[numInputs];
		for (int i = 0; i < numInputs; i++) {
			inputBuffers[i] = new t_sample[currentBufferSize];
		}

		outputBuffers = new t_sample *[numOutputs];
		for (int i = 0; i < numOutputs; i++) {
			outputBuffers[i] = new t_sample[currentBufferSize];
		}


		// Configure parameters
		config(numParams, numInputs + numParams, numOutputs, 0);
		for (int i = 0; i < numParams; i++) {
			std::string name = std::string(getparametername(moduleState, i));
			std::string units = std::string(getparameterunits(moduleState, i));
			float min = 0.0;
			float max = 1.0;
			if (getparameterhasminmax(moduleState, i)) {
				min = getparametermin(moduleState, i);
				max = getparametermax(moduleState, i);
			}
			configParam(i, min, max, min, name, units);
		}
	}


	~Gigaverb() {
		destroy(moduleState);
		deleteBuffers();
	}

	void deleteBuffers() {
		for (int i = 0; i < numInputs; i++) {
			if (inputBuffers[i]) {
				delete inputBuffers[i];
			}
		}

		for (int i = 0; i < numOutputs; i++) {
			if (outputBuffers[i]) {
				delete outputBuffers[i];
			}
		}
	}

	void setBufferSize(long bufferSize) {
		if (bufferSize > currentBufferSize) {
			for (int i = 0; i < numInputs; i++) {
				if (inputBuffers[i]) {
					delete inputBuffers[i];
				}
				inputBuffers[i] = new t_sample[bufferSize];
			}

			for (int i = 0; i < numOutputs; i++) {
				if (outputBuffers[i]) {
					delete outputBuffers[i];
				}
				outputBuffers[i] = new t_sample[bufferSize];
			}
		}
		currentBufferSize = bufferSize;
		moduleState->vs = currentBufferSize;
		count = 0;
	}


	void process(const ProcessArgs& args) override {
		if (args.sampleRate != currentSampleRate) {
			moduleState->sr = args.sampleRate;
			currentSampleRate = args.sampleRate;
		}

		if (count >= currentBufferSize) {
			count = 0;
		}
		
		// Fill inputs
		for (int i = 0; i < numInputs; i++) {
			if (inputs[i].isConnected()) {
				inputBuffers[i][count] = inputs[i].getVoltage() / 5.f;
			}
			else {
				inputBuffers[i][count] = 0.f;
			}
		}

		// Set output
		for (int i = 0; i < numOutputs; i++) {
			outputs[i].setVoltage(outputBuffers[i][count] * 5.f);
		}

		// Step forward
		count++;

		// Perform when we've filled the buffer
		if (count == currentBufferSize) {
			// Update any parameters
			for (int i = 0; i < numParams; i++) {
				// Get VCV inputs
				float knobVal = params[i].getValue();  // Already scaled to range that genlib will understand
				float cvVal = inputs[i + numInputs].isConnected() ? inputs[i + numInputs].getVoltage() / 5.f : 0.f;  // Normalize to -1..1

				// Scale to range of parameter
				t_param min = getparametermin(moduleState, i);
				t_param max = getparametermax(moduleState, i);
				t_param range = fabs(max - min);
				t_param val = clamp(knobVal + cvVal * range, min, max); // Offset the knobVal by the CV input

				setparameter(moduleState, i, val, NULL);
			}

			// Fill the buffers
			perform(moduleState, inputBuffers, numInputs, outputBuffers, numOutputs, currentBufferSize);
		}
	}
};


/// Main module UI

struct BufferSizeMenuItem : MenuItem {
	Gigaverb* module;
	int bufferSize;
	void onAction(const event::Action& e) override {
		if (!module) return;
		module->setBufferSize(bufferSize);
	}
};

struct GigaverbWidget : ModuleWidget {
	int numParams;
	int numInputs;
	int numOutputs;

	std::vector<std::string> inputLabels;
	std::vector<std::string> outputLabels;
	std::vector<std::string> paramLabels;

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
	float port_center_offset = active_box_height / ports_per_col * 0.25f;
	float label_port_offset = active_box_height / ports_per_col * 0.55f;
	float param_knob_center_offset = active_box_height / params_per_col * 0.25f;
	float param_port_center_offset = active_box_height / params_per_col * 0.65f;
	float param_label_offset = active_box_height / params_per_col * 0.85f;

	int module_hp = 12;
	
	genrack::Panel *panel;
	bool dirty = false;


	GigaverbWidget(Gigaverb* module) {
		setModule(module);
		box.size = Vec(RACK_GRID_WIDTH * module_hp, RACK_GRID_HEIGHT);

		// Default background and title for module browser - will be drawn over when step() is called
		panel = new genrack::Panel(40, 40, 40);
		addChild(panel);
		panel->box.size = box.size;
		genrack::Title *title = new genrack::Title(box.size.x / 2, top_margin, box.size.x, "gigaverb");
		addChild(title);

		if (module) {
			// Make these publically accessible to the widget
			numParams = module->numParams;
			numInputs = module->numInputs;
			numOutputs = module->numOutputs;

			for (int i = 0; i < numInputs; i++) {
				std::string inputLabel = std::string("in ") + std::to_string(i + 1);
				inputLabels.push_back(inputLabel);
			}

			for (int i = 0; i < numOutputs; i++) {
				std::string outputLabel = std::string("out ") + std::to_string(i + 1);
				outputLabels.push_back(outputLabel);
			}

			for (int i = 0; i < numParams; i++) {
				std::string paramLabel = std::string(getparametername(module->moduleState, i));
				paramLabel.resize(10);
				paramLabels.push_back(paramLabel);
			}

			// Figure out the width of the module
			module_hp = 2 + 3 * (genrack::util::int_div_round_up(numInputs, ports_per_col)
						  + genrack::util::int_div_round_up(numOutputs, ports_per_col)
						  + genrack::util::int_div_round_up(numParams, params_per_col));

			box.size = Vec(RACK_GRID_WIDTH * module_hp, RACK_GRID_HEIGHT);

			// Draw on the next step
			dirty = true;
			step();
		}
	}


	// Runs with every UI frame update
	void step() override {

		// The widget will be dirtied after the module is registered in the constructor
		if (dirty) {
			// Background panel
			panel = new genrack::Panel(40, 40, 40);
			addChild(panel);
			panel->box.size = box.size;

			// Title text
			genrack::Title *title = new genrack::Title(box.size.x / 2, top_margin, box.size.x, "gigaverb");
			addChild(title);

			// Screws
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 0)));
			addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 365)));
			addChild(createWidget<ScrewSilver>(Vec(box.size.x - 30, 365)));

			// PORTS, PARAMS, LABELS
			for (int i = 0; i < numInputs; i++) {
				float left_x = l_margin
								 + int(i / ports_per_col) * w_col;
				float center_x = left_x + w_col / 2;

				float top_y = top_margin + h_title + (i % ports_per_col) * (active_box_height / ports_per_col);
				float port_center_y = top_y + port_center_offset;
				float label_center_y = top_y + label_port_offset;

				addInput(createInputCentered<PJ301MPort>(Vec(center_x, port_center_y), module, i));

				genrack::TextLabel *label = new genrack::TextLabel(center_x, label_center_y, left_x, inputLabels[i].c_str(), nvgRGB(230, 230, 230), 10);
				addChild(label);
			}
			
			for (int i = 0; i < numParams; i++) {
				float left_x = l_margin 
								+ genrack::util::int_div_round_up(numInputs, ports_per_col) * w_col 
								+ int(i / params_per_col) * w_col;				
				float center_x = left_x + w_col / 2;

				float top_y = top_margin + h_title + (i % params_per_col) * (active_box_height / params_per_col);

				float knob_center_y = top_y + param_knob_center_offset;
				float port_center_y = top_y + param_port_center_offset;
				float label_center_y = top_y + param_label_offset;

				addParam(createParamCentered<RoundSmallBlackKnob>(Vec(center_x, knob_center_y), module, i));
				addInput(createInputCentered<PJ301MPort>(Vec(center_x, port_center_y), module, i + numInputs));

				genrack::TextLabel *label = new genrack::TextLabel(center_x, label_center_y, left_x, paramLabels[i].c_str(), nvgRGB(230, 230, 230), 10);
				addChild(label);
			}
			
			for (int i = 0; i < numOutputs; i++) {
				float left_x = l_margin 
								+ genrack::util::int_div_round_up(numInputs, ports_per_col) * w_col 
								+ genrack::util::int_div_round_up(numParams, params_per_col) * w_col 
								+ int(i / ports_per_col) * w_col;

				float center_x = left_x + w_col / 2;

				float top_y = top_margin + h_title + (i % ports_per_col) * (active_box_height / ports_per_col);
				float port_center_y = top_y + port_center_offset;
				float label_center_y = top_y + label_port_offset;

				addOutput(createOutputCentered<PJ301MPort>(Vec(center_x, port_center_y), module, i));

				genrack::TextLabel *label = new genrack::TextLabel(center_x, label_center_y, left_x, outputLabels[i].c_str(), nvgRGB(230, 230, 230), 10);
				addChild(label);
			}

			dirty = false;
		}

		ModuleWidget::step();
	}


	void appendContextMenu(Menu* menu) override {
		Gigaverb* module = dynamic_cast<Gigaverb*>(this->module);

		// Buffer sizes selection
		menu->addChild(new MenuSeparator());
		MenuItem* bufferSizeLabel = new MenuItem;
		bufferSizeLabel->disabled = true;
		bufferSizeLabel->text = "Buffer size";
		menu->addChild(bufferSizeLabel);

		for (int i = 0; i < (int) module->validBufferSizes.size(); i++) {
			BufferSizeMenuItem* item = new BufferSizeMenuItem;
			item->module = module;
			item->text = std::to_string(module->validBufferSizes[i]).c_str();
			item->rightText = CHECKMARK(module->currentBufferSize == module->validBufferSizes[i]);
			item->bufferSize = module->validBufferSizes[i];
			menu->addChild(item);
		}
	}
};


/// Register the model
Model* modelGigaverb = createModel<Gigaverb, GigaverbWidget>("gigaverb");
