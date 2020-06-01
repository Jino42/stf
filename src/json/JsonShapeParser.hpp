#pragma once
#include <NTL.hpp>
#include <boost/filesystem.hpp>
#include <json.hpp>

class AShape;

class JsonShapeParser {
  public:
    JsonShapeParser() = delete;
    JsonShapeParser(JsonShapeParser const &) = delete;
    ~JsonShapeParser() = default;

    explicit JsonShapeParser(boost::filesystem::path path);

    void parse();

  private:
    boost::filesystem::path jsonPath_;
    json json_;
    std::shared_ptr<AShape> currentShape_;
};
