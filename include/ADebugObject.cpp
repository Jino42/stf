#include <ADebugObject.hpp>
#include <Engine/MainGraphic.hpp>


ADebugObject::ADebugObject() :
	debug_(false) {

}

void	ADebugObject::setDebug(bool b) {
	debug_ = b;
	if (debug_) {
		init();
		MainGraphic::Get().getDebugGraphic().addDebugObject(*this);
	} else {
		linesObject_.clear();
		MainGraphic::Get().getDebugGraphic().removeDebugObject(*this);
	}
}
