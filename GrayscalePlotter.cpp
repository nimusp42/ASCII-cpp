#include "GrayscalePlotter.hpp"
#include "CanvasIterators.hpp"
#include <cmath>
#include <functional>

namespace plotter
{

std::vector<char> GrayscalePlotter::DefaultPalette()
{
    return { ' ', '.', ':', '-', '=', '+', '*', '#', '%', '@' };
}

GrayscalePlotter::GrayscalePlotter(std::unique_ptr<Canvas> canvas, const std::vector<char>& palette) 
: Plotter(std::move(canvas))
, palette_(palette) 
{}

GrayscalePlotter::GrayscalePlotter(int width, int height, char background_char, const std::vector<char>& palette) 
: Plotter(width, height, background_char)
, palette_(palette) 
{}

void GrayscalePlotter::DrawLine(const int x1, const int y1, const int x2, const int y2, const double brightness)
{
    Plotter::DrawLine(x1, y1, x2, y2, BrightnessToChar(brightness));
}

void GrayscalePlotter::DrawRectangle(const int x1, const int y1, const int x2, const int y2, const double brightness, const bool fill)
{
    Plotter::DrawRectangle(x1, y1, x2, y2, BrightnessToChar(brightness), fill);
}

void GrayscalePlotter::DrawTriangle(const int x1, const int y1, const int x2, const int y2, const int x3, const int y3,
    const double brightness, const bool fill)
{
    Plotter::DrawTriangle(x1, y1, x2, y2, x3, y3, BrightnessToChar(brightness), fill);
}

void GrayscalePlotter::DrawCircle(const int center_x, const int center_y, const int radius,
    const double brightness, const bool fill)
{
    Plotter::DrawCircle(center_x, center_y, radius, BrightnessToChar(brightness), fill);
}

void GrayscalePlotter::FloodFill(const int x, const int y, const double brightness)
{
    Plotter::FloodFill(x, y, BrightnessToChar(brightness));
}

void GrayscalePlotter::ScanlineFill(const int x, const int y, const double brightness)
{
    Plotter::ScanlineFill(x, y, BrightnessToChar(brightness));
}

void GrayscalePlotter::DrawLinearGradient(const int x1, const int y1, const int x2, const int y2,
    const double start_brightness, const double end_brightness)
{
    const int width = x2 - x1;
    const int height = y2 - y1;

    for (int y = y1; y <= y2; ++y)
    {
        for (int x = x1; x <= x2; ++x)
        {
            if (!GetCanvas().InBounds(x, y))
                continue;

            const double x_ratio = static_cast<double>(x - x1) / width;
            const double y_ratio = static_cast<double>(y - y1) / height;
            const double ratio = (x_ratio + y_ratio) / 2.0;

            const double brightness = start_brightness + ratio * (end_brightness - start_brightness);
            GetCanvas().at(x, y) = BrightnessToChar(brightness);
        }
    }
}

void GrayscalePlotter::DrawRadialGradient(const int center_x, const int center_y, const int radius,
    const double center_brightness, const double edge_brightness)
{
    for (int y = center_y - radius; y <= center_y + radius; ++y)
    {
        for (int x = center_x - radius; x <= center_x + radius; ++x)
        {
            if (!GetCanvas().InBounds(x, y))
                continue;

            const double distance = std::sqrt(std::pow(x - center_x, 2) + std::pow(y - center_y, 2));
            if (distance > radius)
                continue;

            const double ratio = distance / radius;
            const double brightness = center_brightness + ratio * (edge_brightness - center_brightness);
            GetCanvas().at(x, y) = BrightnessToChar(brightness);
        }
    }
}

double GrayscalePlotter::CalculateAverageBrightness()
{
    double total = 0.0;
    int count = 0;

    std::map<char, double> char_to_brightness;
    for (size_t i = 0; i < palette_.size(); ++i)
    {
        const double brightness = static_cast<double>(i) / (palette_.size() - 1);
        char_to_brightness[palette_[i]] = brightness;
    }
    for (auto pixel : GetCanvas())
    {
        if (const auto it = char_to_brightness.find(pixel); it != char_to_brightness.end())
        {
            total += it->second;
            count++;
        }
    }

    return count > 0 ? total / count : 0.0;
}

std::pair<double, double> GrayscalePlotter::GetMinMaxBrightness()
{
    if (GetCanvas().Size() == 0)
    {
        return { 0.0, 0.0 };
    }

    std::map<char, double> char_to_brightness;
    for (size_t i = 0; i < palette_.size(); ++i)
    {
        const double brightness = static_cast<double>(i) / (palette_.size() - 1);
        char_to_brightness[palette_[i]] = brightness;
    }

    double min_brightness = 1.0;
    double max_brightness = 0.0;

    for (const auto& pixel : GetCanvas())
    {
        if (const auto it = char_to_brightness.find(pixel); it != char_to_brightness.end())
        {
            double brightness = it->second;
            min_brightness = std::min(min_brightness, brightness);
            max_brightness = std::max(max_brightness, brightness);
        }
    }

    return { min_brightness, max_brightness };
}

std::vector<std::vector<double>> GrayscalePlotter::GetBrightnessMatrix() const
{
    std::vector<std::vector<double>> matrix(GetCanvas().Height(),
        std::vector<double>(GetCanvas().Width()));

    std::map<char, double> char_to_brightness;
    for (size_t i = 0; i < palette_.size(); ++i)
    {
        const double brightness = static_cast<double>(i) / (palette_.size() - 1);
        char_to_brightness[palette_[i]] = brightness;
    }

    for (int y = 0; y < GetCanvas().Height(); ++y)
    {
        for (int x = 0; x < GetCanvas().Width(); ++x)
        {
            char pixel_char = GetCanvas().at(x, y);
            auto it = char_to_brightness.find(pixel_char);
            matrix[y][x] = it != char_to_brightness.end() ? it->second : 0.0;
        }
    }

    return matrix;
}

void GrayscalePlotter::AdjustBrightness(const double factor)
{
    std::map<char, double> char_to_brightness;
    for (size_t i = 0; i < palette_.size(); ++i)
    {
        const double brightness = static_cast<double>(i) / (palette_.size() - 1);
        char_to_brightness[palette_[i]] = brightness;
    }

    for (int y = 0; y < GetCanvas().Height(); ++y)
    {
        for (int x = 0; x < GetCanvas().Width(); ++x)
        {
            char old_char = GetCanvas().at(x, y);
            if (auto it = char_to_brightness.find(old_char); it != char_to_brightness.end())
            {
                const double new_brightness = std::clamp(it->second * factor, 0.0, 1.0);
                GetCanvas().at(x, y) = BrightnessToChar(new_brightness);
            }
        }
    }
}

void GrayscalePlotter::ApplyThreshold(const double threshold)
{
    std::map<char, double> char_to_brightness;
    for (size_t i = 0; i < palette_.size(); ++i)
    {
        const double brightness = static_cast<double>(i) / (palette_.size() - 1);
        char_to_brightness[palette_[i]] = brightness;
    }

    for (int y = 0; y < GetCanvas().Height(); ++y)
    {
        for (int x = 0; x < GetCanvas().Width(); ++x)
        {
            char old_char = GetCanvas().at(x, y);
            if (const auto it = char_to_brightness.find(old_char); it != char_to_brightness.end())
            {
                const double new_brightness = it->second >= threshold ? 1.0 : 0.0;
                GetCanvas().at(x, y) = BrightnessToChar(new_brightness);
            }
        }
    }
}

void GrayscalePlotter::InvertBrightness()
{
    std::map<char, double> char_to_brightness;
    for (size_t i = 0; i < palette_.size(); ++i)
    {
        const double brightness = static_cast<double>(i) / (palette_.size() - 1);
        char_to_brightness[palette_[i]] = brightness;
    }

    for (int y = 0; y < GetCanvas().Height(); ++y)
    {
        for (int x = 0; x < GetCanvas().Width(); ++x)
        {
            char old_char = GetCanvas().at(x, y);
            if (const auto it = char_to_brightness.find(old_char); it != char_to_brightness.end())
            {
                const double new_brightness = 1.0 - it->second;
                GetCanvas().at(x, y) = BrightnessToChar(new_brightness);
            }
        }
    }
}

char GrayscalePlotter::BrightnessToChar(double brightness) const {
    int idx = brightness * (palette_.size() - 1);
    return palette_[idx];
}

double GrayscalePlotter::GetPixelBrightness(const int x, const int y) const
{
    if (!GetCanvas().InBounds(x, y))
        return 0.0;

    std::map<char, double> char_to_brightness;
    for (size_t i = 0; i < palette_.size(); ++i)
    {
        const double brightness = static_cast<double>(i) / (palette_.size() - 1);
        char_to_brightness[palette_[i]] = brightness;
    }

    const char pixel_char = GetCanvas().at(x, y);
    const auto it = char_to_brightness.find(pixel_char);
    return it != char_to_brightness.end() ? it->second : 0.0;
}

void GrayscalePlotter::SetPixelBrightness(const int x, const int y, const double brightness)
{
    if (GetCanvas().InBounds(x, y))
    {
        GetCanvas().at(x, y) = BrightnessToChar(brightness);
    }
}

std::vector<std::vector<double>> GrayscalePlotter::CreateGaussianKernel(const int size, const double sigma)
{
    if (size % 2 == 0)
    {
        throw std::invalid_argument("Kernel size must be odd");
    }

    std::vector<std::vector<double>> kernel(size, std::vector<double>(size));
    double sum = 0.0;
    const int center = size / 2;

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            const int x = i - center;
            const int y = j - center;
            kernel[i][j] = std::exp(-(x * x + y * y) / (2 * sigma * sigma));
            sum += kernel[i][j];
        }
    }

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            kernel[i][j] /= sum;
        }
    }

    return kernel;
}

