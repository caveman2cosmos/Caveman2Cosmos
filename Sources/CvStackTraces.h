#pragma once

#ifndef CV_STACK_TRACES_H
#define CV_STACK_TRACES_H

#include <string>

std::string getPyTrace();
std::string getDLLTrace();

#endif