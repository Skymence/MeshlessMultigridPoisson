#pragma once
#include <vector>
#include <tuple>
#include <stdio.h>
#include "gridclasses.hpp"
std::vector<std::tuple<double, double, double>> pointsFromMshFile(const char* fname);