std::vector<std::vector<double>> GrayscalePlotter::CreateBoxKernel(const int size)
{
    std::vector<std::vector<double>> kernel(size, std::vector<double>(size));
    const double value = 1.0 / (size * size);

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            kernel[i][j] = value;
        }
    }

    return kernel;
}

std::vector<std::vector<double>> GrayscalePlotter::Convolve(const std::vector<std::vector<double>>& kernel) const
{
    const int kernel_size = kernel.size();
    if (kernel_size % 2 == 0)
    {
        throw std::invalid_argument("Kernel size must be odd");
    }

    const int offset = kernel_size / 2;
    const int width = GetCanvas().Width();
    const int height = GetCanvas().Height();

    std::vector<std::vector<double>> result(height, std::vector<double>(width, 0.0));

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            double sum = 0.0;

            for (int ky = 0; ky < kernel_size; ++ky)
            {
                for (int kx = 0; kx < kernel_size; ++kx)
                {
                    int src_x = x + kx - offset;
                    int src_y = y + ky - offset;

                    // Обработка границ: отражаем
                    if (src_x < 0)
                    {
                        src_x = -src_x;
                    }
                    if (src_x >= width)
                    {
                        src_x = 2 * width - src_x - 1;
                    }
                    if (src_y < 0)
                    {
                        src_y = -src_y;
                    }
                    if (src_y >= height)
                    {
                        src_y = 2 * height - src_y - 1;
                    }

                    if (GetCanvas().InBounds(src_x, src_y))
                    {
                        const double pixel_brightness = GetPixelBrightness(src_x, src_y);
                        sum += pixel_brightness * kernel[ky][kx];
                    }
                }
            }

            result[y][x] = std::clamp(sum, 0.0, 1.0);
        }
    }

    return result;
}

