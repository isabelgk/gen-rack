#include "gigaverb.h"

namespace gigaverb {

/*******************************************************************************************************************
Cycling '74 License for Max-Generated Code for Export
Copyright (c) 2016 Cycling '74
The code that Max generates automatically and that end users are capable of exporting and using, and any
  associated documentation files (the “Software”) is a work of authorship for which Cycling '74 is the author
  and owner for copyright purposes.  A license is hereby granted, free of charge, to any person obtaining a
  copy of the Software (“Licensee”) to use, copy, modify, merge, publish, and distribute copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The Software is licensed to Licensee only for non-commercial use. Users who wish to make commercial use of the
  Software must contact the copyright owner to determine if a license for commercial use is available, and the
  terms and conditions for same, which may include fees or royalties. For commercial use, please send inquiries
  to licensing (at) cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
  upon the use, not the user. The Software may be used by individuals, institutions, governments, corporations, or
  other business whether for-profit or non-profit so long as the use itself is not a commercialization of the
  materials or a use that generates or is intended to generate income, revenue, sales or profit.
The above copyright notice and this license shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*******************************************************************************************************************/

// global noise generator
Noise noise;
static const int GENLIB_LOOPCOUNT_BAIL = 100000;


// The State struct contains all the state and procedures for the gendsp kernel
typedef struct State {
	CommonState __commonstate;
	Delay m_delay_11;
	Delay m_delay_10;
	Delay m_delay_12;
	Delay m_delay_6;
	Delay m_delay_7;
	Delay m_delay_8;
	Delay m_delay_13;
	Delay m_delay_15;
	Delay m_delay_14;
	Delay m_delay_17;
	Delay m_delay_16;
	Delay m_delay_9;
	int __exception;
	int vectorsize;
	t_sample m_tail_20;
	t_sample m_roomsize_19;
	t_sample m_early_24;
	t_sample m_bandwidth_21;
	t_sample m_revtime_23;
	t_sample m_damping_22;
	t_sample m_history_5;
	t_sample m_dry_18;
	t_sample m_history_1;
	t_sample m_history_4;
	t_sample samplerate;
	t_sample m_history_3;
	t_sample m_history_2;
	t_sample m_spread_25;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_history_1 = ((int)0);
		m_history_2 = ((int)0);
		m_history_3 = ((int)0);
		m_history_4 = ((int)0);
		m_history_5 = ((int)0);
		m_delay_6.reset("m_delay_6", ((int)5000));
		m_delay_7.reset("m_delay_7", ((int)7000));
		m_delay_8.reset("m_delay_8", ((int)15000));
		m_delay_9.reset("m_delay_9", ((int)16000));
		m_delay_10.reset("m_delay_10", ((int)6000));
		m_delay_11.reset("m_delay_11", ((int)10000));
		m_delay_12.reset("m_delay_12", ((int)12000));
		m_delay_13.reset("m_delay_13", ((int)48000));
		m_delay_14.reset("m_delay_14", ((int)48000));
		m_delay_15.reset("m_delay_15", ((int)48000));
		m_delay_16.reset("m_delay_16", ((int)48000));
		m_delay_17.reset("m_delay_17", ((int)48000));
		m_dry_18 = ((int)1);
		m_roomsize_19 = ((int)75);
		m_tail_20 = ((t_sample)0.25);
		m_bandwidth_21 = ((t_sample)0.5);
		m_damping_22 = ((t_sample)0.7);
		m_revtime_23 = ((int)11);
		m_early_24 = ((t_sample)0.25);
		m_spread_25 = ((int)23);
		genlib_reset_complete(this);
		
	};
	// the signal processing routine;
	inline int perform(t_sample ** __ins, t_sample ** __outs, int __n) {
		vectorsize = __n;
		const t_sample * __in1 = __ins[0];
		const t_sample * __in2 = __ins[1];
		t_sample * __out1 = __outs[0];
		t_sample * __out2 = __outs[1];
		if (__exception) {
			return __exception;
			
		} else if (( (__in1 == 0) || (__in2 == 0) || (__out1 == 0) || (__out2 == 0) )) {
			__exception = GENLIB_ERR_NULL_BUFFER;
			return __exception;
			
		};
		t_sample expr_165 = ((m_roomsize_19 * samplerate) * ((t_sample)0.0029411764705882));
		t_sample expr_164 = safepow(((t_sample)0.001), safediv(((int)1), (m_revtime_23 * samplerate)));
		t_sample mul_144 = (expr_165 * ((t_sample)0.7071));
		t_sample expr_157 = (-safepow(expr_164, mul_144));
		t_sample add_69 = (expr_165 + ((int)5));
		t_sample expr_159 = safepow(expr_164, add_69);
		t_sample mul_146 = (expr_165 * ((int)1));
		t_sample expr_163 = (-safepow(expr_164, mul_146));
		t_sample mul_143 = (expr_165 * ((t_sample)0.63245));
		t_sample expr_156 = (-safepow(expr_164, mul_143));
		t_sample mul_145 = (expr_165 * ((t_sample)0.81649));
		t_sample expr_158 = (-safepow(expr_164, mul_145));
		t_sample rsub_154 = (((int)1) - m_bandwidth_21);
		t_sample mul_75 = (expr_165 * ((t_sample)0.41));
		t_sample add_72 = (mul_75 + ((int)5));
		t_sample expr_162 = safepow(expr_164, add_72);
		t_sample mul_74 = (expr_165 * ((t_sample)0.3));
		t_sample add_71 = (mul_74 + ((int)5));
		t_sample expr_161 = safepow(expr_164, add_71);
		t_sample mul_73 = (expr_165 * ((t_sample)0.155));
		t_sample add_70 = (mul_73 + ((int)5));
		t_sample expr_160 = safepow(expr_164, add_70);
		t_sample mul_140 = (expr_165 * ((t_sample)0.000527));
		int int_139 = int(mul_140);
		t_sample mul_80 = (m_spread_25 * (-0.380445));
		t_sample add_79 = (mul_80 + ((int)931));
		t_sample rsub_76 = (((int)1341) - add_79);
		t_sample mul_89 = (int_139 * rsub_76);
		t_sample mul_108 = (m_spread_25 * ((t_sample)0.376623));
		t_sample add_107 = (mul_108 + ((int)931));
		t_sample rsub_104 = (((int)1341) - add_107);
		t_sample mul_115 = (int_139 * rsub_104);
		t_sample mul_138 = (expr_165 * ((t_sample)0.110732));
		t_sample mul_82 = (m_spread_25 * (-0.568366));
		t_sample add_78 = (mul_82 + ((int)369));
		t_sample rsub_77 = (add_79 - add_78);
		t_sample mul_96 = (int_139 * rsub_77);
		t_sample mul_124 = (m_spread_25 * ((t_sample)0.125541));
		t_sample add_106 = (mul_124 + ((int)369));
		t_sample rsub_105 = (add_107 - add_106);
		t_sample mul_122 = (int_139 * rsub_105);
		t_sample add_81 = (mul_82 + ((int)159));
		t_sample mul_103 = (int_139 * add_81);
		t_sample add_123 = (mul_124 + ((int)159));
		t_sample mul_131 = (int_139 * add_123);
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			const t_sample in2 = (*(__in2++));
			t_sample mul_1 = (in2 * m_dry_18);
			t_sample mul_13 = (in1 * m_dry_18);
			t_sample mul_3 = ((in1 + in2) * ((t_sample)0.707));
			t_sample mix_203 = (mul_3 + (rsub_154 * (m_history_5 - mul_3)));
			t_sample mix_153 = mix_203;
			t_sample tap_49 = m_delay_17.read_linear(mul_144);
			t_sample mul_45 = (tap_49 * expr_157);
			t_sample mix_204 = (mul_45 + (m_damping_22 * (m_history_4 - mul_45)));
			t_sample mix_47 = mix_204;
			t_sample tap_151 = m_delay_16.read_linear(mul_146);
			t_sample mul_142 = (tap_151 * expr_163);
			t_sample mix_205 = (mul_142 + (m_damping_22 * (m_history_3 - mul_142)));
			t_sample mix_149 = mix_205;
			t_sample tap_55 = m_delay_15.read_linear(mul_145);
			t_sample mul_51 = (tap_55 * expr_158);
			t_sample mix_206 = (mul_51 + (m_damping_22 * (m_history_2 - mul_51)));
			t_sample mix_53 = mix_206;
			t_sample tap_43 = m_delay_14.read_linear(mul_143);
			t_sample mul_39 = (tap_43 * expr_156);
			t_sample mix_207 = (mul_39 + (m_damping_22 * (m_history_1 - mul_39)));
			t_sample mix_41 = mix_207;
			t_sample tap_57 = m_delay_13.read_linear(add_72);
			t_sample tap_58 = m_delay_13.read_linear(add_71);
			t_sample tap_59 = m_delay_13.read_linear(add_70);
			t_sample tap_60 = m_delay_13.read_linear(add_69);
			t_sample mul_63 = (tap_59 * expr_160);
			t_sample mul_67 = (tap_57 * expr_162);
			t_sample mul_65 = (tap_58 * expr_161);
			t_sample mul_61 = (tap_60 * expr_159);
			t_sample tap_88 = m_delay_12.read_linear(mul_89);
			t_sample mul_86 = (tap_88 * ((t_sample)0.625));
			t_sample tap_114 = m_delay_11.read_linear(mul_115);
			t_sample mul_112 = (tap_114 * ((t_sample)0.625));
			t_sample add_33 = (mix_149 + mix_53);
			t_sample add_31 = (mix_47 + mix_41);
			t_sample sub_30 = (add_33 - add_31);
			t_sample mul_12 = (sub_30 * ((t_sample)0.5));
			t_sample add_37 = (mul_12 + mul_67);
			t_sample add_26 = (add_33 + add_31);
			t_sample mul_9 = (add_26 * ((t_sample)0.5));
			t_sample add_34 = (mul_9 + mul_61);
			t_sample sub_32 = (mix_149 - mix_53);
			t_sample sub_29 = (mix_47 - mix_41);
			t_sample sub_28 = (sub_32 - sub_29);
			t_sample mul_11 = (sub_28 * ((t_sample)0.5));
			t_sample add_36 = (mul_11 + mul_65);
			t_sample add_27 = (sub_32 + sub_29);
			t_sample rsub_25 = (((int)0) - add_27);
			t_sample mul_10 = (rsub_25 * ((t_sample)0.5));
			t_sample add_35 = (mul_10 + mul_63);
			t_sample tap_137 = m_delay_10.read_linear(mul_138);
			t_sample tap_95 = m_delay_9.read_linear(mul_96);
			t_sample mul_93 = (tap_95 * ((t_sample)0.625));
			t_sample tap_121 = m_delay_8.read_linear(mul_122);
			t_sample mul_119 = (tap_121 * ((t_sample)0.625));
			t_sample mul_135 = (tap_137 * ((t_sample)0.75));
			t_sample sub_134 = (mix_153 - mul_135);
			t_sample mul_133 = (sub_134 * ((t_sample)0.75));
			t_sample add_132 = (mul_133 + tap_137);
			t_sample tap_102 = m_delay_7.read_linear(mul_103);
			t_sample mul_100 = (tap_102 * ((t_sample)0.75));
			t_sample tap_130 = m_delay_6.read_linear(mul_131);
			t_sample mul_128 = (tap_130 * ((t_sample)0.75));
			t_sample mul_24 = (mul_12 * m_tail_20);
			t_sample mul_22 = (mul_10 * m_tail_20);
			t_sample add_8 = (mul_24 + mul_22);
			t_sample mul_23 = (mul_11 * m_tail_20);
			t_sample mul_21 = (mul_9 * m_tail_20);
			t_sample add_7 = (mul_23 + mul_21);
			t_sample sub_16 = (add_8 - add_7);
			t_sample mul_20 = (mul_67 * m_early_24);
			t_sample mul_18 = (mul_63 * m_early_24);
			t_sample add_6 = (mul_20 + mul_18);
			t_sample mul_19 = (mul_65 * m_early_24);
			t_sample mul_17 = (mul_61 * m_early_24);
			t_sample add_5 = (mul_19 + mul_17);
			t_sample sub_15 = (add_6 - add_5);
			t_sample add_2 = (sub_16 + sub_15);
			t_sample add_4 = (add_2 + in2);
			t_sample sub_99 = (add_4 - mul_100);
			t_sample mul_98 = (sub_99 * ((t_sample)0.75));
			t_sample add_97 = (mul_98 + tap_102);
			t_sample sub_92 = (add_97 - mul_93);
			t_sample mul_91 = (sub_92 * ((t_sample)0.625));
			t_sample add_90 = (mul_91 + tap_95);
			t_sample sub_85 = (add_90 - mul_86);
			t_sample mul_84 = (sub_85 * ((t_sample)0.625));
			t_sample add_83 = (mul_84 + tap_88);
			t_sample out2 = (mul_1 + add_83);
			t_sample add_14 = (add_2 + in1);
			t_sample sub_127 = (add_14 - mul_128);
			t_sample mul_126 = (sub_127 * ((t_sample)0.75));
			t_sample add_125 = (mul_126 + tap_130);
			t_sample sub_118 = (add_125 - mul_119);
			t_sample mul_117 = (sub_118 * ((t_sample)0.625));
			t_sample add_116 = (mul_117 + tap_121);
			t_sample sub_111 = (add_116 - mul_112);
			t_sample mul_110 = (sub_111 * ((t_sample)0.625));
			t_sample add_109 = (mul_110 + tap_114);
			t_sample out1 = (mul_13 + add_109);
			t_sample history_152_next_166 = fixdenorm(mix_153);
			t_sample history_46_next_167 = fixdenorm(mix_47);
			t_sample history_148_next_168 = fixdenorm(mix_149);
			t_sample history_52_next_169 = fixdenorm(mix_53);
			t_sample history_40_next_170 = fixdenorm(mix_41);
			m_delay_17.write(add_35);
			m_delay_16.write(add_37);
			m_delay_15.write(add_36);
			m_delay_14.write(add_34);
			m_delay_13.write(add_132);
			m_delay_12.write(sub_85);
			m_delay_11.write(sub_111);
			m_delay_10.write(sub_134);
			m_delay_9.write(sub_92);
			m_delay_8.write(sub_118);
			m_delay_7.write(sub_99);
			m_delay_6.write(sub_127);
			m_history_5 = history_152_next_166;
			m_history_4 = history_46_next_167;
			m_history_3 = history_148_next_168;
			m_history_2 = history_52_next_169;
			m_history_1 = history_40_next_170;
			m_delay_6.step();
			m_delay_7.step();
			m_delay_8.step();
			m_delay_9.step();
			m_delay_10.step();
			m_delay_11.step();
			m_delay_12.step();
			m_delay_13.step();
			m_delay_14.step();
			m_delay_15.step();
			m_delay_16.step();
			m_delay_17.step();
			// assign results to output buffer;
			(*(__out1++)) = out1;
			(*(__out2++)) = out2;
			
		};
		return __exception;
		
	};
	inline void set_dry(t_param _value) {
		m_dry_18 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_roomsize(t_param _value) {
		m_roomsize_19 = (_value < 0.1 ? 0.1 : (_value > 300 ? 300 : _value));
	};
	inline void set_tail(t_param _value) {
		m_tail_20 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_bandwidth(t_param _value) {
		m_bandwidth_21 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_damping(t_param _value) {
		m_damping_22 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_revtime(t_param _value) {
		m_revtime_23 = (_value < 0.1 ? 0.1 : (_value > 1 ? 1 : _value));
	};
	inline void set_early(t_param _value) {
		m_early_24 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_spread(t_param _value) {
		m_spread_25 = (_value < 0 ? 0 : (_value > 100 ? 100 : _value));
	};
	
} State;


///
///	Configuration for the genlib API
///

/// Number of signal inputs and outputs

int gen_kernel_numins = 2;
int gen_kernel_numouts = 2;

int num_inputs() { return gen_kernel_numins; }
int num_outputs() { return gen_kernel_numouts; }
int num_params() { return 8; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "in1", "in2" };
const char *gen_kernel_outnames[] = { "out1", "out2" };

/// Invoke the signal process of a State object

int perform(CommonState *cself, t_sample **ins, long numins, t_sample **outs, long numouts, long n) {
	State* self = (State *)cself;
	return self->perform(ins, outs, n);
}

/// Reset all parameters and stateful operators of a State object

void reset(CommonState *cself) {
	State* self = (State *)cself;
	self->reset(cself->sr, cself->vs);
}

/// Set a parameter of a State object

void setparameter(CommonState *cself, long index, t_param value, void *ref) {
	State *self = (State *)cself;
	switch (index) {
		case 0: self->set_bandwidth(value); break;
		case 1: self->set_damping(value); break;
		case 2: self->set_dry(value); break;
		case 3: self->set_early(value); break;
		case 4: self->set_revtime(value); break;
		case 5: self->set_roomsize(value); break;
		case 6: self->set_spread(value); break;
		case 7: self->set_tail(value); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		case 0: *value = self->m_bandwidth_21; break;
		case 1: *value = self->m_damping_22; break;
		case 2: *value = self->m_dry_18; break;
		case 3: *value = self->m_early_24; break;
		case 4: *value = self->m_revtime_23; break;
		case 5: *value = self->m_roomsize_19; break;
		case 6: *value = self->m_spread_25; break;
		case 7: *value = self->m_tail_20; break;
		
		default: break;
	}
}

/// Get the name of a parameter of a State object

const char *getparametername(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].name;
	}
	return 0;
}

/// Get the minimum value of a parameter of a State object

t_param getparametermin(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmin;
	}
	return 0;
}

/// Get the maximum value of a parameter of a State object

t_param getparametermax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmax;
	}
	return 0;
}

