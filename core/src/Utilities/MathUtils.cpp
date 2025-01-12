#include "StdInclude.hpp"
#include "MathUtils.hpp"

namespace IWXMVM::MathUtils
{
	glm::vec3 ForwardVectorFromAngles(float pitch, float yaw, float roll)
	{
		auto p = glm::radians(pitch);
		auto y = glm::radians(yaw);
		return glm::vec3(
			std::cos(y) * std::cos(p),
			std::sin(y) * std::cos(p),
			-std::sin(p)
		);
	}

	glm::vec3 AnglesFromForwardVector(glm::vec3 forward)
	{
		auto yaw = std::atan2(forward.y, forward.x);
		auto pitch = std::atan2(-forward.z, std::sqrt(forward.x * forward.x + forward.y * forward.y));
		return glm::vec3(
			glm::degrees(pitch),
			glm::degrees(yaw),
			0.0f
		);
	}
}