#pragma once

#include <Engine/DebugGraphic.hpp>

class AABB : public ADebugObject {
public:
    AABB();

    void updateLines() override;

    void setMax(glm::vec3 const& vec);
    void setMin(glm::vec3 const& vec);

private:
    glm::vec3 min_;
    glm::vec3 max_;
};