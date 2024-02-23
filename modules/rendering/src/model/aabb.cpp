#include <aabb.h>

/**
 * @brief Constructor for the AABB structure.
 */
Mgtt::Rendering::AABB::AABB() {
  this->min = glm::vec3(FLT_MAX);
  this->max = glm::vec3(-FLT_MAX);
  this->center = glm::vec3(0.0f);
}

/**
 * @brief CalculateBoundingBox calculates the bounding box of an object
 * transformed by the given matrix.
 *
 * This function takes a 4x4 transformation matrix and calculates the bounding
 * box of an object after being transformed by the matrix. The resulting
 * bounding box can be used for various purposes, such as collision detection or
 * rendering optimizations.
 *
 * @param m A 4x4 transformation matrix representing the object's
 * transformation.
 *
 * @return void This function does not return a value. The bounding box
 * information is typically stored or used internally within the calling code.
 *
 * @note The function assumes that the object's original bounding box is defined
 * in its local space. The resulting bounding box is in the same coordinate
 * space as the transformed object.
 *
 * @see glm::mat4 - The type of the transformation matrix.
 */
void Mgtt::Rendering::AABB::CalculateBoundingBox(const glm::mat4 &m) {
  glm::vec3 min = glm::vec3(m[3]);
  glm::vec3 max = min;
  glm::vec3 v0, v1;

  glm::vec3 right = glm::vec3(m[0]);
  v0 = right * this->min.x;
  v1 = right * this->max.x;
  min += glm::min(v0, v1);
  max += glm::max(v0, v1);

  glm::vec3 up = glm::vec3(m[1]);
  v0 = up * this->min.y;
  v1 = up * this->max.y;
  min += glm::min(v0, v1);
  max += glm::max(v0, v1);

  glm::vec3 back = glm::vec3(m[2]);
  v0 = back * this->min.z;
  v1 = back * this->max.z;
  min += glm::min(v0, v1);
  max += glm::max(v0, v1);

  this->min = min;
  this->max = max;
}