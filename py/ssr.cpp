#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <exception>

#include "pyrendererwrapper.h"
#include "binauralrenderer.h"
#include "brsrenderer.h"

namespace py = pybind11;

using PyBinauralRenderer = PyRendererWrapper<ssr::BinauralRenderer>;
using PyBrsRenderer = PyRendererWrapper<ssr::BrsRenderer>;

template<typename Renderer>
void define_renderer_methods(py::class_<Renderer> &renderer_cls) {
	renderer_cls.def(py::init<const std::map<std::string, std::string>&>());
	renderer_cls.def("add_source", &Renderer::add_source, py::arg("params") =
			{ });
	renderer_cls.def("reference_position", &Renderer::reference_position);
	renderer_cls.def("process", &Renderer::process, py::arg("signal_in"),
			py::arg("align") = false);
	renderer_cls.def("block_size", &Renderer::block_size);
	renderer_cls.def("sources", &Renderer::sources);
	renderer_cls.def("outputs", &Renderer::outputs);

	renderer_cls.def("partial_process_begin", &Renderer::partial_process_begin,
			py::arg("signal_in"), py::arg("align") = false);
	renderer_cls.def("partial_process", &Renderer::partial_process);
	renderer_cls.def("partial_process_result",
			&Renderer::partial_process_result);
}

PYBIND11_MODULE(ssr, m) {
	m.doc() = "SoundScape Renderer wrapper"; // optional module docstring

	auto binaural_cls = py::class_<PyBinauralRenderer>(m, "BinauralRenderer");
	define_renderer_methods(binaural_cls);

	auto brs_cls = py::class_<PyBrsRenderer>(m, "BrsRenderer");
	define_renderer_methods(brs_cls);
}

int main() {
	return 0;
}