/// Get parameter of a State object has a minimum and maximum value

char getparameterhasminmax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].hasminmax;
	}
	return 0;
}

/// Get the units of a parameter of a State object

const char *getparameterunits(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].units;
	}
	return 0;
}

/// Get the size of the state of all parameters of a State object

size_t getstatesize(CommonState *cself) {
	return genlib_getstatesize(cself, &getparameter);
}

/// Get the state of all parameters of a State object

short getstate(CommonState *cself, char *state) {
	return genlib_getstate(cself, state, &getparameter);
}

/// set the state of all parameters of a State object

short setstate(CommonState *cself, const char *state) {
	return genlib_setstate(cself, state, &setparameter);
}

/// Allocate and configure a new State object and it's internal CommonState:

void *create(t_param sr, long vs) {
	State *self = new State;
	self->reset(sr, vs);
	ParamInfo *pi;
	self->__commonstate.inputnames = gen_kernel_innames;
	self->__commonstate.outputnames = gen_kernel_outnames;
	self->__commonstate.numins = gen_kernel_numins;
	self->__commonstate.numouts = gen_kernel_numouts;
	self->__commonstate.sr = sr;
	self->__commonstate.vs = vs;
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(8 * sizeof(ParamInfo));
	self->__commonstate.numparams = 8;
	// initialize parameter 0 ("m_bandwidth_21")
	pi = self->__commonstate.params + 0;
	pi->name = "bandwidth";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_bandwidth_21;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 1 ("m_damping_22")
	pi = self->__commonstate.params + 1;
	pi->name = "damping";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_damping_22;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_dry_18")
	pi = self->__commonstate.params + 2;
	pi->name = "dry";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_dry_18;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_early_24")
	pi = self->__commonstate.params + 3;
	pi->name = "early";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_early_24;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 4 ("m_revtime_23")
	pi = self->__commonstate.params + 4;
	pi->name = "revtime";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_revtime_23;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0.1;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 5 ("m_roomsize_19")
	pi = self->__commonstate.params + 5;
	pi->name = "roomsize";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_roomsize_19;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0.1;
	pi->outputmax = 300;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 6 ("m_spread_25")
	pi = self->__commonstate.params + 6;
	pi->name = "spread";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_spread_25;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 100;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 7 ("m_tail_20")
	pi = self->__commonstate.params + 7;
	pi->name = "tail";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_tail_20;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	
	return self;
}

/// Release all resources and memory used by a State object:

void destroy(CommonState *cself) {
	State *self = (State *)cself;
	genlib_sysmem_freeptr(cself->params);
		
	delete self;
}


} // gigaverb::
