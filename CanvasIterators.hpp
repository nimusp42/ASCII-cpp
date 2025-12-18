#pragma once
#include "Canvas.hpp"

namespace plotter
{

class Canvas::RowIterator
{
public:
    RowIterator(Canvas* canvas, int column, int row)
        : canvas_(canvas), col_(column), row_(row)
    {
    }

    char operator*() const
    {
        return canvas_->at(row_, col_);
    }

    char* operator->() const
    {
        return &canvas_->at(row_, col_);
    }

    char operator[](int shift) const
    {
        return canvas_->at(row_, col_ + shift);
    }

    RowIterator operator++()
    {
        ++col_;
        return *this;
    }

    RowIterator operator++(int)
    {
        RowIterator tmp = *this;
        col_++;
        return tmp;
    }

    RowIterator operator--()
    {
        --col_;
        return *this;
    }

    RowIterator operator--(int)
    {
        RowIterator tmp = *this;
        col_--;
        return tmp;
    }

    RowIterator operator+=(int delta)
    {
        col_ += delta;
        return *this;
    }

    bool operator==(const RowIterator other) const
    {
        return col_ == other.col_ && row_ == other.row_;
    }

    bool operator!=(const RowIterator other) const
    {
        return !(*this == other);
    }

    bool operator<(const RowIterator& other) const
    {
        return col_ < other.col_;
    }

    bool operator<=(const RowIterator& other) const
    {
        return *this < other || *this == other;
    }

    bool operator>(const RowIterator& other) const
    {
        return other < *this;
    }

    bool operator>=(const RowIterator& other) const
    {
        return other < *this || *this == other;
    }

private:
    Canvas* canvas_;
    int row_;
    int col_;
};

class Canvas::ColumnIterator
{
public:
    ColumnIterator(Canvas* canvas, int column, int row)
        : canvas_(canvas), col_(column), row_(row)
    {
    }

    char operator*() const
    {
        return canvas_->at(row_, col_);
    }

    char* operator->() const
    {
        return &canvas_->at(row_, col_);
    }

    char operator[](int shift) const
    {
        return canvas_->at(row_ + shift, col_);
    }

    ColumnIterator& operator++()
    {
        ++row_;
        return *this;
    }

    ColumnIterator operator++(int)
    {
        ColumnIterator tmp = *this;
        row_++;
        return tmp;
    }

    ColumnIterator operator--()
    {
        --row_;
        return *this;
    }

    ColumnIterator operator--(int)
    {
        ColumnIterator tmp = *this;
        row_--;
        return tmp;
    }

    ColumnIterator operator+=(int delta)
    {
        row_ += delta;
        return *this;
    }

    bool operator==(const ColumnIterator& other) const
    {
        return col_ == other.col_ && row_ == other.row_;
    }

    bool operator!=(const ColumnIterator& other) const
    {
        return !(*this == other);
    }

    bool operator<(const ColumnIterator& other) const
    {
        return row_ < other.row_;
    }

    bool operator<=(const ColumnIterator& other) const
    {
        return *this < other || *this == other;
    }

    bool operator>(const ColumnIterator& other) const
    {
        return other < *this;
    }

    bool operator>=(const ColumnIterator& other) const
    {
        return other < *this || *this == other;
    }

private:
    Canvas* canvas_;
    int col_;
    int row_;
};

class Canvas::PixelIterator
{
public:
    PixelIterator(Canvas* canvas, size_t pos) : canvas_(canvas), pos_(pos)
    {
    }

    char& operator*() const
    {
        return canvas_->data_.at(pos_);
    }

    char* operator->() const
    {
        return &canvas_->data_.at(pos_);
    }

    char& operator[](int shift) const
    {
        return canvas_->data_[pos_ + shift];
    }

    PixelIterator& operator++()
    {
        ++pos_;
        return *this;
    }

    PixelIterator operator++(int)
    {
        PixelIterator tmp = *this;
        pos_++;
        return tmp;
    }

    PixelIterator& operator--()
    {
        --pos_;
        return *this;
    }

    PixelIterator operator--(int)
    {
        PixelIterator tmp = *this;
        pos_--;
        return tmp;
    }

    PixelIterator operator+=(int delta)
    {
        pos_ += delta;
        return *this;
    }

    bool operator==(const PixelIterator& other) const
    {
        return pos_ == other.pos_;
    }

    bool operator!=(const PixelIterator& other) const
    {
        return !(*this == other);
    }

    bool operator<(const PixelIterator& other) const
    {
        return pos_ < other.pos_;
    }

    bool operator<=(const PixelIterator& other) const
    {
        return *this < other || *this == other;
    }

    bool operator>(const PixelIterator& other) const
    {
        return other < *this;
    }

    bool operator>=(const PixelIterator& other) const
    {
        return other < *this || *this == other;
    }

private:
    Canvas* canvas_;
    size_t pos_;
};

} // namespace plotter