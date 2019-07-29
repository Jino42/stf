#include <ADebugObject.hpp>
#include <Engine/MainGraphic.hpp>

ADebugObject::ADebugObject() :
	debug_(false),
    pastSizeBufferLines_(0) {

}

void	ADebugObject::setDebug(bool b) {
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
