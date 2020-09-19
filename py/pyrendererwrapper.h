/*
 * PyRendererWrapper.h
 *
 *  Created on: Sep 18, 2020
 *      Author: pawel
 */

#ifndef PY_PYRENDERERWRAPPER_H_
#define PY_PYRENDERERWRAPPER_H_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <exception>

#include "apf/pointer_policy.h"
#include "rendererbase.h"

namespace py = pybind11;

template<typename Renderer>
class PyRendererWrapper: public Renderer {
public:

	using array = py::array_t<float, py::array::forcecast | py::array::f_style>;

	PyRendererWrapper(const std::map<std::string, std::string> &params) :
			Renderer(apf::parameter_map(params)) {

		this->load_reproduction_setup();

//		apf::parameter_map source_params;
//		source_params.set("properties_file", filename);
//		auto id = this->add_source("", source_params);
//		this->get_source(id)->active = true;

		_block_size = this->Renderer::block_size();
		_in_channels = 0;
		_out_channels = this->get_output_list().size();

		_partial_process_offset = 0;
		_partial_process_signal_in_len = 0;

		this->activate();

		_outputs.resize(_out_channels);
	}

	void add_source(const std::map<std::string, std::string> &params = { }) {
		apf::parameter_map source_params(params);
		auto id = this->Renderer::add_source("", source_params);
		this->get_source(id)->active = true;

		_in_channels += 1;
		_inputs.resize(_in_channels);
	}

	// TODO: separate "partial_" methods to another class (iterator?)
	void partial_process_begin(const array &signal_in, bool align = false) {
		_partial_process_signal_in_len = _get_signal_len(signal_in, align);
		_partial_process_signal_in = signal_in;
		_partial_process_signal_out = _create_signal_buffer(
				_partial_process_signal_in_len);
		_partial_process_offset = 0;
	}

	bool partial_process() {
//		_process(_partial_process_signal_in, _partial_process_signal_out,
//				_partial_process_offset, _partial_process_offset + _block_size);
		this->_process_block(_partial_process_signal_in,
				_partial_process_signal_out, _partial_process_offset);
		_partial_process_offset += _block_size;

		if (_partial_process_offset >= _partial_process_signal_in_len) {
			return false;
		} else {
			return true;
		}
	}

	py::array_t<float> partial_process_result() {
		return _partial_process_signal_out;
	}

	py::array_t<float> process(const array &signal_in, bool align = false) {
		if (signal_in.ndim() != 2) {
			throw std::invalid_argument(
					"the dimension of the input signal must be 2");
		}

		if (signal_in.shape(1) != _in_channels) {
			throw std::invalid_argument(
					"second dimension must be equal to the number of sources");
		}

		size_t signal_in_len = _get_signal_len(signal_in, align);
		array signal_out = _create_signal_buffer(signal_in_len);
		_process(signal_in, signal_out, 0, signal_in_len);

		return signal_out;
	}

	void reference_position(float x, float y) {
		this->state.reference_position = ssr::Pos { x, y };
	}

	size_t block_size() {
		return _block_size;
	}

	size_t sources() {
		return _in_channels;
	}

	size_t outputs() {
		return _out_channels;
	}

private:
	size_t _in_channels, _out_channels, _block_size;
	std::vector<float*> _inputs;
	std::vector<float*> _outputs;

	array _partial_process_signal_in;
	array _partial_process_signal_out;
	size_t _partial_process_offset;
	size_t _partial_process_signal_in_len;

	array _create_signal_buffer(ssize_t len) {
		return array( { len, static_cast<ssize_t>(_out_channels) });
	}

	void _process(const array &signal_in, array &signal_out, size_t begin,
			size_t end) {

		for (size_t offset = begin; offset + _block_size < end; offset +=
				_block_size) {
			this->_process_block(signal_in, signal_out, offset);
		}
	}

	size_t _get_signal_len(const array &signal, bool align = false) {
		size_t signal_in_len;

		if (!align) {
			if (signal.shape(0) % _block_size != 0) {
				throw std::invalid_argument(
						"second dimension must be divisible by block_size");
			}

			signal_in_len = signal.shape(0);
		} else {
			signal_in_len = signal.shape(0) / _block_size * _block_size;
		}

		return signal_in_len;
	}

	void _process_block(const array &in, array &out, size_t offset) {
		for (int i = 0; i < _in_channels; ++i) {
			// input data should not be modified
			_inputs[i] = const_cast<float*>(in.data(offset, i));
		}

		for (int i = 0; i < _out_channels; ++i) {
			_outputs[i] = out.mutable_data(offset, i);
		}

		this->audio_callback(_block_size, &_inputs[0], &_outputs[0]);
	}
};

#endif /* PY_PYRENDERERWRAPPER_H_ */
