#pragma once
#include <memory>
#include <map>
#include <string>

class Debug {
public:
	Debug() = default;
	Debug(Debug &) = delete;
	~Debug() = default;

	static Debug &Get() {
		if (!instance_)
			instance_ = std::make_unique<Debug>();
		return *Debug::instance_;
	}



	void setDebug(std::string const &name, bool &ref);
	void debug(std::string const &name, bool b);
	void flipDebug(std::string const &name);

private:


	std::map< std::string, bool & > mapBool_;


	static std::unique_ptr<Debug> instance_;

};