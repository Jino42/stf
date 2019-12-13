#include "RadixCl.hpp"
#include "Cl/ClProgram.hpp"
#include "PathManager.hpp"
#include <climits>
#include "Debug.hpp"

RadixCl::RadixCl() :
queue_(ClContext::Get().context, ClContext::Get().deviceDefault),
kernelCount_(ClProgram::Get().getKernel("count")),
kernelScan_(ClProgram::Get().getKernel("scan")),
kernelCoalesce_(ClProgram::Get().getKernel("coalesce")),
kernelReorder_(ClProgram::Get().getKernel("reorder")),
kernelBlocksum_(ClProgram::Get().getKernel("scan2"))
{
	ClProgram::Get().addProgram(PathManager::Get().getPath("particleKernels") / "Test.cl");
	Debug::Get().setDebug("Radix", RadixCl::debug_);
}


void RadixCl::radix(cl::Buffer &arrayGpu, size_t lengthArrayGpu) {

	//
	//	SETUP SIZE
	//

	size_t groups = 16;
	size_t localWorkSize = 16;
	size_t globalWorkSize = localWorkSize * groups;

	const size_t baseArrayLength = lengthArrayGpu;
	const size_t rest = lengthArrayGpu % (groups * localWorkSize);
	const size_t arrayLength = (rest == 0 ? lengthArrayGpu : (lengthArrayGpu - rest + (groups * localWorkSize)));

	//
	//	CREATE BUFFERS
	//

	cl::Buffer bufferArray(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * arrayLength);
	cl::Buffer bufferHisto(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * BUCK * groups * localWorkSize);
	cl::Buffer bufferScan(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * BUCK * groups * localWorkSize);
	cl::Buffer bufferBlocksum(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * groups);
	cl::Buffer bufferOutput(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * arrayLength);

	cl::LocalSpaceArg bufferLocalHisto = cl::Local(sizeof(int) * BUCK * localWorkSize);;
	cl::LocalSpaceArg bufferLocalScan = cl::Local(sizeof(int) * BUCK * localWorkSize);
	cl::LocalSpaceArg bufferLocalBloclsum = cl::Local(sizeof(int) * groups);

	//Write
	err_.err = queue_.enqueueCopyBuffer(arrayGpu, bufferArray, 0, 0, sizeof(int) * baseArrayLength);
	err_.err |= queue_.enqueueFillBuffer<int>(bufferArray, INT_MAX, sizeof(int) * baseArrayLength, sizeof(int) * (arrayLength - baseArrayLength));
	err_.clCheckError();

	//
	//		SET FIXED ARGS
	//
	//Histo
	err_.err = kernelCount_.setArg(2, bufferLocalHisto); // Local Histogram
	err_.err |= kernelCount_.setArg(4, arrayLength);
	err_.clCheckError();

	//SCAN
	size_t scanGlobalWorkSize = (BUCK * groups * localWorkSize) / 2;
	size_t scanLocalWorkSize = scanGlobalWorkSize / groups;
	int doBlockSum2 = 1;
	err_.err = kernelScan_.setArg(1, bufferScan);
	err_.err |= kernelScan_.setArg(2, bufferLocalScan);
	err_.err |= kernelScan_.setArg(3, bufferBlocksum);
	err_.err |= kernelScan_.setArg(4, doBlockSum2);
	err_.clCheckError();

	//Blocksum
	size_t blocksumGlobalWorkSize = groups / 2;
	size_t blocksumLocalWorkSize =  groups / 2;
	int doBlockSum = 0;
	err_.err = kernelBlocksum_.setArg(0, bufferBlocksum);
	err_.err |= kernelBlocksum_.setArg(1, bufferBlocksum);
	err_.err |= kernelBlocksum_.setArg(2, bufferLocalBloclsum);
	err_.err |= kernelBlocksum_.setArg(3, bufferBlocksum);
	err_.err |= kernelBlocksum_.setArg(4, doBlockSum);
	err_.clCheckError();


	//Coalesce
	size_t coalesceGlobalWorkSize = (BUCK * localWorkSize * groups) / 2;
	size_t coalesceLocalWorkSize = coalesceGlobalWorkSize / groups;
	err_.err = kernelCoalesce_.setArg(0, bufferScan);
	err_.err |= kernelCoalesce_.setArg(1, bufferBlocksum);
	err_.clCheckError();

	//Reorder
	size_t reorderGlobalWorkSize = groups * localWorkSize;
	size_t reorderLocalWorkSize = groups;
	err_.err = kernelReorder_.setArg(1, bufferScan);
	err_.err |= kernelReorder_.setArg(4, arrayLength);
	err_.err |= kernelReorder_.setArg(5, bufferLocalHisto);
	err_.clCheckError();


	//
	//		PASS
	//
	for (int pass = 0; pass < BITS / RADIX; pass++) {

		//
		//	Count
		//
		err_.err = kernelCount_.setArg(0, bufferArray);
		err_.err |= kernelCount_.setArg(1, bufferHisto);
		err_.err |= kernelCount_.setArg(3, pass);
		err_.clCheckError();

		if (debug_) {
			std::cout << "globalWorkSize : " << globalWorkSize << std::endl;
			std::cout << "localWorkSize : " << localWorkSize << std::endl;
		}
		err_.err = queue_.enqueueNDRangeKernel(kernelCount_, cl::NullRange, cl::NDRange(globalWorkSize),
											 cl::NDRange(localWorkSize));
		err_.clCheckError();
		queue_.finish();
		if (debug_)
			printIntArrayGpu(bufferHisto, BUCK * groups * localWorkSize, "Count : bufferHisto");


		//
		//	Scan
		//
		err_.err = kernelScan_.setArg(0, bufferHisto);
		err_.clCheckError();

		if (debug_) {
			std::cout << "scanGlobalWorkSize : " << scanGlobalWorkSize << std::endl;
			std::cout << "scanLocalWorkSize : " << scanLocalWorkSize << std::endl;
		}

		err_.err = queue_.enqueueNDRangeKernel(kernelScan_, cl::NullRange, cl::NDRange(scanGlobalWorkSize),
											 cl::NDRange(scanLocalWorkSize));
		err_.clCheckError();
		queue_.finish();
		if (debug_) {
			printIntArrayGpu(bufferScan, BUCK * groups * localWorkSize, "Scan : bufferScan");
			printIntArrayGpu(bufferBlocksum, groups, "Scan : bufferBlocksum");
		}

		//
		//	Blocksum
		//

		if (debug_) {
			std::cout << "blocksumGlobalWorkSize : " << blocksumGlobalWorkSize << std::endl;
			std::cout << "blocksumLocalWorkSize : " << blocksumLocalWorkSize << std::endl;
		}
		err_.err = queue_.enqueueNDRangeKernel(kernelBlocksum_, cl::NullRange, cl::NDRange(blocksumGlobalWorkSize),
											 cl::NDRange(blocksumLocalWorkSize));
		err_.clCheckError();
		queue_.finish();

		if (debug_)
			printIntArrayGpu(bufferBlocksum, groups, "Blocksum : bufferBlocksum");

		//
		//	Coalesce
		//
		if (debug_) {
			std::cout << "coalesceGlobalWorkSize : " << coalesceGlobalWorkSize << std::endl;
			std::cout << "coalesceLocalWorkSize : " << coalesceLocalWorkSize << std::endl;
		}
		err_.err = queue_.enqueueNDRangeKernel(kernelCoalesce_, cl::NullRange, cl::NDRange(coalesceGlobalWorkSize),
											 cl::NDRange(coalesceLocalWorkSize));
		err_.clCheckError();
		queue_.finish();
		if (debug_)
			printIntArrayGpu(bufferScan, BUCK * localWorkSize * groups, "Coalesce : bufferScan");

		//
		//	Reorder
		//

		//Reorder arguments
		err_.err = kernelReorder_.setArg(0, bufferArray);
		err_.err |= kernelReorder_.setArg(2, bufferOutput);
		err_.err |= kernelReorder_.setArg(3, pass);
		err_.clCheckError();

		if (debug_) {
			std::cout << "ReorderGlobalWorkSize : " << reorderGlobalWorkSize << std::endl;
			std::cout << "ReorderLocalWorkSize : " << reorderLocalWorkSize << std::endl;
		}

		err_.err = queue_.enqueueNDRangeKernel(kernelReorder_, cl::NullRange, cl::NDRange(reorderGlobalWorkSize),
											 cl::NDRange(reorderLocalWorkSize));
		err_.clCheckError();
		queue_.finish();


		if (debug_)
			printIntArrayGpu(bufferOutput, arrayLength, "Reorder : bufferOutput");

		cl::Buffer tmp = bufferArray;
		bufferArray = bufferOutput;
		bufferOutput = tmp;
	}

	queue_.enqueueCopyBuffer(bufferArray, arrayGpu, 0, 0, sizeof(int) * baseArrayLength);
	printIntArrayGpu(arrayGpu, baseArrayLength, "FinalArray");
}

void RadixCl::printIntArrayGpu(cl::Buffer &buffer, size_t length, std::string const &name) {
	std::cout << name << std::endl;
	int *arrayCpu = new int[length];
	err_.err = queue_.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(int) * length, arrayCpu);
	err_.clCheckError();
	queue_.finish();

	std::cout << "start [" << name << "]" << std::endl;
	for (int i = 0; i < length; i++)
		std::cout << i << " : " << arrayCpu[i] << std::endl;
	std::cout << "end [" << name << "]" << std::endl;
}

bool RadixCl::debug_ = false;