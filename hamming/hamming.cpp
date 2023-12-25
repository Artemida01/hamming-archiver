#include "hamming.h"

uint32_t CntBits(uint32_t num) {
    uint32_t ans = 0;
    while (num) {
        num = num & (num - 1);
        ans++;
    }

    return ans;
}

void SetZeros(bool* arr, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        arr[i] = false;
    }
}

void ThrowBug(std::string& error) {
    throw std::invalid_argument(error);
}

std::vector<char> ToHamming(const bool* bits, uint32_t n, uint32_t cnt_add_bits) {
    bool bits_hamming[n + cnt_add_bits];
    SetZeros(bits_hamming, n + cnt_add_bits);

    uint32_t cur_poz = 0;
    for (uint32_t i = 1; i <= n + cnt_add_bits; i++) {
        if (CntBits(i) == 1) {
            bits_hamming[i - 1] = false;
            continue;
        }

        bits_hamming[i - 1] = bits[cur_poz++];
        for (uint32_t bit = 0; bit < 32; bit++) {
            bool is_bit_on_position = ((i >> bit) & 1);
            if (!is_bit_on_position) {
                continue;
            }
            uint32_t real_position = (1 << bit) - 1;
            bits_hamming[real_position] ^= bits_hamming[i - 1];
        }
    }

    uint32_t cnt_full_chars = (n + cnt_add_bits + 7) / 8;
    std::vector<char> ans(cnt_full_chars, 0);
    for (uint32_t i = 0; i < n + cnt_add_bits; i++) {
        uint32_t value_position = bits_hamming[i] * (1 << (i % 8));
        ans[i / 8] |= value_position;
    }

    return ans;
}

std::vector<char> FromHamming(bool* bits, uint32_t n, uint32_t cnt_control_bits) {
    bool control_bits_old[cnt_control_bits];
    bool control_bits_new[cnt_control_bits];

    uint32_t pointer_old = 0;
    SetZeros(control_bits_new, cnt_control_bits);

    for (uint32_t i = 1; i <= n; i++) {
        if (CntBits(i) == 1) {
            control_bits_old[pointer_old++] = bits[i - 1];
            continue;
        }
        for (uint32_t bit = 0; bit < 32; bit++) {
            if ((1 << bit) > i) {
                break;
            }
            if (i & (1 << bit)) {
                control_bits_new[bit] ^= bits[i - 1];
            }
        }
    }

    bool equal = true;
    for (uint32_t i = 0; i < cnt_control_bits; i++) {
        equal &= (control_bits_new[i] == control_bits_old[i]);
    }

    if (!equal) {
        uint32_t poz = 0;
        for (uint32_t bit = 0; bit < cnt_control_bits; bit++) {
            if (control_bits_new[bit] == control_bits_old[bit]) {
                continue;
            }
            poz += (1 << bit);
        }

        if (poz > n) {
            std::string error = "File damaged, can't open it";
            ThrowBug(error);
        }

        bits[poz - 1] = (1 - bits[poz - 1]);
    }

    std::vector<char> ans;
    char cur = 0;
    uint8_t cur_poz = 0;

    for (uint32_t i = 1; i <= n; i++) {
        if (CntBits(i) == 1) {
            continue;
        }
        cur |= (1 << cur_poz) * bits[i - 1];
        cur_poz++;

        if (cur_poz == 8) {
            ans.push_back(cur);
            cur = 0;
            cur_poz = 0;
        }
    }
    if (cur_poz != 0) {
        ans.push_back(cur);
    }

    return ans;
}
