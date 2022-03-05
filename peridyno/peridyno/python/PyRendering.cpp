#include "PyPeridyno.h"

#include <Node.h>
#include <GLPointVisualModule.h>
#include <GLSurfaceVisualModule.h>
#include "glad/glad.h"

using namespace dyno;


/**
	Wrap the visual module to make sure the GLAD is initialzied.
*/
static int glInitialized = 0;

#define WRAP_VISUAL_MODULE(T) \
class T##Wrap : public T {\
protected:\
bool initializeGL() override{\
    if(glInitialized==0) glInitialized=gladLoadGL();\
    if(glInitialized) return T::initializeGL();\
    return false; }\
};

WRAP_VISUAL_MODULE(GLPointVisualModule)
WRAP_VISUAL_MODULE(GLSurfaceVisualModule)

void pybind_rendering(py::module& m)
{
	py::class_<GLVisualModule, VisualModule, std::shared_ptr<GLVisualModule>>(m, "GLVisualModule")
		.def("set_color", &GLVisualModule::setColor)
		.def("set_metallic", &GLVisualModule::setMetallic)
		.def("set_roughness", &GLVisualModule::setRoughness)
		.def("set_alpha", &GLVisualModule::setAlpha)
		.def("is_transparent", &GLVisualModule::isTransparent);

	py::class_<GLPointVisualModuleWrap, GLVisualModule, std::shared_ptr<GLPointVisualModuleWrap>>
		(m, "GLPointVisualModule", py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def("set_point_size", &GLPointVisualModuleWrap::setPointSize)
		.def("get_point_size", &GLPointVisualModuleWrap::getPointSize);

	py::class_<GLSurfaceVisualModuleWrap, GLVisualModule, std::shared_ptr<GLSurfaceVisualModuleWrap>>
		(m, "GLSurfaceVisualModule", py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<>())
		.def("set_node", [](std::shared_ptr<GLSurfaceVisualModuleWrap> r, std::shared_ptr<Node> n)
			{
				n->currentTopology()->connect(r->inTriangleSet());
				n->graphicsPipeline()->pushModule(r);
			});

}