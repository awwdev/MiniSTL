#pragma once
#include "mini/Utils/Types.hpp"
#include <cmath>
#include <cstring>

#define ND [[nodiscard]]

namespace mini
{
    //used for vector access (mat[Vx] == mat[0][0])
    enum V : u8 { Vx = 0, Vy = 1, Vz = 2, Vw = 3 };
}

namespace mini::math
{
    template<class T, u8 Y_T, u8 X_T, bool IsQuat = false, typename = IsArithmetic<T>>
    struct Mat
    {
        static constexpr auto X = X_T;
        static constexpr auto Y = Y_T;

        T cells[Y][X];


        //matrix access [][] 
        inline T*       operator[](const u8 y)       { return cells[y]; }
        inline const T* operator[](const u8 y) const { return cells[y]; } 

        //vector access
        inline T&       operator[](const V  x)       { return cells[0][x]; }
        inline const T& operator[](const V  x) const { return cells[0][x]; } 

        //conversion
        template<class VAL> operator Mat<VAL, Y, X>()
        {
            Mat<VAL, Y, X> out;
            for (u8 y = 0; y < Y; ++y) {
            for (u8 x = 0; x < X; ++x) {
                out[y][x] = static_cast<T>(cells[y][x]);
            }}
            return out;
        }

    };


    template<class T, u8 X>
    using Vec = Mat<T, 1, X>;

    using Mat4f = Mat<f32, 4, 4>;
    using Mat3f = Mat<f32, 3, 3>;

    using Vec2f = Vec<f32, 2>;
    using Vec3f = Vec<f32, 3>;
    using Vec4f = Vec<f32, 4>;

    using Vec2i = Vec<s32, 2>;
    using Vec3i = Vec<s32, 3>;

    using Quatf = Mat<float, 1, 4, true>;

    ///comparsion

    template<class T, u8 Y, u8 X>
    bool operator==(const Mat<T, Y, X>& a, const Mat<T, Y, X>& b)
    {
        return std::memcmp(a.cells, b.cells, Y*X*sizeof(T)) == 0;
    }

    ///identity

    template<class T = f32> ND
    auto Identity4x4() -> Mat<T, 4, 4>
    {
        return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
    }

    template<class T = f32> ND
    auto Identity3x3() -> Mat<T, 3, 3> 
    {
        return {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
        };
    }

    ///addition

