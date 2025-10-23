#pragma once

/*
 * Opengl v4.6
*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*
 * glm
*/
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
 * std includes
*/
#include <iostream>
#include <fstream>
// #include <sstream>

#include <stdexcept>

#include <memory>
#include <vector>
#include <algorithm>
#include <array>
// #include <map>
#include <stack>
#include <random>

#include <chrono>
#include <thread>
#include <condition_variable>


namespace shrekrooms {

using DurationSecondsFloat = std::chrono::duration<float>;


class error : public std::exception {
public:
    error() noexcept {
        m_msg = "\n[null | null]\n";
    }
    error(const std::string &file, const std::string &func, const std::string &msg) noexcept {
        m_msg = "\n[" + file + " | " + func + "]\n" + msg;
    }
    ~error() noexcept override { }

    const char *what() const noexcept override {
        return m_msg.c_str();
    }

    error &operator <<(const std::string &add_msg) {
        m_msg += add_msg;
        return *this;
    }
    error &operator <<(char add_char) {
        m_msg += add_char;
        return *this;
    }

protected:
    std::string m_msg;

};


} // namespace shrekrooms

