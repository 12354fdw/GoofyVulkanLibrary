/**
 * @file GFVL_definition.hpp
 * @brief Defines some helper stuff for GFVL.
 * @details Don't include this. Unless you wanna do some master hacking?
 */
#ifndef GFVL_DEFINITION_HPP
#define GFVL_DEFINITION_HPP
#define PRINT(message) std::cout << "[GFVL] " << message << "\n";
#define THROW_EXCEPTION(reason)                 \
  do {                                          \
    std::ostringstream oss;                     \
    oss << "[GFVL] Error! Reason : " << reason; \
    throw std::runtime_error(oss.str());        \
  } while (0);
#define ASSERT(statement, message)               \
  if (statement) {                               \
    std::ostringstream oss;                      \
    oss << "[GFVL] Error! Reason : " << message; \
    throw std::runtime_error(oss.str());         \
  };

#define GFVL_VERSION 1 // internal application name
#define DEBUG_MODE true
#endif