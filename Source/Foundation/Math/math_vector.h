#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include "math_constants.h"

namespace WMVEngine {

// --== Top-Level data ==--

template <int N, class Real>
class Vector;

typedef Vector<2, int> Vec2i;			// 2-component mathematical vector, single precision
typedef Vector<3, int> Vec3i;			// 3-component mathematical vector, single precision
typedef Vector<2, float> Vec2f;			// 2-component mathematical vector, single precision
typedef Vector<2, double> Vec2d;		// 2-component mathematical vector, double precision
typedef Vector<3, float> Vec3f;			// 3-component mathematical vector, single precision
typedef Vector<3, double> Vec3d;		// 3-component mathematical vector, double precision
typedef Vector<4, float> Vecf;			// 4-component mathematical vector, single precision
typedef Vector<4, double> Vec4d;		// 4-component mathematical vector, double precision

// --== Class Definition ==--

template <int N, class Real>
class Vector
{
	public:
		// Constructors
		Vector ();
		Vector (const Vector& rvec);
		explicit Vector (const Real* rarray);
        
		template <class OtherReal> 
		explicit Vector (const OtherReal* rarray);

		template<int rN, class OtherReal>
		explicit Vector (const Vector<rN, OtherReal>& rvec);

		explicit Vector (Real scalar);
		Vector (Real x, Real y);
		Vector (Real x, Real y, Real z);
		Vector (Real x, Real y, Real z, Real w);

		// Access to vector components
		const Real& operator[] (int i) const;
		Real& operator[] (int i);

		// Assigns the values of a vector to another.
		Vector& operator= (const Vector& rkV);

		Vector& operator= (Real scalar);

		template <class OtherReal> 
		Vector<N, Real>& operator= (const OtherReal* rarray );

		// Copy vectors of different sizes
		template <int rN, class OtherReal> 
		Vector<N,Real>& operator= (const Vector<rN,OtherReal>& rvec);

		// Zeroes the vector components
		void set_zero();

		enum {num_elements = N};

	private:
		// Storage for vector data
		Real data[N];
};


// --== Constructors ==--

template <int N, class Real>
Vector<N,Real>::Vector()
{
	// no initialization for efficiency
}

template <int N, class Real>
Vector<N,Real>::Vector(const Real* rarray)
{
	for (int i = 0; i < N; ++i)
		data[i] = rarray[i];
}

template <int N, class Real>
Vector<N,Real>::Vector(const Vector& rvec)
{
	for (int i = 0; i < N; ++i)
	{
		data[i] = rvec[i];
	}
}

template <int N, class Real>
Vector<N,Real>::Vector (Real scalar)
{
	for (int i = 0; i < N; ++i)
	{
		data[i] = scalar;
	}
}

template <int N, class Real>
Vector<N,Real>::Vector (Real x, Real y)
{
	assert(N >= 2);
	data[0] = x;
	data[1] = y;
}

template <int N, class Real>
Vector<N,Real>::Vector (Real x, Real y, Real z)
{
	assert(N >= 3);
	data[0] = x;
	data[1] = y;
	data[2] = z;
}

template <int N, class Real>
Vector<N,Real>::Vector (Real x, Real y, Real z, Real w)
{
	assert(N >= 4);
	data[0] = x;
	data[1] = y;
	data[2] = z;
	data[3] = w;
}

template <int N, class Real>
template <int rN, class OtherReal>
Vector<N,Real>::Vector (const Vector<rN, OtherReal>& rvec)
{
	int n = (rN >= N) ? N : rN;
	for (int i = 0; i < n; ++i)
		data[i] = static_cast<Real>(rvec[i]);
	for (int i = n; i < N; ++i)
		data[i] = static_cast<Real>(0);
}

template <int N, class Real>
template <class OtherReal>
Vector<N,Real>::Vector (const OtherReal* rarray)
{
	for (int i = 0; i < N; ++i)
		data[i] = static_cast<Real>(rarray[i]);
}


// --== Access Operators ==--

template <int N, class Real>
const Real& Vector<N,Real>::operator[] (int i) const
{
    assert( 0 <= i && i < N && "Out of bounds!");
    return data[i];
}

template <int N, class Real>
Real& Vector<N,Real>::operator[] (int i)
{
    assert( 0 <= i && i < N && "Out of bounds!");
    return data[i];
}


// --== Assignment Operators ==--

template <int N, class Real>
Vector<N,Real>& Vector<N,Real>::operator= (const Vector& rvec)
{
    for (int i = 0; i < N; ++i)
    {
        data[i] = rvec[i];
    }
    return *this;
}

template <int N, class Real>
template <class OtherReal>
Vector<N,Real>& Vector<N,Real>::operator= ( const OtherReal* rarray)
{
    for (int i = 0; i < N; ++i)
        data[i] = static_cast<Real>(rarray[i]);
    return *this;
}

template <int N, class Real>
template <int rN, class OtherReal>
Vector<N,Real>& Vector<N,Real>::operator= ( const Vector<rN,OtherReal>& rvec)
{
    int n = (rN >= N) ? N : rN;
    for (int i = 0; i < n; ++i)
        data[i] = static_cast<Real>(rvec[i]);
    for (int i = n; i < N; ++i)
        data[i] = static_cast<Real>(0);
    return *this;
}

template <int N, class Real>
Vector<N,Real>& Vector<N,Real>::operator= (Real scalar)
{
    for (int i = 0; i < N; ++i)
    {
        data[i] = scalar;
    }
    return *this;
}


// --== Vector Operators ==--

template <int N, class Real>
void Vector<N, Real>::set_zero()
{
    for (int i = 0; i < N; ++i)
        data[i] = 0;
}


// --== Comparison Operators ==--

// true if all components are equal
template <int N, class Real>
bool operator== (const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] != rvec[j]) 
            return false; // found a component that's not equal
    }
    return true; // all components equal
}

