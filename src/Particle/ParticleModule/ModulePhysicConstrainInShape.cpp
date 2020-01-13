#include "Cl/ClKernel.hpp"
#include "Cl/ClProgram.hpp"
#include "ModulePhysicConstrainInShape.hpp"
#include "ModuleResolvePhysic.hpp"
#include "OpenCGL_Tools.hpp"
#include "Particle/PaticleEmitter/AParticleEmitter.hpp"
#include "cl_type.hpp"
#include <iostream>
#include "Engine/Shape/ShapeManager.hpp"
#include "Engine/Shape/AABB.hpp"
#include "Engine/Shape/Sphere.hpp"

ModulePhysicConstrainInShape::ModulePhysicConstrainInShape(AParticleEmitter &emitter)
    : AParticleModule(emitter) {

    ClProgram::Get().addProgram(pathKernel_ / "PhysicConstrainInShape.cl");
    kernelUpdate_.setKernel(emitter_, "PhysicConstrainInShapeUpdate");
}

void ModulePhysicConstrainInShape::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    gpuBufferParticles_Physic_ = emitter_.getClBuffer<ParticleDataPhysic>();
}

void ModulePhysicConstrainInShape::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelUpdate_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModulePhysicConstrainInShape::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

void ModulePhysicConstrainInShape::setShapeContrain_(std::string const &name) {
    ShapeManager::Get().getShape(name);
    std::shared_ptr<AShape> shape = std::dynamic_pointer_cast<AABB>(ShapeManager::Get().getShape(name));
    std::shared_ptr<AABB> aabb = std::dynamic_pointer_cast<AABB>(shape);
    std::shared_ptr<Sphere> sphere = std::dynamic_pointer_cast<Sphere>(shape);

    if (aabb) {
        gpuBufferLocal_ShapeConstrain_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(cl_AABB));
        cl_AABB *clAABB = aabb->getCl_Shape();
    } else if (sphere) {
        gpuBufferLocal_ShapeConstrain_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(cl_Sphere));
        cl_Sphere *clSphere = sphere->getCl_Shape();
    }
}