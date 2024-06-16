#include <print>
#include <filesystem>
#include <fstream>
#include <vector>
#include <array>
#include <span>
#include <expected>

enum class decrypt_error {
    invalid_input_file = 0,
    invalid_output_file,
};

[[nodiscard]]
std::expected<void, decrypt_error> clog_decrypt(const std::filesystem::path& input_file, const std::filesystem::path& output_file, std::span<const uint8_t> xor_key) {
    std::ifstream input_stream{
        input_file, std::ios::binary
    };

    if (!input_stream.is_open()) {
        return std::unexpected{ decrypt_error::invalid_input_file };
    }

    std::ofstream output_stream{
        output_file, std::ios::binary
    };

    if (!output_stream.is_open()) {
        return std::unexpected{ decrypt_error::invalid_output_file };
    }

    std::vector<char> buffer{ std::istreambuf_iterator<char>(input_stream), {} };

    for (size_t i{}, j{}; i < buffer.size(); i++, j++) {
        if (j == xor_key.size()) {
            j = 0;
        }
        buffer[i] ^= xor_key[j];
    }

    output_stream.write(buffer.data(), buffer.size());
    return {};
}


int main(int argc, char* argv[])
{
    static constexpr std::array<uint8_t, 128> xor_key = { // kept here for easy modification in case of updates
        130, 135, 151, 64, 141, 139, 70, 11, 187, 115, 148, 3, 229, 179, 131, 83,
        105, 107, 131, 218, 149, 175, 74, 35, 135, 229, 151, 172, 36, 88, 175, 54,
        78, 225, 90, 249, 241, 1, 75, 177, 173, 182, 76, 76, 250, 116, 40, 105,
        194, 139, 17, 23, 213, 182, 71, 206, 179, 183, 205, 85, 254, 249, 193, 36,
        255, 174, 144, 46, 73, 108, 78, 9, 146, 129, 78, 103, 188, 107, 156, 222,
        177, 15, 104, 186, 139, 128, 68, 5, 135, 94, 243, 78, 254, 9, 151, 50,
        192, 173, 159, 233, 187, 253, 77, 6, 145, 80, 137, 110, 224, 232, 238, 153,
        83, 0, 60, 166, 184, 34, 65, 50, 177, 189, 245, 40, 80, 224, 114, 174
    };

    if (argc != 1) {
        std::println("Usage: {} <input_file>", argv[0]);
        return 1;
    }

    std::filesystem::path input_file{ argv[1] };
    std::filesystem::path output_file{ input_file };
    output_file.replace_extension(".txt");

    auto result = clog_decrypt(input_file, output_file, xor_key);
    if (!result) {
        switch (result.error())
        {
        case decrypt_error::invalid_input_file:
            std::println("Failed to open input file: {}", input_file.string());
            break;

        case decrypt_error::invalid_output_file:
            std::println("Failed to open output file: {}", output_file.string());
            break;
        default:
            break;
        }
    }
}
