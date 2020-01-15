#include <ADebugObject.hpp>
#include <Engine/MainGraphic.hpp>

ADebugObject::ADebugObject() :
	debug_(false),
    pastSizeBufferLines_(0),
    color(glm::vec4(0.f, 9.f, 0.f, 1.f)) {
}

ADebugObject::~ADebugObject() {
    if (debug_)
        setDebug(false);
}

bool &ADebugObject::getDebug() {
    return debug_;
}

void	ADebugObject::setDebug(bool b) {
    if (debug_ == b)
        return ;
	debug_ = b;
	if (debug_) {
        initDebug();
		MainGraphic::Get().getDebugGraphic().addDebugObject(*this);
	} else {
		linesObject_.clear();
		MainGraphic::Get().getDebugGraphic().removeDebugObject(*this);
	}
    pastSizeBufferLines_ = 0;
}

void ADebugObject::updateDebug() {
    if (pastSizeBufferLines_ != linesObject_.size())
        MainGraphic::Get().getDebugGraphic().updateInfo_();
    else
        MainGraphic::Get().getDebugGraphic().updateDebugObject_(*this);
    pastSizeBufferLines_ = linesObject_.size();
}

bool ADebugObject::isDebug() const {
    return debug_;
}

void ADebugObject::setColor(glm::vec4 const &acolor) {
    color = acolor;
}
