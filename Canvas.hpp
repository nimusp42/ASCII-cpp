#pragma once
#include <filesystem>
#include <iostream>
#include <vector>

namespace plotter
{

class Canvas
{
public:
    class RowIterator;
    class ColumnIterator;
    class PixelIterator;

    Canvas(int width, int height, char background_char = ' ');

    Canvas(const Canvas& other) = default;
    Canvas(Canvas&& other) noexcept;
    Canvas& operator=(const Canvas& other);
    Canvas& operator=(Canvas&& other) noexcept;

    [[nodiscard]] int Width() const noexcept;
    [[nodiscard]] int Height() const noexcept;
    [[nodiscard]] int Size() const noexcept;

    char& at(int x, int y);
    [[nodiscard]] const char& at(int x, int y) const;
    char& operator()(int x, int y) noexcept;
    [[nodiscard]] const char& operator()(int x, int y) const noexcept;

    void Clear(char fill_char);
    void FillRegion(int x1, int y1, int x2, int y2, char fill_char);

    [[nodiscard]] bool InBounds(int x, int y) const noexcept;

    void Render(std::ostream& os = std::cout) const;
    void SaveToFile(const std::filesystem::path& filepath) const;
    void SaveToFile(const std::string& filename) const;

    RowIterator RowBegin(int row);
    RowIterator RowEnd(int row);
    ColumnIterator ColBegin(int col);
    ColumnIterator ColEnd(int col);
    PixelIterator begin();
    PixelIterator end();

private:
    int width_;
    int height_;
    char background_;
    std::vector<char> data_;

    size_t CalculateShift(int x, int y) const;
};

} // namespace plotter
