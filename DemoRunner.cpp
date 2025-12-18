#include "DemoRunner.hpp"
#include "PlotterFactory.hpp"
#include <fstream>
#include <iostream>
#include <chrono>

namespace plotter
{

void DemoRunner::RunAllDemos()
{
    std::cout << "Запускаем все демо\n\n";

    EnsureDemoDirectory();
    DemoBasicPlotter();
    DemoGrayscalePlotter();
    DemoAdvancedGrayscalePlotter();
    DemoFromConfig();
    DemoAdvancedShapes();
    DemoFilters();
    DemoCustomPalettes();
    CompareFillAlgorithms();

    std::cout << "\nВсе демо запущены! Проверь папку Demo, чтобы посмотреть результаты\n";
}

void DemoRunner::EnsureDemoDirectory()
{
    const auto demo_dir = std::filesystem::path("Demo");
    if (!std::filesystem::exists(demo_dir))
    {
        std::filesystem::create_directories(demo_dir);
    }
}

std::filesystem::path DemoRunner::GetDemoPath(const std::string& filename)
{
    return std::filesystem::path("Demo") / filename;
}

void DemoRunner::DemoBasicPlotter()
{
    std::cout << "Запускаем демо базового плоттера...\n";

    // Создаем базовый плоттер
    Plotter plotter(40, 20, ' ');

    // Рисуем простые фигуры
    plotter.DrawRectangle(2, 2, 15, 8, '#');
    plotter.DrawLine(5, 12, 35, 12, '-');
    plotter.DrawCircle(20, 5, 4, '*');
    plotter.DrawTriangle(30, 3, 35, 8, 25, 8, '@');

    // Сохраняем результат
    plotter.SaveToFile(GetDemoPath("basic_plotter.txt"));
    std::cout << "\tСохраняем результат в: Demo/basic_plotter.txt\n";
}

void DemoRunner::DemoGrayscalePlotter()
{
    std::cout << "Запускаем базовое демо в оттенках серого...\n";

    // Создаем grayscale плоттер с палитрой по умолчанию
    GrayscalePlotter plotter(50, 25, ' ');

    // Рисуем с разной яркостью
    plotter.DrawRectangle(5, 5, 20, 12, 0.2); // Темный
    plotter.DrawCircle(35, 10, 6, 0.7); // Средний
    plotter.DrawLine(10, 20, 40, 20, 0.9); // Светлый

    // Градиенты
    plotter.DrawLinearGradient(25, 2, 45, 5, 0.1, 0.9);
    plotter.DrawRadialGradient(40, 18, 4, 1.0, 0.3);

    plotter.SaveToFile(GetDemoPath("grayscale_basic.txt"));
    std::cout << "\tСохраняем результат в: Demo/grayscale_basic.txt\n";
}

void DemoRunner::DemoAdvancedGrayscalePlotter()
{
    std::cout << "Запускаем красивое демо в оттенках серого...\n";
    GrayscalePlotter plotter(60, 30, ' ');

    // Рисуем с разной яркостью
    plotter.DrawRectangle(5, 5, 15, 10, 0.2); // Темно-серый
    plotter.DrawCircle(30, 15, 8, 0.7); // Светло-серый
    plotter.DrawLine(40, 5, 55, 25, 1.0); // Белый

    // Градиенты
    plotter.DrawLinearGradient(20, 20, 40, 25, 0.1, 0.9);
    plotter.DrawRadialGradient(45, 10, 5, 1.0, 0.3);

    // Применяем фильтры
    plotter.ApplyBoxBlur(3);

    // Сохраняем
    plotter.SaveToFile(GetDemoPath("grayscale_advanced.txt"));
    std::cout << "\tСохраняем результат в: Demo/grayscale_advanced.txt\n";
}

void DemoRunner::DemoFromConfig()
{
    std::cout << "Запускам демо с оттенками серого из конфига...\n";

    try
    {
        // Загружаем конфиг для grayscale
        const auto config = Config::LoadFromFile("Demo/grayscale_config.json");
        const auto plotter = PlotterFactory::CreatePlotter(config);

        if (config.plotter_type == "grayscale")
        {
            if (const auto grayscale_plotter = static_cast<GrayscalePlotter*>(plotter.get()))
            {
                grayscale_plotter->DrawRectangle(5, 5, 30, 15, 0.3);
                grayscale_plotter->DrawCircle(50, 10, 8, 0.8);
                grayscale_plotter->DrawLinearGradient(10, 20, 60, 25, 0.1, 0.9);
            }
        }

        plotter->SaveToFile(GetDemoPath("from_config.txt"));
        std::cout << "\tСохраняем результат в: Demo/from_config.txt\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "\tПроизошла ошибка: " << e.what() << "\n";
    }
}

void DemoRunner::DemoAdvancedShapes()
{
    std::cout << "Запускаем демо с дополнительными методами...\n";

    GrayscalePlotter plotter(60, 30, ' ');

    // Создаем сложную композицию
    plotter.DrawRectangle(5, 5, 25, 12, 0.8);
    plotter.DrawRectangle(8, 8, 22, 10, 0.2);

    // Несколько кругов
    plotter.DrawCircle(15, 20, 5, 0.6);
    plotter.DrawCircle(25, 20, 5, 0.6);
    plotter.DrawCircle(35, 20, 5, 0.6);

    // Заливка
    plotter.FloodFill(10, 9, 0.4);

    // Линии-сетка
    for (int i = 0; i < 60; i += 5)
    {
        plotter.DrawLine(i, 25, i, 29, 0.3);
    }

    plotter.SaveToFile(GetDemoPath("advanced_shapes.txt"));
    std::cout << "\tСохраняем результат в: Demo/advanced_shapes.txt\n";
}

void DemoRunner::DemoFilters()
{
    std::cout << "Запускаем демо с фильтрами...\n";

    GrayscalePlotter plotter(50, 25, ' ');

    // Создаем изображение с резкими границами
    plotter.DrawRectangle(10, 5, 25, 15, 0.2);
    plotter.DrawRectangle(30, 8, 45, 20, 0.8);
    plotter.DrawCircle(25, 5, 3, 1.0);

    // Сохраняем оригинал
    plotter.SaveToFile(GetDemoPath("filters_original.txt"));

    // Применяем фильтры
    plotter.ApplyBoxBlur(5);
    plotter.SaveToFile(GetDemoPath("filters_box_blur.txt"));

    // Создаем новое изображение для Gaussian blur
    GrayscalePlotter plotter2(50, 25, ' ');
    plotter2.DrawRectangle(10, 5, 25, 15, 0.2);
    plotter2.DrawRectangle(30, 8, 45, 20, 0.8);
    plotter2.DrawCircle(25, 5, 3, 1.0);

    plotter2.ApplyGaussianBlur(5);
    plotter2.SaveToFile(GetDemoPath("filters_gaussian_blur.txt"));

    std::cout << "\tСохраняем результаты в: Demo/filters_*.txt\n";
}

void DemoRunner::DemoCustomPalettes()
{
    std::cout << "Запускаем демо с кастомными палитрами...\n";

    // Разные палитры
    const std::vector<char> ascii_palette = { ' ', '.', ':', '-', '=', '+', '*', '#', '%', '@' };
    const std::vector<char> simple_palette = { ' ', '+', '#' };

    // Большая ASCII палитра
    GrayscalePlotter ascii_plotter(40, 20, ' ', ascii_palette);
    ascii_plotter.DrawRadialGradient(20, 10, 8, 1.0, 0.1);
    ascii_plotter.SaveToFile(GetDemoPath("palette_ascii.txt"));

    // Простая палитра
    GrayscalePlotter simple_plotter(40, 20, ' ', simple_palette);
    simple_plotter.DrawRadialGradient(20, 10, 8, 1.0, 0.1);
    simple_plotter.SaveToFile(GetDemoPath("palette_simple.txt"));

    // Палитра из конфига
    try
    {
        const auto config = Config::LoadFromFile("Demo/custom_config.json");
        const auto custom_plotter = PlotterFactory::CreatePlotter(config);

        if (const auto grayscale_plotter = static_cast<GrayscalePlotter*>(custom_plotter.get()))
        {
            grayscale_plotter->DrawRadialGradient(25, 12, 10, 1.0, 0.1);
            custom_plotter->SaveToFile(GetDemoPath("palette_from_config.txt"));
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "\tПроизошла ошибка в запуске с конфигом: " << e.what() << "\n";
    }

    std::cout << "\tСохраняем результат в: Demo/palette_*.txt\n";
}

void DemoRunner::CompareFillAlgorithms()
{
    std::cout << "Запускаем демо сравнения алгоритмов заливки...\n";

    Plotter plotter1(50, 30, '.');
    Plotter plotter2(50, 30, '.');

    plotter1.DrawRectangle(5, 5, 25, 20, '#');
    plotter1.DrawCircle(35, 15, 8, '*');

    plotter2.DrawRectangle(5, 5, 25, 20, '#');
    plotter2.DrawCircle(35, 15, 8, '*');
    std::stringstream ss;

    ss << "\nInitial canvas: \n";
    plotter1.Render(ss);

    auto start = std::chrono::high_resolution_clock::now();
    plotter1.FloodFill(10, 10, 'F');
    auto end = std::chrono::high_resolution_clock::now();
    auto floodfill_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    start = std::chrono::high_resolution_clock::now();
    plotter2.ScanlineFill(10, 10, 'S');
    end = std::chrono::high_resolution_clock::now();
    auto scanline_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    ss << "FloodFill time: " << floodfill_time << " microseconds\n";
    ss << "ScanlineFill time: " << scanline_time << " microseconds\n";
    ss << "Speed ratio: " << static_cast<double>(floodfill_time) / static_cast<double>(scanline_time) << "x\n";

    ss << "\nFloodFill result:\n";
    plotter1.Render(ss);

    ss << "\nScanlineFill result:\n";
    plotter2.Render(ss);
    const auto filename = GetDemoPath("scanline_benchmark.txt");
    std::ofstream output(filename, std::ios::out | std::ios::trunc);
    output << ss.str();
    std::cout << "\tСохраняем результат в: Demo/scanline_benchmark.txt";
}

} // namespace plotter