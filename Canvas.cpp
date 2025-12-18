#include "Canvas.hpp"
#include "CanvasIterators.hpp"
#include <algorithm>
#include <fstream>

namespace plotter {

Canvas::Canvas(int width, int height, char background_char)
    : width_(width), height_(height), background_(background_char), data_(std::vector<char>(width * height, background_char)) {}

Canvas::Canvas(Canvas&& other) noexcept {
    width_ = other.width_;
    height_ = other.height_;
    background_ = other.background_;
    std::swap(data_, other.data_);
}

Canvas& Canvas::operator=(const Canvas& other) {
    if (this != &other) {
        Canvas tmp(other);
        width_ = tmp.width_;
        height_ = tmp.height_;
        background_ = tmp.background_;
        std::swap(data_, tmp.data_);
    }
    return *this;
}

Canvas& Canvas::operator=(Canvas&& other) noexcept {
    if (this != &other) {
        Canvas tmp(std::move(other));
        width_ = tmp.width_;
        height_ = tmp.height_;
        background_ = tmp.background_;
        std::swap(data_, tmp.data_);
    }
    return *this;
}

[[nodiscard]] int Canvas::Width() const noexcept {
    return width_;
}

[[nodiscard]] int Canvas::Height() const noexcept {
    return height_;
}

[[nodiscard]] int Canvas::Size() const noexcept {
    return Width() * Height();
}

char& Canvas::at(int x, int y) {
    return data_[CalculateShift(x, y)];
}

[[nodiscard]] const char& Canvas::at(int x, int y) const {
    return data_.at(CalculateShift(x, y));
}

char& Canvas::operator()(int x, int y) noexcept {
    return data_[CalculateShift(x, y)];
}

[[nodiscard]] const char& Canvas::operator()(int x, int y) const noexcept {
    return data_[CalculateShift(x, y)];
}

void Canvas::Clear(char fill_char) {
    std::fill(data_.begin(), data_.end(), fill_char);
}

void Canvas::FillRegion(int x1, int y1, int x2, int y2, char fill_char) {
    while (y1 < y2) {
        int shift_left = CalculateShift(x1, y1);
        int shift_right = CalculateShift(x2, y2) + 1;
        std::fill(data_.begin() + shift_left, data_.begin() + shift_right, fill_char);
        ++y1;
    }
}

[[nodiscard]] bool Canvas::InBounds(int x, int y) const noexcept {
    return x >= 0 && x < Width() && y >= 0 && y < Height();
}

void Canvas::Render(std::ostream& os) const {
    for (int i = 0; i < data_.size(); i += Width()) {
        for (int j = 0; j < Width(); ++j) {
            os << data_[i + j];
        }
        os << '\n';
    }

    os << std::flush;
}

void Canvas::SaveToFile(const std::filesystem::path& filepath) const {
    std::filesystem::path absolute_path = std::filesystem::absolute(filepath);

    if (absolute_path.has_parent_path()) {
        try {
            std::filesystem::create_directories(absolute_path.parent_path());
        } catch (std::filesystem::filesystem_error& err) {
            throw std::runtime_error("create parent directories for '" + absolute_path.string() + "' error: " + err.what());
        }
    }

    std::ofstream out_file(absolute_path);
    if (!out_file.is_open()) {
        return;
    }

    out_file << "Canvas " << Width() << 'x' << Height() << '\n';
    out_file << "Background: " << '\'' << background_ << '\'' << '\n';
    out_file << "Content:" << '\n';
    Render(out_file);

    if (out_file.fail()) {
        throw std::runtime_error("failed to write to file: " + absolute_path.string());
    }
}

void Canvas::SaveToFile(const std::string& filename) const {
    return SaveToFile(std::filesystem::path(filename));
}

Canvas::RowIterator Canvas::RowBegin(int row) {
    return Canvas::RowIterator(this, 0, row);
}

Canvas::RowIterator Canvas::RowEnd(int row) {
    return Canvas::RowIterator(this, Width(), row);
}

Canvas::ColumnIterator Canvas::ColBegin(int col) {
    return Canvas::ColumnIterator(this, col, 0);
}

Canvas::ColumnIterator Canvas::ColEnd(int col) {
    return Canvas::ColumnIterator(this, col, Height());
}

Canvas::PixelIterator Canvas::begin() {
    return Canvas::PixelIterator(this, 0);
}

Canvas::PixelIterator Canvas::end() {
    return Canvas::PixelIterator(this, data_.size());
}

size_t Canvas::CalculateShift(int x, int y) const {
    return x + y * Width();
}

} // namespace plotter