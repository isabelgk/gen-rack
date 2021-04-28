#include "rack.hpp"

using namespace rack;

extern Plugin *pluginInstance;


namespace genrack {

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

struct Title : TransparentWidget {
	std::shared_ptr<Font> font;
	float _x;
	float _y;
	float _w;
	const char* _text;
	NVGcolor _color;
	int _fs;

	Title(float x, float y, float w, const char* text, NVGcolor color = nvgRGB(230, 230, 230), int fs = 24) {
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

struct TextLabel : TransparentWidget {
	std::shared_ptr<Font> font;
	float _x;
	float _y;
	float _w;
	const char* _text;
	NVGcolor _color;
	int _fs;

	TextLabel(float x, float y, float w, const char* text, NVGcolor color = nvgRGB(230, 230, 230), int fs = 12) {
		_x = x;
		_y = y;
		_w = w;
		_text = text;
		_color = color;
		_fs = fs;
		font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Lato/Lato-Regular.ttf"));
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

}  // namespace genrack