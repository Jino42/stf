#pragma once

#include <ADebugObject.hpp>
#include <memory>

class DebugGraphic : public ALifeTimeObject {
	friend class ADebugObject;
public:

	DebugGraphic();
	~DebugGraphic();

	void render() override;
	void addDebugObject(ADebugObject &debugObject);
	void removeDebugObject(ADebugObject &debugObject);

private:

	unsigned int VAO;
	unsigned int VBO;
	unsigned int const debugVertexBufferMarge_ = 1000;
	unsigned int debugVertexBufferActualSize_;
	unsigned int totalLine_;
	DebugVertex						*debugBuffer_;
	std::vector< ADebugObject * >	bufferObject_;

	void updateInfo_();
	void updateDebugBuffer_();
	void buildBuffers_(unsigned int newSize);
};