// true if all components are equal to a scalar
template <int N, class Real>
bool operator== (const Vector<N,Real>& lvec, const Real scalar)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] != scalar) 
            return false; // found a component that's not equal
    }
    return true; // all components equal
}

// true if any component is not equal
template <int N, class Real>
bool operator!= (const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] != rvec[j]) 
            return true; // found a component that's not equal
    }
    return false; // all components equal
}

// true if any components are not equal to a scalar
template <int N, class Real>
bool operator!= (const Vector<N,Real>& lvec, const Real scalar)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] != scalar) 
            return true; // found a component that's not equal
    }
    return false; // all components equal
}

// we compare vectors like we compare strings
template <int N, class Real>
bool operator< (const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] < rvec[j]) 
            return true;
        else if (lvec[j] > rvec[j])
            return false;
    }
    return false; // all components equal
}

// true if any components are less than a scalar
template <int N, class Real>
bool operator< (const Vector<N,Real>& lvec, Real scalar)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] < scalar) 
            return true;
    }
    return false; // all components greater than or equal to scalar
}

template <int N, class Real>
bool operator<= (const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] < rvec[j]) 
            return true;
        else if (lvec[j] > rvec[j])
            return false;
    }
    return true; // all components less than or equal
}

// True if all components are less than or equal to a scalar
template <int N, class Real>
bool operator<= (const Vector<N,Real>& lvec, Real scalar)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] > scalar)
            return false;
    }
    return true; // all components less than or equal to scalar
}

template <int N, class Real>
bool operator> (const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] > rvec[j]) 
            return true;
        else if (lvec[j] < rvec[j])
            return false;
    }
    return false; // all components equal
}

// true if any components are greater than a scalar
template <int N, class Real>
bool operator> (const Vector<N,Real>& lvec, Real scalar)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] > scalar) 
            return true;
    }
    return false; // all components less than scalar
}

template <int N, class Real>
bool operator>= (const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] > rvec[j]) 
            return true;
        else if (lvec[j] < rvec[j])
            return false;
    }
    return true; // all components equal
}

// True if all components are greater than or equal to a scalar
template <int N, class Real>
bool operator>= (const Vector<N,Real>& lvec, Real scalar)
{
    for (int j=0; j < N; ++j)
    {
        if (lvec[j] < scalar)
            return false;
    }
    return true;
}


// --== Arithmetic Operators ==--

