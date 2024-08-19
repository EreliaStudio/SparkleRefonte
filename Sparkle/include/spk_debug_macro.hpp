#pragma once

#include "structure/spk_iostream.hpp"

#define DEBUG_LINE() spk::cout << __FUNCTION__ << L"::" << __LINE__ << std::endl;
#define CHECK_GL_ERROR() { GLenum err = glGetError(); if (err != GL_NO_ERROR) { std::cerr << "OpenGL error: " << err << std::endl; }}