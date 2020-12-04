// This file is part of dr4w, a library for computer graphics routines.
//
// Copyright (C) 2020 Mikko Kuitunen <mikko.kuitunen@iki.fi>
//
// This Source Code Form is subject to the terms of the MIT License (see LICENSE.txt)

#pragma once

#include <dr4/dr4_tuples.h>

#include <utility>
#include <vector>

namespace dr4 {

	template<class T>
	class Array2D {
		std::vector<T> m_data;
		size_t m_dim1;
		size_t m_dim2;
	public:

		Array2D(size_t dim1, size_t dim2)
			:m_dim1(dim1), m_dim2(dim2), m_data(dim1* dim2) {
		}

		Array2D(size_t dim1, size_t dim2, T defaultValue)
			:m_dim1(dim1), m_dim2(dim2), m_data(dim1* dim2, defaultValue) {
		}

		Array2D(const std::pair<size_t, size_t>& dims) :Array2D(dims.first, dims.second) {}

		Array2D(const Array2D& rhs)
			: m_data(rhs.m_data), m_dim1(rhs.m_dim1), m_dim2(rhs.m_dim2) {
		}

		Array2D(Array2D&& rhs) noexcept
			: m_data(std::move(rhs.m_data)), m_dim1(rhs.m_dim1), m_dim2(rhs.m_dim2) {
		}

		Array2D& operator=(const Array2D& rhs)
		{
			m_data = rhs.m_data;
			m_dim1 = rhs.m_dim1;
			m_dim2 = rhs.m_dim2;
			return *this;
		}

		Array2D& operator=(Array2D&& rhs)
		{
			m_data = std::move(rhs.m_data);
			m_dim1 = rhs.m_dim1;
			m_dim2 = rhs.m_dim2;
			return *this;
		}

		std::vector<T>& asVector() { return m_data; }

		typename std::vector<T>::iterator begin() { return m_data.begin(); }
		typename std::vector<T>::iterator end() { return m_data.end(); }
		typename std::vector<T>::const_iterator begin() const { return m_data.begin(); }
		typename std::vector<T>::const_iterator end() const { return m_data.end(); }

		inline size_t index2d(size_t x, size_t y) const { return (y * m_dim1) + x; }

		size_t dim1() const { return m_dim1; }
		size_t dim2() const { return m_dim2; }

		std::pair<size_t, size_t> size() const { return { m_dim1, m_dim2 }; }

		size_t sizeInBytes() const { return m_dim1 * m_dim2 * sizeof(T); }

		size_t elementCount() const { return m_dim1 * m_dim2; }

		T* data() { return m_data.data(); }

		const T* data() const { return m_data.data(); }

		T& at(size_t x, size_t y) noexcept { return m_data[index2d(x, y)]; }
		const T& at(size_t x, size_t y) const noexcept { return m_data[index2d(x, y)]; }

		T& at(size_t i) noexcept { return m_data[i]; }
		const T& at(size_t i) const noexcept { return m_data[i]; }

		T& at(const PairIdx& idx) noexcept { return at(idx.x, idx.y); }
		const T& at(const PairIdx& idx) const noexcept { return at(idx.x, idx.y); }

		void set(size_t x, size_t y, const T& value) noexcept { m_data[index2d(x, y)] = value; }
		void set(const PairIdx& idx, const T& value) { set(idx.x, idx.y, value); }

	};

}
