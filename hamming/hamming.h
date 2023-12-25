#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>

std::vector<char> ToHamming(const bool* bits, uint32_t n, uint32_t cnt_add_bits);

void ThrowBug(std::string& error);

void SetZeros(bool* arr, uint32_t size);

std::vector<char> FromHamming(bool* bits, uint32_t n, uint32_t cnt_control_bits);
