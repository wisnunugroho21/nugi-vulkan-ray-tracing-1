#include <vector>
#include <algorithm>

template <typename T> 
const uint32_t getIndex(std::vector<T>& vec, const T& element) {
  auto itr = std::find(vec.begin(), vec.end(), element);
  
  if (itr != vec.end()) {
    return static_cast<uint32_t>(std::distance(vec.begin(), itr)) + 1u;
  }

  return 0u;
}

template <typename T> 
const uint32_t getIndex(std::shared_ptr<std::vector<T>> vec, const T& element) {
  auto itr = std::find(vec->begin(), vec->end(), element);
  
  if (itr != vec->end()) {
    return static_cast<uint32_t>(std::distance(vec->begin(), itr)) + 1u;
  }

  return 0u;
}