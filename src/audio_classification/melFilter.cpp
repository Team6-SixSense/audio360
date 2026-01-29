/**
 
@file melFilter.cpp
@brief Mel Filter Module*/

#include <vector>
#include <stdexcept>

namespace audio360 {
namespace features {

class MelFilter {
public:
    explicit MelFilter(const std::vector<std::vector<float>>& H)
        : filterbank_H(H)
    {
        if (filterbank_H.empty() || filterbank_H[0].empty()) {
            throw std::invalid_argument("invalidSpectrogramDimension");
        }

        const size_t K = filterbank_H[0].size();
        for (const auto& row : filterbank_H) {
            if (row.size() != K) {
                throw std::invalid_argument("invalidSpectrogramDimension");
            }
        }
    }

    std::vector<std::vector<float>> applyMelFilterBank(
        const std::vector<std::vector<float>>& spectrogram) const
    {
        if (spectrogram.empty()) {
            return {};
        }

        const size_t T = spectrogram.size();
        const size_t K = spectrogram[0].size();

        for (const auto& frame : spectrogram) {
            if (frame.size() != K) {
                throw std::invalid_argument("invalidSpectrogramDimension");
            }
        }

        const size_t M = filterbank_H.size();
        const size_t K_h = filterbank_H[0].size();

        if (K != K_h) {
            throw std::invalid_argument("invalidSpectrogramDimension");
        }

        std::vector<std::vector<float>> E(T, std::vector<float>(M, 0.0f));

        for (size_t t = 0; t < T; ++t) {
            for (size_t m = 0; m < M; ++m) {
                for (size_t k = 0; k < K; ++k) {
                    E[t][m] += spectrogram[t][k] * filterbank_H[m][k];
                }
            }
        }

        return E;
    }

private:
    const std::vector<std::vector<float>> filterbank_H;
};

} // namespace features
} // namespace audio360