template <int N, class Real>
Vector<N,Real> operator+ (const Vector<N, Real>& lvec, const Vector<N, Real>& rvec)
{
    Vector<N,Real> kSum;
    for (int i = 0; i < N; ++i)
        kSum[i] = lvec[i] + rvec[i];
    return kSum;
}

template <int N, class Real>
Vector<N,Real> operator- (const Vector<N, Real>& lvec, const Vector<N, Real>& rvec)
{
    Vector<N,Real> kDiff;
    for (int i = 0; i < N; ++i)
        kDiff[i] = lvec[i] - rvec[i];
    return kDiff;
}

template <int N, class Real>
Vector<N,Real> operator* (const Vector<N, Real>& lvec, Real scalar)
{
    Vector<N,Real> kprod;
    for (int i = 0; i < N; ++i)
        kprod[i] = scalar * lvec[i];
    return kprod;
}

template <int N, class Real>
Vector<N,Real> operator/ (const Vector<N, Real>& lvec, Real scalar)
{
    Vector<N,Real> quot;
    if ( scalar != static_cast<Real>(0.0) )
    {
        for (int i = 0; i < N; ++i)
            quot[i] = lvec[i] / scalar;
    }
    else
    {
        for (int i = 0; i < N; ++i)
            quot[i] = std::numeric_limits<Real>::max();
    }
    return quot;
}

template <int N> static
Vector<N,float> operator/ (const Vector<N, float>& lvec, float scalar)
{
    Vector<N,float> quot;
    if ( scalar != static_cast<float>(0.0) )
    {
        float finvscalar = (static_cast<float>(1.0))/scalar;
        for (int i = 0; i < N; ++i)
            quot[i] = finvscalar * lvec[i];
    }
    else
    {
        for (int i = 0; i < N; ++i)
            quot[i] = std::numeric_limits<float>::max();
    }
    return quot;
}

template <int N> static
Vector<N,double> operator/ (const Vector<N, double>& lvec, double scalar)
{
    Vector<N,double> quot;
    if ( scalar != static_cast<double>(0.0) )
    {
        double finvscalar = (static_cast<double>(1.0))/scalar;
        for (int i = 0; i < N; ++i)
            quot[i] = finvscalar * lvec[i];
    }
    else
    {
        for (int i = 0; i < N; ++i)
            quot[i] = std::numeric_limits<double>::max();
    }
    return quot;
}

template <int N, class Real>
Vector<N,Real> operator/ (const Vector<N, Real>& lvec, const Vector<N,Real>& rvec)
{
    Vector<N,Real> quot;
    int i;

    for (i = 0; i < N; ++i)
    {
        if( rvec[i] != static_cast<Real>(0.0) )
            quot[i] = lvec[i] / rvec[i];
        else
            quot[i] = std::numeric_limits<Real>::max();
    }

    return quot;
}

template <int N, class Real>
Vector<N,Real> operator- (const Vector<N, Real>& lvec)
{
    Vector<N,Real> neg;
    for (int i = 0; i < N; ++i)
        neg[i] = -lvec[i];
    return neg;
}

template <int N, class Real>
Vector<N,Real> operator* (Real scalar, const Vector<N,Real>& rvec)
{
    Vector<N,Real> prod;
    for (int i = 0; i < N; ++i)
        prod[i] = scalar * rvec[i];
    return prod;
}

template <int N, class Real>
Vector<N,Real> operator* (const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    Vector<N,Real> prod;
    for (int i = 0; i < N; ++i)
        prod[i] = lvec[i] * rvec[i];
    return prod;
}

template <int N, class Real>
Vector<N, Real>& operator+= (Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
        lvec[j] += rvec[j];
    return lvec;
}

template <int N, class Real>
Vector<N, Real>& operator+= (Vector<N,Real>& lvec, const Real* rvec)
{
    for (int j=0; j < N; ++j)
        lvec[j] += rvec[j];
    return lvec;
}

template <int N, class Real>
Vector<N,Real>& operator-= (Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
        lvec[j] -= rvec[j];
    return lvec;
}

template <int N, class Real>
Vector<N,Real>& operator-= (Vector<N,Real>& lvec, const Real* rvec)
{
    for (int j=0; j < N; ++j)
        lvec[j] -= rvec[j];
    return lvec;
}

