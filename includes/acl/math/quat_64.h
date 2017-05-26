#pragma once

////////////////////////////////////////////////////////////////////////////////
// The MIT License (MIT)
//
// Copyright (c) 2017 Nicholas Frechette & Animation Compression Library contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#include "acl/math/math.h"
#include "acl/math/vector4_64.h"

namespace acl
{
	inline Quat_64 quat_set(double x, double y, double z, double w)
	{
#if defined(ACL_SSE2_INTRINSICS)
		return Quat_64{ _mm_set_pd(x, y), _mm_set_pd(z, w) };
#else
		return Quat_64{ x, y, z, w };
#endif
	}

	inline Quat_64 quat_unaligned_load(const double* input)
	{
		return quat_set(input[0], input[1], input[2], input[3]);
	}

	inline Quat_64 quat_64_identity()
	{
		return quat_set(0.0, 0.0, 0.0, 1.0);
	}

	inline Quat_64 quat_cast(const Vector4_64& input)
	{
#if defined(ACL_SSE2_INTRINSICS)
		return Quat_64{ input.xy, input.zw };
#else
		return Quat_64{ input.x, input.y, input.z, input.w };
#endif
	}

	inline double quat_get_x(const Quat_64& input)
	{
#if defined(ACL_SSE2_INTRINSICS)
		return _mm_cvtsd_f64(input.xy);
#else
		return input.x;
#endif
	}

	inline double quat_get_y(const Quat_64& input)
	{
#if defined(ACL_SSE2_INTRINSICS)
		return _mm_cvtsd_f64(_mm_shuffle_pd(input.xy, input.xy, _MM_SHUFFLE(1, 1, 1, 1)));
#else
		return input.y;
#endif
	}

	inline double quat_get_z(const Quat_64& input)
	{
#if defined(ACL_SSE2_INTRINSICS)
		return _mm_cvtsd_f64(input.zw);
#else
		return input.z;
#endif
	}

	inline double quat_get_w(const Quat_64& input)
	{
#if defined(ACL_SSE2_INTRINSICS)
		return _mm_cvtsd_f64(_mm_shuffle_pd(input.zw, input.zw, _MM_SHUFFLE(1, 1, 1, 1)));
#else
		return input.w;
#endif
	}

	inline Quat_64 quat_conjugate(const Quat_64& input)
	{
		return quat_set(-quat_get_x(input), -quat_get_y(input), -quat_get_z(input), quat_get_w(input));
	}

	inline Quat_64 quat_mul(const Quat_64& lhs, const Quat_64& rhs)
	{
		double lhs_x = quat_get_x(lhs);
		double lhs_y = quat_get_y(lhs);
		double lhs_z = quat_get_z(lhs);
		double lhs_w = quat_get_w(lhs);

		double rhs_x = quat_get_x(rhs);
		double rhs_y = quat_get_y(rhs);
		double rhs_z = quat_get_z(rhs);
		double rhs_w = quat_get_w(rhs);

		double x = (rhs_w * lhs_x) + (rhs_x * lhs_w) + (rhs_y * lhs_z) - (rhs_z * lhs_y);
		double y = (rhs_w * lhs_y) - (rhs_x * lhs_z) + (rhs_y * lhs_w) + (rhs_z * lhs_x);
		double z = (rhs_w * lhs_z) + (rhs_x * lhs_y) - (rhs_y * lhs_x) + (rhs_z * lhs_w);
		double w = (rhs_w * lhs_w) - (rhs_x * lhs_x) - (rhs_y * lhs_y) - (rhs_z * lhs_z);

		return quat_set(x, y, z, w);
	}

	inline void quat_to_axis_angle(const Quat_64& input, Vector4_64& out_axis, double& out_angle)
	{
		constexpr double EPSILON = 1.0e-8;
		constexpr double EPSILON_SQUARED = EPSILON * EPSILON;

		double real_length_squared = vector_length_squared3(vector_cast(input));

		if (real_length_squared < EPSILON_SQUARED)
		{
			out_axis = vector_set(1.0, 0.0, 0.0);
			out_angle = 0.0;
		}
		else
		{
			double real_length = sqrt_reciprocal(real_length_squared);
			out_axis = vector_mul(vector_set(quat_get_x(input), quat_get_y(input), quat_get_z(input)), real_length);
			out_angle = abs(quat_get_w(input)) < EPSILON ? ACL_PI_64 : atan2(real_length_squared * real_length, quat_get_w(input)) * 2.0;
		}
	}

	inline Vector4_64 quat_get_axis(const Quat_64& input)
	{
		constexpr double EPSILON = 1.0e-8;
		constexpr double EPSILON_SQUARED = EPSILON * EPSILON;

		double real_length_squared = vector_length_squared3(vector_cast(input));

		if (real_length_squared < EPSILON_SQUARED)
		{
			return vector_set(1.0, 0.0, 0.0);
		}
		else
		{
			double real_length = sqrt_reciprocal(real_length_squared);
			return vector_mul(vector_set(quat_get_x(input), quat_get_y(input), quat_get_z(input)), real_length);
		}
	}

	inline double quat_get_angle(const Quat_64& input)
	{
		constexpr double EPSILON = 1.0e-8;
		constexpr double EPSILON_SQUARED = EPSILON * EPSILON;

		double real_length_squared = vector_length_squared3(vector_cast(input));

		if (real_length_squared < EPSILON_SQUARED)
		{
			return 0.0;
		}
		else
		{
			double real_length = sqrt_reciprocal(real_length_squared);
			return abs(quat_get_w(input)) < EPSILON ? ACL_PI_64 : atan2(real_length_squared * real_length, quat_get_w(input)) * 2.0;
		}
	}
}