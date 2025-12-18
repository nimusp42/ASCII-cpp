#include "Plotter.hpp"
#include "CanvasIterators.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <stack>

namespace plotter
{

Plotter::Plotter(std::unique_ptr<Canvas> canvas) : canvas_(std::move(canvas))
{
    if (!canvas_)
    {
        throw std::invalid_argument("Canvas cannot be null");
    }
}

Plotter::Plotter(int width, int height, char background_char)
    : canvas_(std::make_unique<Canvas>(width, height, background_char))
{
}

void Plotter::DrawLine(const int x1, const int y1, const int x2, const int y2,
                       const char brush)
{
    DrawLineBresenham(x1, y1, x2, y2, brush);
}

void Plotter::DrawRectangle(const int x1, const int y1, const int x2,
                            const int y2, const char brush, const bool fill)
{
    if (fill)
    {
        canvas_->FillRegion(x1, y1, x2, y2, brush);
    }
    else
    {
        DrawLine(x1, y1, x2, y1, brush);
        DrawLine(x2, y1, x2, y2, brush);
        DrawLine(x2, y2, x1, y2, brush);
        DrawLine(x1, y2, x1, y1, brush);
    }
}

void Plotter::DrawTriangle(const int x1, const int y1, const int x2,
                           const int y2, const int x3, const int y3,
                           const char brush, const bool fill)
{
    if (fill)
    {
        FillTriangle(x1, y1, x2, y2, x3, y3, brush);
    }
    else
    {
        DrawLine(x1, y1, x2, y2, brush);
        DrawLine(x2, y2, x3, y3, brush);
        DrawLine(x3, y3, x1, y1, brush);
    }
}

void Plotter::DrawCircle(const int center_x, const int center_y,
                         const int radius, const char brush, const bool fill)
{
    if (fill)
    {
        for (int y = -radius; y <= radius; ++y)
        {
            for (int x = -radius; x <= radius; ++x)
            {
                if (x * x + y * y <= radius * radius)
                {
                    const int px = center_x + x;
                    const int py = center_y + y;
                    if (canvas_->InBounds(px, py))
                    {
                        canvas_->at(px, py) = brush;
                    }
                }
            }
        }
    }
    else
    {
        DrawCircleBresenham(center_x, center_y, radius, brush);
    }
}

void Plotter::FloodFill(int x, int y, const char fill_brush)
{
    if (!canvas_->InBounds(x, y))
        return;

    const char target_brush = canvas_->at(x, y);
    if (target_brush == fill_brush)
        return;

    std::queue<std::pair<int, int>> pixels;
    pixels.emplace(x, y);

    while (!pixels.empty())
    {
        auto [cx, cy] = pixels.front();
        pixels.pop();

        if (!canvas_->InBounds(cx, cy) || canvas_->at(cx, cy) != target_brush)
        {
            continue;
        }

        canvas_->at(cx, cy) = fill_brush;

        pixels.emplace(cx + 1, cy);
        pixels.emplace(cx - 1, cy);
        pixels.emplace(cx, cy + 1);
        pixels.emplace(cx, cy - 1);
    }
}

std::map<char, int> Plotter::ColorHistogram() const
{
    return ColorHistogram(0, 0, canvas_->Width() - 1, canvas_->Height() - 1);
}

std::map<char, int> Plotter::ColorHistogram(const int x1, const int y1,
                                            const int x2, const int y2) const
{
    std::map<char, int> histogram;

    for (int y = y1; y <= y2; ++y)
    {
        for (int x = x1; x <= x2; ++x)
        {
            if (canvas_->InBounds(x, y))
            {
                char color = canvas_->at(x, y);
                histogram[color]++;
            }
        }
    }

    return histogram;
}

std::pair<char, char>
Plotter::MinMaxColors(const std::map<char, int>& color_weights)
{
    if (color_weights.empty())
    {
        return {' ', ' '};
    }

    char min_color = color_weights.begin()->first;
    char max_color = color_weights.begin()->first;
    int min_value = color_weights.begin()->second;
    int max_value = color_weights.begin()->second;

    for (const auto& [color, weight] : color_weights)
    {
        if (weight < min_value)
        {
            min_value = weight;
            min_color = color;
        }
        if (weight > max_value)
        {
            max_value = weight;
            max_color = color;
        }
    }

    return {min_color, max_color};
}

std::unique_ptr<Canvas> Plotter::ExtractRegion(const int x1, const int y1,
                                               const int x2, const int y2) const
{
    int width = x2 - x1 + 1;
    int height = y2 - y1 + 1;

    auto region = std::make_unique<Canvas>(width, height, ' ');

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const int src_x = x1 + x;
            const int src_y = y1 + y;
            if (canvas_->InBounds(src_x, src_y))
            {
                region->at(x, y) = canvas_->at(src_x, src_y);
            }
        }
    }

    return region;
}