template <int N, class Real>
Vector<N,Real>& operator*= (Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
        lvec[j] *= rvec[j];
    return lvec;
}

template <int N, class Real>
Vector<N,Real>& operator*= (Vector<N,Real>& lvec, Real scalar)
{
    for (int j=0; j < N; ++j)
        lvec[j] *= scalar;
    return lvec;
}

template <int N, class Real>
Vector<N,Real>& operator/= (Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    for (int j=0; j < N; ++j)
        lvec[j] /= rvec[j];
    return lvec;
}

template <int N, class Real>
Vector<N,Real>& operator/= (Vector<N,Real>& lvec, Real scalar)
{
    for (int j=0; j < N; ++j)
        lvec[j] /= scalar;
    return lvec;
}


// --== Vector Operators ==--

template <int N, class Real>
Real dot(const Vector<N, Real>& lvec, const Vector<N, Real>& rvec)
{
    Real dot = static_cast<Real>(0.0);
    for (int i = 0; i < N; ++i)
        dot += lvec[i] * rvec[i];
    return dot;
}

template <int N, class Real>
Real distance(const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    Real sqrlen = static_cast<Real>(0);
    for (int i = 0; i < N; ++i)
    {
        Real diff = lvec[i] - rvec[i];
        sqrlen += diff * diff;
    }
    return static_cast<Real>(sqrt(static_cast<double>(sqrlen)) );
}

template <int N, class Real>
void to_float_array(float* out, const Vector<N, Real>& vec)
{
    for (int i = 0; i < N; ++i)
        out[i] = static_cast<float>(vec[i]);
}

template <int N, class Real>
void to_double_array(double* out, const Vector<N, Real>& vec)
{
    for (int i = 0; i < N; ++i)
        out[i] = static_cast<double>(vec[i]);
}

template <int N, class Real>
Real angle(const Vector<N,Real>& lvec, const Vector<N,Real>& rvec)
{
    Real l_dot_r = dot(lvec, rvec);
    Real l = length(lvec) * length(rvec);
    if(l <= std::numeric_limits<Real>::epsilon())
        return 0;

    Real cos_angle = l_dot_r / l;
    cos_angle = std::max( std::min(cos_angle, (Real)1), (Real)-1);

    return acos(cos_angle);
}

template <class Real>
Vector<3, Real> cross(const Vector<3, Real>& lvec, const Vector<3, Real>& rvec)
{
    return Vector<3, Real>(
        lvec[1] * rvec[2] - lvec[2] * rvec[1],
        lvec[2] * rvec[0] - lvec[0] * rvec[2],
        lvec[0] * rvec[1] - lvec[1] * rvec[0]
    );
}

template <int N, class Real>
void clamp( Vector<N, Real>& v, Real low, Real high )
{
    for (int i = 0; i < N; ++i)
        v[i] = std::max( std::min( v[i], (Real)high ), (Real)low );
}

template <int N, class Real>
void invert(Vector<N,Real>& v)
{
    for (int i = 0; i < N; ++i)
    {
        // check for division by zero
        if (v[i] == 0.0)
            v[i] = std::numeric_limits<Real>::max();
        else
            v[i] = static_cast<Real>(1) / v[i];
    }
}

// Create a cartesian vector from homogeneous vector
template <int N, class Real>
Vector<N-1,Real> cart (const Vector<N,Real>& v)
{
        Vector<N-1,Real> cartv;
        const Real w = v[N-1];
        if(w != 0)
        {
            for(int i = 0; i < N-1; ++i)
                cartv[i] = v[i] / w;
        }
        return cartv;
    }

// Create a homogeneous vector from cartesion vector
template <int N, class Real>
Vector<N+1,Real> hom (const Vector<N,Real>& v)
{
    Vector<N+1,Real> homv;
    for(int i = 0; i < N; ++i)
        homv[i] = v[i];
    homv[N] = 1;
    return homv;
}

// Normalizes the given vector to a length of 1.0
// Returns the length of the vector before normalization.
template <int N, class Real>
Real normalize (Vector<N,Real>& v)
{
    Real len = length(v);
    int i;

    if ( len > std::numeric_limits<Real>::epsilon() )
    {
        Real inv_len = (static_cast<Real>(1.0))/len;
        for (i = 0; i < N; ++i)
            v[i] *= inv_len;
    }
    else
    {
        len = static_cast<Real>(0.0);
        for (i = 0; i < N; ++i)
            v[i] = static_cast<Real>(0.0);
    }

    return len;
}

