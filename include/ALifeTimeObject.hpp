#pragma once

class ALifeTimeObject {
public:
	virtual ~ALifeTimeObject() = default;

	virtual void init() {}
	virtual void update() {};
	virtual void render() {};

};