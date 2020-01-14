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

ModulePhysicConstrainInShape::ModulePhysicConstrainInShape(AParticleEmitter &emitter, std::string nameShape)
    : AParticleModule(emitter),
      nameShape_(nameShape) {

    ClProgram::Get().addProgram(pathKernel_ / "PhysicConstrainInShape.cl");
    kernelUpdate_.setKernel(emitter_, "PhysicConstrainInShapeUpdate");
    kernelUpdate_.setArgsGPUBuffers(eParticleBuffer::kData);

    setShapeContrain_();
}

void ModulePhysicConstrainInShape::init() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    gpuBufferParticles_Physic_ = emitter_.getClBuffer<ParticleDataPhysic>();
}

void ModulePhysicConstrainInShape::update(float deltaTime) {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);

    kernelUpdate_.beginAndSetUpdatedArgs(*gpuBufferParticles_Physic_, gpuBufferLocal_ShapeConstrain_);

    OpenCGL::RunKernelWithMem(queue_.getQueue(), kernelUpdate_, emitter_.getParticleOCGL_BufferData().mem, cl::NullRange, cl::NDRange(nbParticleMax_));
}

void ModulePhysicConstrainInShape::reload() {
    if (debug_)
        printf("%s\n", __FUNCTION_NAME__);
    init();
}

void ModulePhysicConstrainInShape::setShapeContrain_() {
    ClError err;
    std::shared_ptr<AShape> shape = std::dynamic_pointer_cast<AShape>(ShapeManager::Get().getShape(nameShape_));
    std::shared_ptr<AABB> aabb = std::dynamic_pointer_cast<AABB>(shape);
    std::shared_ptr<Sphere> sphere = std::dynamic_pointer_cast<Sphere>(shape);

    if (aabb) {
        gpuBufferLocal_ShapeConstrain_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(cl_AABB));
        cl_AABB *clAABB = aabb->getCl_Shape();
        err.err = queue_.getQueue().enqueueWriteBuffer(gpuBufferLocal_ShapeConstrain_, GL_TRUE, 0, sizeof(cl_AABB), clAABB);
        err.clCheckError();
        queue_.getQueue().finish();
        delete clAABB;
    } else if (sphere) {
        gpuBufferLocal_ShapeConstrain_ = cl::Buffer(ClContext::Get().context, CL_MEM_WRITE_ONLY, sizeof(cl_Sphere));
        cl_Sphere *clSphere = sphere->getCl_Shape();
        err.err = queue_.getQueue().enqueueWriteBuffer(gpuBufferLocal_ShapeConstrain_, GL_TRUE, 0, sizeof(cl_Sphere), clSphere);
        err.clCheckError();
        queue_.getQueue().finish();
        delete clSphere;
    }
}