template <int N, class Real>
Real length (const Vector<N,Real>& v)
{
    Real sqrlen = static_cast<Real>(0.0);
    for (int i = 0; i < N; ++i)
        sqrlen += v[i]*v[i];
    return static_cast<Real>(sqrt((double)sqrlen));
}

template <int N, class Real>
Real squared_length (const Vector<N,Real>& v)
{
    Real sqrlen = static_cast<Real>(0.0);
    for (int i = 0; i < N; ++i)
        sqrlen += v[i]*v[i];
    return static_cast<Real>(sqrlen);
}

template <int N, class T>
Vector<N, T> vmin(const Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    Vector<N, T> min_vec(lhs);
    for (int j=0; j < 3; ++j)
    {
        if (rhs[j] < min_vec[j])
            min_vec[j] = rhs[j];
    }
    return min_vec;
}

template <int N, class T>
Vector<N, T> vmax(const Vector<N, T>& lhs, const Vector<N, T>& rhs)
{
    Vector<N, T> max_vec(lhs);
    for (int j=0; j < 3; ++j)
    {
        if (rhs[j] > max_vec[j])
            max_vec[j] = rhs[j];
    }
    return max_vec;
}


// --== Conversion Functions ==--

template <class Real>
Real hpb_to_axis_angle(const Vector<3, Real>& hpb, Vector<3, Real>& axis)
{
    Real ch = cos(hpb[0] / 2);
    Real sh = sin(hpb[0] / 2);
    Real cp = cos(hpb[1] / 2);
    Real sp = sin(hpb[1] / 2);
    Real cb = cos(hpb[2] / 2);
    Real sb = sin(hpb[2] / 2);

    axis[0] = ch*sp*cb + sh*cp*sb;
    axis[1] = sh*cp*cb - ch*sp*sb;
    axis[2] = ch*cp*sb - sh*sp*cb;
    Real l = normalize(axis);
    if(l < std::numeric_limits<float>::epsilon())
    {
        return 0;
    }
    else
    {
        Real angle_a = ch*cp*cb + sh*sp*sb;
        if(angle_a < -1)
            angle_a = -1;
        else if(angle_a > 1)
            angle_a = 1;
        return 2 * acos(angle_a);
    }
}

template <class Real>
Vector<3, Real> axis_angle_to_hpb(const Vector<3, Real>& axis, Real angle)
{
    Vector<3, Real> hpb;

    Real s = sin(angle);
    Real c = cos(angle);
    Real t = 1 - c;

    hpb[0] = atan2(axis[0] * axis[2] * t + axis[1] * s, 1 - (axis[0] * axis[0] + axis[1] * axis[1]) * t);

    Real sp = axis[0] * s - axis[1] * axis[2] * t;
    if(sp < -1) sp = -1;
    else if(sp > 1) sp = 1;
    hpb[1] = asin(sp);

    hpb[2] = atan2(axis[0] * axis[1] * t + axis[2] * s, 1 - (axis[0] * axis[0] + axis[2] * axis[2]) * t);

    return hpb;
}

template <class Real>
Vector<2, Real> vector_to_hp(const Vector<3, Real>& v)
{
    Vector<2, Real> ret;
    if(v[0] == 0.0 && v[2] == 0.0)
    {
        ret[0] = 0.0;
        if(v[1] != 0.0)
            ret[1] = (v[1] < 0.0) ? HALFPI : -HALFPI;
        else
            ret[1] = 0.0;
    } else {
        if(v[2] == 0.0)
            ret[0] = (v[0] < 0.0) ? -HALFPI : HALFPI;
        else
            ret[0] = atan2(v[0], v[2]);
        Real hyp = sqrt(v[0] * v[0] + v[2] * v[2]);
        if(hyp == 0.0)
            ret[1] = (v[1] < 0.0f) ? HALFPI : -HALFPI;
        else
            ret[1] = -atan2(v[1], hyp);
    }
    return ret;
}

}	// Namespace

#endif
