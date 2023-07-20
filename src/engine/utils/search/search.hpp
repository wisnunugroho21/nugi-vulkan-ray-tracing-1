#include <vector>
#include <algorithm>

template <typename T> 
const int getIndex(std::vector<T>& vec, const T& element) {
  std::vector<T>::iterator itr = std::find(vec.begin(), vec.end(), element);
  
  if (itr != vec.end()) {
    return static_cast<int>(std::distance(vec.begin(), itr));
  }

  return -1;
}

template <typename T> 
const int getIndex(std::shared_ptr<std::vector<T>> vec, const T& element) {
  std::vector<T>::iterator itr = std::find(vec->begin(), vec->end(), element);
  
  if (itr != vec->end()) {
    return static_cast<int>(std::distance(vec->begin(), itr));
  }

  return -1;
}