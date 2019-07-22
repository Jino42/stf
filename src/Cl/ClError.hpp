#pragma once

#include <NTL.hpp>

class ClError {
public:

	ClError();
	static void clCheckError(cl_int perr);
	void clCheckError();

	cl_int err;
	static const char *getErrorString(cl_int error);
private:
};