void GrayscalePlotter::ApplyBoxBlur(int kernel_size)
{
    if (kernel_size % 2 == 0)
    {
        kernel_size++; // Делаем нечетным
    }

    const auto kernel = CreateBoxKernel(kernel_size);
    const auto convolved = Convolve(kernel);

    for (int y = 0; y < GetCanvas().Height(); ++y)
    {
        for (int x = 0; x < GetCanvas().Width(); ++x)
        {
            SetPixelBrightness(x, y, convolved[y][x]);
        }
    }
}

void GrayscalePlotter::ApplyGaussianBlur(int kernel_size)
{
    if (kernel_size % 2 == 0)
    {
        kernel_size++; // Делаем нечетным
    }

    const double sigma = kernel_size / 3.0;
    const auto kernel = CreateGaussianKernel(kernel_size, sigma);
    const auto convolved = Convolve(kernel);

    // Применяем результат к canvas
    for (int y = 0; y < GetCanvas().Height(); ++y)
    {
        for (int x = 0; x < GetCanvas().Width(); ++x)
        {
            SetPixelBrightness(x, y, convolved[y][x]);
        }
    }
}

void GrayscalePlotter::SetPalette(const std::vector<char>& new_palette)
{
    if (!new_palette.empty())
    {
        palette_ = new_palette;

        const auto brightness_matrix = GetBrightnessMatrix();
        for (int y = 0; y < GetCanvas().Height(); ++y)
        {
            for (int x = 0; x < GetCanvas().Width(); ++x)
            {
                GetCanvas().at(x, y) = BrightnessToChar(brightness_matrix[y][x]);
            }
        }
    }
}

} // namespace plotter