void Plotter::PasteRegion(const Canvas& region, const int x, const int y)
{
    for (int ry = 0; ry < region.Height(); ++ry)
    {
        for (int rx = 0; rx < region.Width(); ++rx)
        {
            const int dest_x = x + rx;
            const int dest_y = y + ry;
            if (canvas_->InBounds(dest_x, dest_y))
            {
                (*canvas_)(dest_x, dest_y) = region.at(rx, ry);
            }
        }
    }
}

void Plotter::DrawLineBresenham(int x1, int y1, const int x2, const int y2,
                                const char brush)
{
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    const int sx = x1 < x2 ? 1 : -1;
    const int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        if (canvas_->InBounds(x1, y1))
        {
            (*canvas_)(x1, y1) = brush;
        }

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void Plotter::DrawCircleBresenham(const int center_x, const int center_y,
                                  const int radius, const char brush)
{

    auto draw_circle_points =
        [&](const int cx, const int cy, const int x, const int y)
    {
        if (canvas_->InBounds(cx + x, cy + y))
            (*canvas_)(cx + x, cy + y) = brush;
        if (canvas_->InBounds(cx - x, cy + y))
            (*canvas_)(cx - x, cy + y) = brush;
        if (canvas_->InBounds(cx + x, cy - y))
            (*canvas_)(cx + x, cy - y) = brush;
        if (canvas_->InBounds(cx - x, cy - y))
            (*canvas_)(cx - x, cy - y) = brush;
        if (canvas_->InBounds(cx + y, cy + x))
            (*canvas_)(cx + y, cy + x) = brush;
        if (canvas_->InBounds(cx - y, cy + x))
            (*canvas_)(cx - y, cy + x) = brush;
        if (canvas_->InBounds(cx + y, cy - x))
            (*canvas_)(cx + y, cy - x) = brush;
        if (canvas_->InBounds(cx - y, cy - x))
            (*canvas_)(cx - y, cy - x) = brush;
    };

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    draw_circle_points(center_x, center_y, x, y);

    while (y >= x)
    {
        x++;
        if (d > 0)
        {
            --y;
            d = d + 4 * (x - y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }
        draw_circle_points(center_x, center_y, x, y);
    }
}

void Plotter::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3,
                           const char brush) const
{
    const int min_x = std::min({x1, x2, x3});
    const int max_x = std::max({x1, x2, x3});
    const int min_y = std::min({y1, y2, y3});
    const int max_y = std::max({y1, y2, y3});

    auto edge_function = [](const int x1_, const int y1_, const int x2_,
                            const int y2_, const int x, const int y)
    { return (x - x1_) * (y2_ - y1_) - (y - y1_) * (x2_ - x1_); };

    for (int y = min_y; y <= max_y; ++y)
    {
        for (int x = min_x; x <= max_x; ++x)
        {
            if (!canvas_->InBounds(x, y))
            {
                continue;
            }

            bool inside = true;
            inside &= edge_function(x1, y1, x2, y2, x, y) >= 0;
            inside &= edge_function(x2, y2, x3, y3, x, y) >= 0;
            inside &= edge_function(x3, y3, x1, y1, x, y) >= 0;

            if (inside)
            {
                (*canvas_)(x, y) = brush;
            }
        }
    }
}

void Plotter::ScanlineFill(const int x, const int y, const char fill_brush)
{
    if (!canvas_->InBounds(x, y))
    {
        return;
    }

    const char target_brush = (*canvas_)(x, y);
    if (target_brush == fill_brush)
    {
        return;
    }

    std::stack<ScanlineSegment> segments;

    // Находим начальный горизонтальный отрезок
    int x_start = x;
    int x_end = x;

    // Идем влево
    while (x_start >= 0 && canvas_->at(x_start, y) == target_brush)
    {
        x_start--;
    }
    x_start++;

    // Идем вправо
    while (x_end < canvas_->Width() && canvas_->at(x_end, y) == target_brush)
    {
        x_end++;
    }
    x_end--;

    // Закрашиваем начальный отрезок
    for (int i = x_start; i <= x_end; i++)
    {
        canvas_->at(i, y) = fill_brush;
    }

    // Добавляем сегменты сверху и снизу
    if (y > 0)
    {
        segments.emplace(y - 1, x_start, x_end);
    }
    if (y < canvas_->Height() - 1)
    {
        segments.emplace(y + 1, x_start, x_end);
    }

    while (!segments.empty())
    {
        const auto [y, x_start, x_end] = segments.top();
        segments.pop();

        const int current_y = y;
        int current_x = x_start;

        while (current_x <= x_end)
        {
            // Пропускаем уже закрашенные или неподходящие пиксели
            if (canvas_->at(current_x, current_y) != target_brush)
            {
                current_x++;
                continue;
            }

            // Находим начало нового отрезка
            int new_x_start = current_x;
            while (new_x_start > 0 &&
                   canvas_->at(new_x_start - 1, current_y) == target_brush)
            {
                new_x_start--;
            }

            // Находим конец отрезка
            int new_x_end = current_x;
            while (new_x_end < canvas_->Width() - 1 &&
                   canvas_->at(new_x_end + 1, current_y) == target_brush)
            {
                new_x_end++;
            }

            // Закрашиваем отрезок
            for (int i = new_x_start; i <= new_x_end; i++)
            {
                canvas_->at(i, current_y) = fill_brush;
            }

            // Проверяем соседние строки на наличие новых сегментов
            if (current_y > 0)
            {
                // Для строки выше проверяем, есть ли непрерывные отрезки
                int above_y = current_y - 1;
                int above_x = new_x_start;
                while (above_x <= new_x_end)
                {
                    if (canvas_->at(above_x, above_y) == target_brush)
                    {
                        int above_start = above_x;
                        while (above_x <= new_x_end &&
                               canvas_->at(above_x, above_y) == target_brush)
                        {
                            above_x++;
                        }
                        segments.emplace(above_y, above_start, above_x - 1);
                    }
                    else
                    {
                        above_x++;
                    }
                }
            }

            if (current_y < canvas_->Height() - 1)
            {
                // Для строки ниже проверяем, есть ли непрерывные отрезки
                int below_y = current_y + 1;
                int below_x = new_x_start;
                while (below_x <= new_x_end)
                {
                    if (canvas_->at(below_x, below_y) == target_brush)
                    {
                        int below_start = below_x;
                        while (below_x <= new_x_end &&
                               canvas_->at(below_x, below_y) == target_brush)
                        {
                            below_x++;
                        }
                        segments.emplace(below_y, below_start, below_x - 1);
                    }
                    else
                    {
                        below_x++;
                    }
                }
            }

            // Переходим к следующему потенциальному отрезку
            current_x = new_x_end + 1;
        }
    }
}

} // namespace plotter