    template<class IDX, class VAL, u8 Y, u8 X> ND
    auto operator+(const Mat<IDX, Y, X>& m1, const Mat<VAL, Y, X>& m2) 
    {
        Mat<std::common_type_t<IDX, VAL>, Y, X> out;

        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            out[y][x] = m1[y][x] + m2[y][x];
        }}

        return out;
    }

    template<class IDX, class VAL, u8 Y, u8 X> 
    void operator+=(Mat<IDX, Y, X>& ref, const Mat<VAL, Y, X>& other)
    {
        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            ref[y][x] += other[y][x];
        }}
    }

    ///subtraction

    template<class IDX, class VAL, u8 Y, u8 X> ND
    auto operator-(const Mat<IDX, Y, X>& m1, const Mat<VAL, Y, X>& m2)
    {
        Mat<std::common_type_t<IDX, VAL>, Y, X> out;

        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            out[y][x] = m1[y][x] - m2[y][x];
        }}

        return out;
    }

    template<class IDX, class VAL, u8 Y, u8 X> 
    void operator-=(Mat<IDX, Y, X>& ref, const Mat<VAL, Y, X>& other)
    {
        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            ref[y][x] -= other[y][x];
        }}
    }

    ///scalar multiplication

    template<class T, class S, u8 Y, u8 X> ND
    auto operator*(const Mat<T, Y, X>& m, const S scalar)
    {
        Mat<std::common_type_t<T, S>, Y, X> out;

        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            out[y][x] = m[y][x] * scalar;
        }}

        return out;
    }

    template<class IDX, class S, u8 Y, u8 X>
    void operator*=(Mat<IDX, Y, X>& ref, const S scalar)
    {
        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            ref[y][x] *= scalar;
        }}
    }

    ///scalar division

    template<class T, class S, u8 Y, u8 X> ND
    auto operator/(const Mat<T, Y, X>& m, const S scalar)
    {
        Mat<std::common_type_t<T, S>, Y, X> out;

        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            out[y][x] = m[y][x] / scalar;
        }}

        return out;
    }

    template<class T, class S, u8 Y, u8 X>
    void operator/=(Mat<T, Y, X>& ref, const S scalar)
    {
        for (u8 y = 0; y < Y; ++y) {
        for (u8 x = 0; x < X; ++x) {
            ref[y][x] /= scalar;
        }}
    }

    ///vector magnitude

    template<class T, u8 X, bool IsQuat, typename F = f32> ND
    F Magnitude(const Mat<T, 1, X, IsQuat>& vec)
    {
        T mag = 0;
        for (u8 x = 0; x < X; ++x) {
            mag += vec[0][x] * vec[0][x];
        }

        //todo: faster square root method (like quake sqrt)
        if constexpr (std::is_same_v<F, f32>) return std::sqrtf(mag);
        if constexpr (std::is_same_v<F, f64>) return std::sqrt(mag);
    }

    ///vector normalize

    template<class T, u8 X, typename F = f32> ND
    auto Normalize(const Vec<T, X>& vec)
    {
        const F mag = Magnitude(vec);
        if (mag == 0 || std::isnan(mag)) {
            return {};
        }
            

        Vec<F, X> out;
        for (u8 x = 0; x < X; ++x) {
            out[0][x] = vec[0][x] / mag;
        }

        return out;
    }

    template<class T, u8 X, bool IsQuat>
    void NormalizeThis(Mat<T, 1, X, IsQuat>& ref)
    {
        const auto mag = Magnitude(ref);
        if (mag == 0 || std::isnan(mag)) {
            return;
        }
        for (u8 x = 0; x < X; ++x) {
            ref[0][x] /= mag;
        }
    }

    ///vector dot product

    template<class IDX, class VAL, u8 X> ND
    auto Dot(const Vec<IDX, X>& v1, const Vec<VAL, X>& v2)
    {
        std::common_type_t<IDX, VAL> out = 0; //scalar

        for (u8 x = 0; x < X; ++x) {
            out += v1[0][x] * v2[0][x];
        }

        return out;
    }

    ///vector cross product

    template<class IDX, class VAL> ND
    auto Cross(const Vec<IDX, 3>& v1, const Vec<VAL, 3>& v2) //not commutative
    {
        return Vec<std::common_type_t<IDX, VAL>, 3>
        {
            v1[0][1] * v2[0][2] - v1[0][2] * v2[0][1],
            v1[0][2] * v2[0][0] - v1[0][0] * v2[0][2],
            v1[0][0] * v2[0][1] - v1[0][1] * v2[0][0],
        };
    }

    ///multiplication 

    template<class IDX, class VAL, u8 Y1, u8 X1, u8 X2> ND
    auto operator*(const Mat<IDX, Y1, X1>& m1, const Mat<VAL, X1, X2>& m2) //requirement: columns1 == rows2
    {
        Mat<std::common_type_t<IDX, VAL>, Y1, X2> out {};

        for (u8 y = 0; y < Y1; ++y) {
        for (u8 x = 0; x < X2; ++x) {

            for (u8 c = 0; c < X1; ++c)
                out[y][x] += m2[y][c] * m1[c][x];
            
        }}

        return out;
    }

    template<class IDX, class VAL, u8 Y1, u8 X1, u8 X2>
    void operator*=(Mat<IDX, Y1, X1>& ref, const Mat<VAL, X1, X2>& other)
    {
        ref = ref * other;
    }


    template<class T, auto Y, auto X>
    std::ostream& operator<<(std::ostream& os, const Mat<T, Y, X>& mat)
    {
        os << "mat\n";
        for(auto y = 0; y < Y; ++y) {
        for(auto x = 0; x < X; ++x) {
            os << mat[y][x] << '|';
        }
            os << '\n';
        }
        return os;
    }

    //? ROTATION 

    inline Mat4f RotationMatrixY(const f32 rad)
    {
        const auto c = std::cosf(rad);
        const auto s = std::sinf(rad);
        return {
            c, 0, s, 0,
            0, 1, 0, 0,
           -s, 0, c, 0,
            0, 0, 0, 1,
        };
    }

    inline Mat4f RotationMatrixZ(const f32 rad)
    {
        const auto c = std::cosf(rad);
        const auto s = std::sinf(rad);
        return {
            c,-s, 0, 0,
            s, c, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1,
        };
    }

    inline Mat4f RotationMatrixX(const f32 rad)
    {
        const auto c = std::cosf(rad);
        const auto s = std::sinf(rad);
        return {
            1, 0, 0, 0,
            0, c,-s, 0,
            0, s, c, 0,
            0, 0, 0, 1,
        };
    }

    //Quaternion

    inline Quatf QuatAngleAxis(const float degree, const Vec3f& unitAxis)
    {
        const float radh = ((degree * 3.14f) / 180.f) * 0.5f;
        const float s = std::sinf(radh);
        return {
            unitAxis[Vx] * s,
            unitAxis[Vy] * s,
            unitAxis[Vz] * s,
            std::cosf(radh),
        };
    }

    inline Vec3f operator*(const Vec3f& v, const Quatf& q)
    {
        const Vec3f u { q[Vx], q[Vy], q[Vz] };
        const float s { q[Vw] };
        return { u * 2.0f * Dot(u, v) + v * (s*s - Dot(u, u)) + Cross(u, v) * s * 2.0f };
    }

    Quatf operator*(const Quatf& q1, const Quatf& q2)
    {
        return {
             q1[Vx] * q2[Vw] + q1[Vy] * q2[Vz] - q1[Vz] * q2[Vy] + q1[Vw] * q2[Vx],
            -q1[Vx] * q2[Vz] + q1[Vy] * q2[Vw] + q1[Vz] * q2[Vx] + q1[Vw] * q2[Vy],
             q1[Vx] * q2[Vy] - q1[Vy] * q2[Vx] + q1[Vz] * q2[Vw] + q1[Vw] * q2[Vz],
            -q1[Vx] * q2[Vx] - q1[Vy] * q2[Vy] - q1[Vz] * q2[Vz] + q1[Vw] * q2[Vw],
        };
    }

    inline Mat4f ToMat4(const Quatf& q)
    {
        const float y2 = 2*q[Vy]*q[Vy];
        const float z2 = 2*q[Vz]*q[Vz];
        const float x2 = 2*q[Vx]*q[Vx];
                        
        const float xw = 2*q[Vx]*q[Vw];
        const float xy = 2*q[Vx]*q[Vy];
        const float xz = 2*q[Vx]*q[Vz];
        const float yw = 2*q[Vy]*q[Vw];
        const float yz = 2*q[Vy]*q[Vz];
        const float zw = 2*q[Vz]*q[Vw];

        const float a =  1 - y2 - z2;
        const float b = xy - zw;
        const float c = xz + yw;
        const float d = xy + zw;
        const float e = 1 - x2 - z2;
        const float f = yz - xw;
        const float g = xz - yw;
        const float h = yz + xw;
        const float i = 1 - x2 - y2;

        return {
            a, b, c, 0, 
            d, e, f, 0, 
            g, h, i, 0, 
            0, 0, 0, 1
        };
    }

    /* inline mat4 lookAt(const vec3& from, const vec3& at)
    {
        const vec3 f = normalize(at - from);
              vec3 u = normalize({ 0, 1, 0});
        const vec3 s = normalize(cross(f, u));
        u = cross(s, f);

        const float Tx =-dot(s, from);
        const float Ty =-dot(u, from);
        const float Tz = dot(f, from);

        return { 
            s[Vx], u[Vx],-f[Vx], 0,
            s[Vy], u[Vy],-f[Vy], 0, 
            s[Vz], u[Vz],-f[Vz], 0,
            Tx , Ty , Tz , 1,
        };
    } */


    ///stringify

    //todo: string class (cannot copy char* due to local dtor and char[] not possible)

    //template<class T, u8 Y, u8 X>
    //mini::String ToString(const Mat<T, Y, X>& m)
    //{
    //    const char* out = "Hello Matrix";
    //    for (u8 y = 0; y < Y; ++y)
    //    {
    //        for (u8 x = 0; x < X; ++x)
    //        {
    //            //os << mat[y][x] << '|';
    //        }
    //        //out << '\n';
    //    }
    //    return out;
    //}

}

#undef ND