#ifndef MATH_MATRIX_H
#define MATH_MATRIX_H

#include "math_constants.h"
#include "math_vector.h"


// --== Top-Level data ==--

template <int N, class Real>
class Matrix;

typedef Matrix<3, float>  Matrix3f;		// 3x3 Matrix, single precision
typedef Matrix<3, double> Matrix3d;		// 3x3 Matrix, double precision
typedef Matrix<4, float>  Matrix4f;		// 4x4 Matrix, single precision
typedef Matrix<4, double> Matrix4d;		// 4x4 Matrix, double precision


// --== Class Definition ==--

template <int N, class Real>
class Matrix
{
public:
    static const Matrix<N, Real> zero;
    static const Matrix<N, Real> identity;

    // Constructors
    Matrix();							// Uninitialized Constructor
    Matrix(const Matrix& matrix);		// Copy Constructor
	Matrix(const Real* rarray);			// Takes the provided array of floating point numbers for initialization

    template <class OtherReal>
    explicit Matrix (const Matrix<N, OtherReal>& rmatrix);	// Constructs matrix from another of equal size, but different type

    // Copy assignment
    Matrix<N, Real>& operator= (const Matrix<N, Real>& source);

    // Member access
    const Real* operator[] (int iRow) const;
    Real* operator[] (int iRow);

    Vector<N, Real> column(int column_index);
    void set_column(int column_index, Vector<N, Real> vec);

    Real* data();
    const Real* data() const;

    // Matrix operations
    Matrix<N,Real> transpose() const;
    void make_zero();
    void make_identity();

private:
    // Data member
    Real m_matrix[N*N];
};


// --== Constants ==--

namespace detail
{
    template <int N, class Real>
    Matrix<N, Real> compute_zero_matrix( void)
    {
        Matrix<N, Real> m;
        memset(m.data(), 0, N*N*sizeof(Real));
        return m;
    }

    template <int N, class Real>
    Matrix<N, Real> compute_identity_matrix( void)
    {
        Matrix<N, Real> m(compute_zero_matrix<N, Real>());
        for (int i = 0; i < N; i++)
        {
            m.data()[N*i + i] = static_cast<Real>(1);
        }
        return m;
    }
}
    
template <int N, class Real>
const Matrix<N, Real> Matrix<N, Real>::zero = detail::compute_zero_matrix<N, Real>();

template <int N, class Real>
const Matrix<N, Real> Matrix<N, Real>::identity = detail::compute_identity_matrix<N, Real>();


// --== Constructors ==--

template <int N, class Real>
Matrix<N,Real>::Matrix ()
{
    // uninitialized for efficiency
}

template <int N, class Real>
Matrix<N,Real>::Matrix (const Matrix& matrix)
{
    memcpy(m_matrix, matrix.m_matrix, N*N*sizeof(Real));
}

template <int N, class Real>
Matrix<N,Real>::Matrix(const Real* rarray)
{
    memcpy(m_matrix, rarray, N*N*sizeof(Real));
}

template <int N, class Real>
template <class OtherReal>
Matrix<N,Real>::Matrix (const Matrix<N, OtherReal>& rmatrix)
{
    const OtherReal* rdata = rmatrix.data();
    for(int n = 0; n < N*N; n++)
        m_matrix[n] = static_cast<Real>(rdata[n]);
}


// --== Copier ==--

template <int N, class Real>
Matrix<N, Real>& Matrix<N, Real>::operator= (const Matrix<N, Real>& source)
{
    if (this != &source)
        memcpy(m_matrix, source.m_matrix, N*N*sizeof(Real) );
    return *this;
}


// --== Member access ==--

template <int N, class Real>
const Real* Matrix<N,Real>::operator[] (int iRow) const
{
    assert( 0 <= iRow && iRow < N );
    return &m_matrix[N*iRow];
}

template <int N, class Real>
Real* Matrix<N,Real>::operator[] (int iRow)
{
    assert( 0 <= iRow && iRow < N );
    return &m_matrix[N*iRow];
}

template <int N, class Real>
Real* Matrix<N,Real>::data()
{
    return m_matrix;
}

template <int N, class Real>
const Real* Matrix<N,Real>::data() const
{
    return m_matrix;
}

template <int N, class Real>
Vector<N, Real> Matrix<N,Real>::column(int column_index)
{
    assert(column_index < N);
    Vector<N, Real> result;
    for (int i = 0; i < N; i++)
        result[i] = m_matrix[N*i + column_index];

    return result;
}

template <int N, class Real>
void Matrix<N,Real>::set_column(int column_index, Vector<N, Real> vec)
{
    assert(column_index < N);
    for (int i = 0; i < N; i++)
        m_matrix[N*i + column_index] = vec[i];
}

// --== Matrix Functions ==--

template <int N, class Real>
Matrix<N,Real> Matrix<N,Real>::transpose () const
{
    Matrix<N,Real> transposed;
    for (int irow = 0; irow < N; irow++)
    {
        for (int icol = 0; icol < N; icol++)
            transposed.m_matrix[irow*N + icol] = m_matrix[icol*N + irow];
    }
    return transposed;
}

template <int N, class Real>
void Matrix<N,Real>::make_zero ()
{
    *this = zero;
}

template <int N, class Real>
void Matrix<N,Real>::make_identity ()
{
    *this = identity;
}


// --== Arithmetic Operators ==--

template <int N, class Real>
Vector<N,Real> operator* (const Matrix<N,Real>& m, const Vector<N,Real>& vec)
{
    Vector<N,Real> prod;
    for (int irow = 0; irow < N; irow++)
    {
        prod[irow] = static_cast<Real>(0.0);
        for (int icol = 0; icol < N; icol++)
            prod[irow] += m[irow][icol] * vec[icol];
    }
    return prod;
}


// --== Matrix operations ==--

template <int N, class Real>
Matrix<N,Real> operator* (const Matrix<N,Real>& m, const Matrix<N,Real>& mat)
{
    Matrix<N,Real> prod;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            prod[j][i] = 0;
            for (int k = 0; k < N; k++)
                prod[j][i] += m.data()[j*N + k] * mat[k][i];
         }

    return prod;
}

template <int N, class Real>
Matrix<N,Real> operator* (const Matrix<N,Real>& m, Real scalar)
{
    Matrix<N,Real> scaled;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            scaled[i][j] = m.data()[i*N+j] * scalar;

    return scaled;
}

template <int N, class Real>
Matrix<N,Real> operator+ (const Matrix<N,Real>& m, const Matrix<N,Real>& mat)
{
    Matrix<N,Real> sum;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            sum[i][j] = m.data()[i*N+j] + mat[i][j];

    return sum;
}

template <int N, class Real>
Matrix<N,Real> operator- (const Matrix<N,Real>& m, const Matrix<N,Real>& mat)
{
    Matrix<N,Real> sum;

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            sum[i][j] = m.data()[i*N+j] - mat[i][j];

    return sum;
}


// --== Helper Functions ==--

// Copies the matrix into a buffer.
// Make sure that the buffer is big enough to hold all the matrix data.
template <int N, class type1, class type2>
void CopyMatrix(type1* buffer, Matrix<N,type2>& matrix)
{
    for (int irow = 0; irow < N; irow++)
    {
        for (int icol = 0; icol < N; icol++)
        {
            buffer[irow*N + icol] = static_cast<type1>(matrix[irow][icol]);
        }
    }
}

// Creates a square matrix from the outer product of two vectors
template <int N, class Real>
Matrix<N,Real> OuterProduct(Vector<N,Real>& v1, Vector<N,Real>& v2)
{
    Matrix<N,Real> m;
    for (int irow = 0; irow < N; irow++)
    {
        for (int icol = 0; icol < N; icol++)
        {
            m[irow][icol] = v1[irow] * v2[icol];
        }
    }
    return m;
}


// --== Comparison Operators ==--

// true if all components are equal
template <int N, class Real>
bool operator == (const Matrix<N,Real>& lmat, const Matrix<N,Real>& rmat)
{
    for (int j = 0; j < N; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            if (lmat[i][j] != rmat[i][j])
                return false; // found a component that's not equal
        }
    }
    return true; // all components equal
}

// true if one or more components are not equal
template <int N, class Real>
bool operator != (const Matrix<N,Real>& lmat, const Matrix<N,Real>& rmat)
{
    return !(lmat == rmat);
}


// --== Matrix4 operations ==--

template <class Real>
Matrix<4, Real> inverse(const Matrix<4, Real>& source)
{
    const Real* m_array = source.data();
    Real fA0 = m_array[ 0]*m_array[ 5] - m_array[ 1]*m_array[ 4];
    Real fA1 = m_array[ 0]*m_array[ 6] - m_array[ 2]*m_array[ 4];
    Real fA2 = m_array[ 0]*m_array[ 7] - m_array[ 3]*m_array[ 4];
    Real fA3 = m_array[ 1]*m_array[ 6] - m_array[ 2]*m_array[ 5];
    Real fA4 = m_array[ 1]*m_array[ 7] - m_array[ 3]*m_array[ 5];
    Real fA5 = m_array[ 2]*m_array[ 7] - m_array[ 3]*m_array[ 6];
    Real fB0 = m_array[ 8]*m_array[13] - m_array[ 9]*m_array[12];
    Real fB1 = m_array[ 8]*m_array[14] - m_array[10]*m_array[12];
    Real fB2 = m_array[ 8]*m_array[15] - m_array[11]*m_array[12];
    Real fB3 = m_array[ 9]*m_array[14] - m_array[10]*m_array[13];
    Real fB4 = m_array[ 9]*m_array[15] - m_array[11]*m_array[13];
    Real fB5 = m_array[10]*m_array[15] - m_array[11]*m_array[14];

    Real det = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;

    Matrix<4, Real> inv;
    if ( static_cast<Real>(fabs(static_cast<double>(det))) <= std::numeric_limits<Real>::epsilon() )
    {
        inv.make_zero();
        return inv;
    }

    inv[0][0] = + m_array[ 5]*fB5 - m_array[ 6]*fB4 + m_array[ 7]*fB3;
    inv[1][0] = - m_array[ 4]*fB5 + m_array[ 6]*fB2 - m_array[ 7]*fB1;
    inv[2][0] = + m_array[ 4]*fB4 - m_array[ 5]*fB2 + m_array[ 7]*fB0;
    inv[3][0] = - m_array[ 4]*fB3 + m_array[ 5]*fB1 - m_array[ 6]*fB0;
    inv[0][1] = - m_array[ 1]*fB5 + m_array[ 2]*fB4 - m_array[ 3]*fB3;
    inv[1][1] = + m_array[ 0]*fB5 - m_array[ 2]*fB2 + m_array[ 3]*fB1;
    inv[2][1] = - m_array[ 0]*fB4 + m_array[ 1]*fB2 - m_array[ 3]*fB0;
    inv[3][1] = + m_array[ 0]*fB3 - m_array[ 1]*fB1 + m_array[ 2]*fB0;
    inv[0][2] = + m_array[13]*fA5 - m_array[14]*fA4 + m_array[15]*fA3;
    inv[1][2] = - m_array[12]*fA5 + m_array[14]*fA2 - m_array[15]*fA1;
    inv[2][2] = + m_array[12]*fA4 - m_array[13]*fA2 + m_array[15]*fA0;
    inv[3][2] = - m_array[12]*fA3 + m_array[13]*fA1 - m_array[14]*fA0;
    inv[0][3] = - m_array[ 9]*fA5 + m_array[10]*fA4 - m_array[11]*fA3;
    inv[1][3] = + m_array[ 8]*fA5 - m_array[10]*fA2 + m_array[11]*fA1;
    inv[2][3] = - m_array[ 8]*fA4 + m_array[ 9]*fA2 - m_array[11]*fA0;
    inv[3][3] = + m_array[ 8]*fA3 - m_array[ 9]*fA1 + m_array[10]*fA0;

    Real inv_det = (static_cast<Real>(1.0))/det;
    for (int irow = 0; irow < 4; irow++)
    {
        for (int icol = 0; icol < 4; icol++)
            inv[irow][icol] *= inv_det;
    }

    return inv;
}

template <class Real>
Matrix<4, Real> position_matrix(Real x, Real y, Real z)
{
    Matrix<4, Real> dest = Matrix<4, Real>::identity;
    dest[0][3] = x;
    dest[1][3] = y;
    dest[2][3] = z;
    return dest;
}

template <class Real>
Matrix<4, Real> position_matrix(const Vector<3, Real>& vec)
{
    Matrix<4, Real> dest = Matrix<4, Real>::identity;
    dest[0][3] = vec[0];
    dest[1][3] = vec[1];
    dest[2][3] = vec[2];
    return dest;
}

template <class Real>
Matrix<4, Real> rotation_matrix_hpb(Real h, Real p, Real b)
{
    Matrix<4, Real> dest = Matrix<4, Real>::identity;
    Real sinh = sin(h);
    Real cosh = cos(h);
    Real sinp = sin(p);
    Real cosp = cos(p);
    Real sinb = sin(b);
    Real cosb = cos(b);
    dest[0][0] = cosh * cosb + sinh * sinp * sinb;
    dest[1][0] = cosp * sinb;
    dest[2][0] = -sinh * cosb + cosh * sinp * sinb;
    dest[0][1] = -cosh * sinb + sinh * sinp * cosb;
    dest[1][1] = cosp * cosb;
    dest[2][1] = sinh * sinb + cosh * sinp * cosb;
    dest[0][2] = sinh * cosp;
    dest[1][2] = -sinp;
    dest[2][2] = cosh * cosp;
    return dest;
}

template <class Real>
Matrix<4, Real> rotation_matrix_hpb(const Vector<3, Real>& vec)
{
    return rotation_matrix_hpb(vec[0], vec[1], vec[2]);
}


template <class Real>
Matrix<3, Real> rotation_matrix_hpb3(Real h, Real p, Real b)
{
    Matrix<3, Real> dest = Matrix<3, Real>::identity;
    Real sinh = sin(h);
    Real cosh = cos(h);
    Real sinp = sin(p);
    Real cosp = cos(p);
    Real sinb = sin(b);
    Real cosb = cos(b);
    dest[0][0] = cosh * cosb + sinh * sinp * sinb;
    dest[1][0] = cosp * sinb;
    dest[2][0] = -sinh * cosb + cosh * sinp * sinb;
    dest[0][1] = -cosh * sinb + sinh * sinp * cosb;
    dest[1][1] = cosp * cosb;
    dest[2][1] = sinh * sinb + cosh * sinp * cosb;
    dest[0][2] = sinh * cosp;
    dest[1][2] = -sinp;
    dest[2][2] = cosh * cosp;
    return dest;
}

template <class Real>
Matrix<3, Real> rotation_matrix_hpb3(const Vector<3, Real>& vec)
{
    return rotation_matrix_hpb3(vec[0], vec[1], vec[2]);
}

template <class Real>
Matrix<4, Real> rotation_matrix(Real x, Real y, Real z)
{
    return rotation_matrix_hpb(y, x, z);
}

template <class Real>
Matrix<4, Real> rotation_matrix(Real x, Real y, Real z, Real angle)
{
    /*
        R = u u^T + cos theta (I - u u^T) + sin theta S
          = u u^T + I cos(theta) - u u^T cos(theta) + S sin(theta)
          = u u^t (1 - cos(theta)) + I cos(theta) + S sin(theta)
        u = v/||v|| = (x' y' z')^T
        u u^T = (x'x'  x'y'  x'z')
                (y'x'  y'y'  y'z')
                (z'x'  z'y'  z'z')
        S = ( 0  -z'  y')
            ( z'  0  -x')
            (-y'  x'  0 )

    */
    Matrix<4, Real> dest = Matrix<4, Real>::identity;

    Real l = sqrt(x*x + y*y + z*z);
    if(l == 0)
        return dest;

    x /= l;
    y /= l;
    z /= l;

    Real sina = sin(angle);
    Real cosa = cos(angle);

    dest[0][0] = x*x * (1 - cosa) + cosa;
    dest[0][1] = x*y * (1 - cosa)         - z * sina;
    dest[0][2] = x*z * (1 - cosa)         + y * sina;

    dest[1][0] = y*x * (1 - cosa)         + z * sina;
    dest[1][1] = y*y * (1 - cosa) + cosa;
    dest[1][2] = y*z * (1 - cosa)         - x * sina;

    dest[2][0] = z*x * (1 - cosa)         - y * sina;
    dest[2][1] = z*y * (1 - cosa)         + x * sina;
    dest[2][2] = z*z * (1 - cosa) + cosa;

    return dest;
}

template <class Real>
Matrix<4, Real> rotation_matrix(const Vector<3, Real>& axis, Real angle)
{
    return rotation_matrix(axis[0], axis[1], axis[2], angle);
}

template <class Real>
Matrix<4, Real> scale_matrix(Real sx, Real sy, Real sz)
{
    Matrix<4, Real> dest = Matrix<4, Real>::identity;
    dest[0][0] = sx;
    dest[1][1] = sy;
    dest[2][2] = sz;
    return dest;
}

template <class Real>
Matrix<4, Real> scale_matrix(const Vector<3, Real>& vec)
{
    return scale_matrix(vec[0], vec[1], vec[2]);
}

template <class Real>
Matrix<4, Real> scale_matrix(Real sx, Real sy, Real sz, const Vector<3, Real>& origin)
{
    return position_matrix(origin) * scale_matrix(sx, sy, sz) * position_matrix(-origin);
}

template <class Real>
Matrix<4, Real> look_at_matrix(Real x, Real y, Real z, Real at_x, Real at_y, Real at_z, bool camera_like = false)
{
    // Note that cameras look in the direction opposite to forward
    Vector<3, Real> forward(at_x-x, at_y-y, at_z-z);
    if(camera_like)
        forward = -forward;
    normalize(forward);

    Vector<3, Real> right(cross(Vector<3, Real>(0,1,0), forward));
    if(normalize(right) == 0)
        right = Vector<3, Real>(1,0,0);
    Vector<3, Real> up(cross(forward, right));
    normalize(up);

    Matrix<4, Real> dest;
    dest[0][0] = right[0];
    dest[1][0] = right[1];
    dest[2][0] = right[2];
    dest[3][0] = 0;
    dest[0][1] = up[0];
    dest[1][1] = up[1];
    dest[2][1] = up[2];
    dest[3][1] = 0;
    dest[0][2] = forward[0];
    dest[1][2] = forward[1];
    dest[2][2] = forward[2];
    dest[3][2] = 0;
    dest[0][3] = x;
    dest[1][3] = y;
    dest[2][3] = z;
    dest[3][3] = 1;

    return dest;
}

template <class Real>
Matrix<4, Real> look_at_matrix(Real x, Real y, Real z, Real at_x, Real at_y, Real at_z, Real up_x, Real up_y, Real up_z,
                               bool camera_like = false)
{
    // Note that cameras look in the direction opposite to forward
    Vector<3, Real> forward(at_x-x, at_y-y, at_z-z);
    if(camera_like)
        forward = -forward;
    normalize(forward);

    Vector<3, Real> right(cross(Vector<3, Real>(up_x, up_y, up_z), forward));
    if(normalize(right) == 0)
        right = Vector<3, Real>(1,0,0);

    Matrix<4, Real> dest;
    dest[0][0] = right[0];
    dest[1][0] = right[1];
    dest[2][0] = right[2];
    dest[3][0] = 0;
    dest[0][1] = up_x;
    dest[1][1] = up_y;
    dest[2][1] = up_z;
    dest[3][1] = 0;
    dest[0][2] = forward[0];
    dest[1][2] = forward[1];
    dest[2][2] = forward[2];
    dest[3][2] = 0;
    dest[0][3] = x;
    dest[1][3] = y;
    dest[2][3] = z;
    dest[3][3] = 1;

    return dest;
}

template <class Real>
Matrix<4, Real> coordsys_matrix(const Vector<3, Real>& x_axis, const Vector<3, Real>& y_axis, const Vector<3, Real>& z_axis, const Vector<3, Real>& pos = Vector<3,Real>(0,0,0))
{
    Matrix<4, Real> dest;
    dest[0][0] = x_axis[0];
    dest[1][0] = x_axis[1];
    dest[2][0] = x_axis[2];
    dest[3][0] = 0;
    dest[0][1] = y_axis[0];
    dest[1][1] = y_axis[1];
    dest[2][1] = y_axis[2];
    dest[3][1] = 0;
    dest[0][2] = z_axis[0];
    dest[1][2] = z_axis[1];
    dest[2][2] = z_axis[2];
    dest[3][2] = 0;
    dest[0][3] = pos[0];
    dest[1][3] = pos[1];
    dest[2][3] = pos[2];
    dest[3][3] = 1;

    return dest;
}

template <class Real>
static Vector<3, Real> matrix_to_hpb(const Matrix<4, Real>& mat)
{
    Vector<3, Real> y(mat[0][1], mat[1][1], mat[2][1]);
    Vector<3, Real> z(mat[0][2], mat[1][2], mat[2][2]);

    Vector<3, Real> hpb;

    // First get RX and RY
    bool zzero[3] = {
        (fabs(z[0]) <= std::numeric_limits<float>::epsilon()),
        (fabs(z[1]) <= std::numeric_limits<float>::epsilon()),
        (fabs(z[2]) <= std::numeric_limits<float>::epsilon())
    };

    if (zzero[0] && zzero[2])
    {
        hpb[0] = 0;
        if (!zzero[1])
            hpb[1] = (z[1] < 0) ? HALFPI : -HALFPI;
        else
            hpb[1] = 0;
    }
    else
    {
        if (zzero[2])
            hpb[0] = (z[0] < 0) ? -HALFPI : HALFPI;
        else
            hpb[0] = atan2(z[0], z[2]);
        Real hyp = sqrt(z[0] * z[0] + z[2] * z[2]);
        if (hyp <= std::numeric_limits<float>::epsilon())
            hpb[1] = (z[1] < 0.0) ? HALFPI : -HALFPI;
        else
            hpb[1] = -atan2(z[1], hyp);
    }

    // Find RZ
    Matrix<4,Real> rot_hp(rotation_matrix_hpb<Real>(hpb[0], hpb[1], 0));
    Vector<3, Real> rot_y(inverse(rot_hp) * Vector<4,Real>(y));
    bool rot_yzero[3] = {
        (fabs(rot_y[0]) <= std::numeric_limits<float>::epsilon()),
        (fabs(rot_y[1]) <= std::numeric_limits<float>::epsilon()),
        (fabs(rot_y[2]) <= std::numeric_limits<float>::epsilon())
    };
    if(rot_yzero[0] && rot_yzero[1])
        hpb[2] = 0;
    else if(rot_yzero[1])
        hpb[2] = (rot_y[0] < 0) ? HALFPI : -HALFPI;
    else
        hpb[2] = atan2(-rot_y[0], rot_y[1]);

    return hpb;
}

// Note that the RotationOrder enum is set up so that reverse order = (eNumRotationOrders-1) - order
enum RotationOrder
{
    eRotationOrderXYZ,
    eRotationOrderYZX,
    eRotationOrderZXY,
    eRotationOrderYXZ, eRotationOrderHPB = eRotationOrderYXZ,
    eRotationOrderXZY,
    eRotationOrderZYX,
    eNumRotationOrders
};

static const int RotationPermutation[eNumRotationOrders][3] = {
    { 0, 1, 2 },    // XYZ
    { 1, 2, 0 },    // YZX
    { 2, 0, 1 },    // ZXY
    { 1, 0, 2 },    // YXZ
    { 0, 2, 1 },    // XZY
    { 2, 1, 0 }     // ZYX
};

template <class Real>
static Matrix<4, Real> rotation_matrix_xyz(const Vector<3, Real>& xyz, RotationOrder order)
{
    const int& k = RotationPermutation[order][0];
    const int& j = RotationPermutation[order][1];
    const int& i = RotationPermutation[order][2];

    Real sink = sin(xyz[k]);
    Real cosk = cos(xyz[k]);
    Real sinj = sin(xyz[j]);
    Real cosj = cos(xyz[j]);
    Real sini = sin(xyz[i]);
    Real cosi = cos(xyz[i]);

    if((j == k+1) || (i == j+1))
    {
        sink = -sink;
        sinj = -sinj;
        sini = -sini;
    }

    Matrix<4, Real> dest = Matrix<4, Real>::identity;
    dest[i][i] = cosk * cosj;
    dest[i][j] = -sink * cosi + cosk * sinj * sini;
    dest[i][k] = sink * sini + cosk * sinj * cosi;
    dest[j][i] = sink * cosj;
    dest[j][j] = cosk * cosi + sink * sinj * sini;
    dest[j][k] = -cosk * sini + sink * sinj * cosi;
    dest[k][i] = -sinj;
    dest[k][j] = cosj * sini;
    dest[k][k] = cosj * cosi;

    return dest;
}

template <class Real>
static Vector<3, Real> matrix_to_xyz_rotation(const Matrix<4, Real>& mat, RotationOrder order)
{
    const int& k = RotationPermutation[order][0];
    const int& j = RotationPermutation[order][1];
    const int& i = RotationPermutation[order][2];

    Vector<3, Real> xyz;

    Real hyp = sqrt(mat[i][i] * mat[i][i] + mat[j][i] * mat[j][i]);
    if (hyp <= std::numeric_limits<float>::epsilon())
    {
        xyz[i] = atan2(-mat[j][k], mat[j][j]);
        xyz[j] = atan2(-mat[k][i], hyp);
        xyz[k] = 0;
    }
    else
    {
        xyz[i] = atan2(mat[k][j], mat[k][k]);
        xyz[j] = atan2(-mat[k][i], hyp);
        xyz[k] = atan2(mat[j][i], mat[i][i]);
    }

    if((j == k+1) || (i == j+1))
        return -xyz;
    else
        return xyz;
}

template <class Real>
static Real matrix_to_axis_angle(const Matrix<4, Real>& mat, Vector<3, Real>& axis)
{
    // Do a matrix_to_hpb, then convert the Euler rotation to axis-angle

    const Vector<3, Real> hpb = matrix_to_hpb(mat);

    // Convert Euler to axis-angle
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

// NOTE: with_sign assumes no significant rotations in the matrix
template <class Real>
static Vector<3, Real> matrix_to_scale(const Matrix<4, Real>& mat, bool with_sign = false)
{
    Vector<3, Real> x(mat[0][0], mat[1][0], mat[2][0]);
    Vector<3, Real> y(mat[0][1], mat[1][1], mat[2][1]);
    Vector<3, Real> z(mat[0][2], mat[1][2], mat[2][2]);

    Vector<3, Real> scale(length(x), length(y), length(z));

    /// @todo Detect negative scaling by sign of determinant

    if(with_sign)
    {
        if(x[0] < 0)
            scale[0] = -scale[0];
        if(y[1] < 0)
            scale[1] = -scale[1];
        if(z[2] < 0)
            scale[2] = -scale[2];
    }

    return scale;
}

template <class Real>
static Vector<3, Real> matrix_to_pos(const Matrix<4, Real>& mat)
{
    Vector<3, Real> pos(mat[0][3], mat[1][3], mat[2][3]);

    return pos;
}

// Returns true if the matrix is reversed (flipping objects) due to negative scaling.
template <class Real>
bool is_reversed_matrix(const Matrix<4, Real>& m)
{
    return (m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] - m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1]) < 0.0;
}

template<class Real>
Matrix<4, Real> compose_matrix_hpb(const Vector<3, Real>& t, const Vector<3, Real>& r, const Vector<3, Real>& s)
{
    return position_matrix(t) 
         * rotation_matrix_hpb(r) 
         * scale_matrix(s);
}

template <class Real>
Vector<3,Real> transform_point(const Matrix<4,Real>& m, const Vector<3,Real>& point)
{
    return cart(m * hom(point) );
}

template <class Real>
Vector<3,Real> transform_point(const Matrix<4,Real>& m, Real x, Real y, Real z)
{
    return transform_point(m, Vector<3,Real>(x, y, z) );
}

template <class Real>
Vector<3,Real> transform_vector(const Matrix<4,Real>& m, const Vector<3,Real>& vec)
{
    return Vector<3, Real>(m * Vector<4, Real>(vec) );
}

template <class Real>
Vector<3,Real> transform_vector(const Matrix<4,Real>& m, Real x, Real y, Real z)
{
    return transform_vector(m, Vector<3, Real>(x, y, z) );
}

template <class Real>
Vector<3,Real> rotate_vector(const Matrix<4,Real>& m, const Vector<3,Real>& vec)
{
    Vector<3, Real> hpb = matrix_to_hpb(m);
    return transform_vector(rotation_matrix_hpb(hpb), vec);
}

// Transforms a rotation XYZ in some given order
// into a rotation XYZ' in some other order, such that
//   Rot(XYZ') = LeftM * Rot(XYZ) * RightM
// with XYZ' preserving the amount of spinning done by HPB.
template <class Real>
Vector<3,Real> transform_xyz_rotation(const Matrix<4,Real>& left_m, const Vector<3,Real>& xyz, RotationOrder in_order, const Matrix<4,Real>& right_m, RotationOrder out_order)
{
    const Matrix<4,Real> inv_left_m = inverse(left_m);

    const int steps = 16;

    Matrix<4,Real> trans_xyz_matrix(left_m * right_m);
    Vector<3,Real> trans_xyz(matrix_to_xyz_rotation(trans_xyz_matrix, out_order));

    // Note that LeftM X Y Z RightM == (LeftM X LeftM^-1) (LeftM Y LeftM^-1) (LeftM Z LeftM^-1) LeftM RightM
    for(int i = 2; i >= 0; i--)
    {
        const int axis = RotationPermutation[in_order][i];

        Vector<3,Real> trans_steps_xyz[steps+1] = { trans_xyz };

        Vector<3,Real> match_xyz(trans_steps_xyz[0]);

        const Real axis_angle = xyz[axis];

        const Real step_angle = 2*PI/steps * ((axis_angle < 0) ? -1 : (axis_angle == 0) ? 0 : 1);

        if(step_angle != 0)
        {
            // Apply rotation in steps, using the previous step as a stepping stone
            // to prevent wraparound of the angle values
            int full_steps = (int)floor(axis_angle / step_angle + 1/(Real)2);
            int full_spins = full_steps / steps;
            int part_steps = full_steps % steps;
            int max_steps = (full_spins != 0) ? steps : part_steps;

            for(int s = 1; s <= max_steps; s++)
            {
                Vector<3,Real> rot(0, 0, 0);
                rot[axis] = s * step_angle;
                trans_steps_xyz[s] = matrix_to_xyz_rotation(left_m * rotation_matrix_xyz(rot, in_order) * inv_left_m * trans_xyz_matrix, out_order);
                find_best_xyz_rotation(trans_steps_xyz[s], trans_steps_xyz[s-1], out_order);
            }

            // The angles near the desired rotation
            match_xyz = trans_steps_xyz[part_steps];
            if(full_spins != 0)
                match_xyz += (trans_steps_xyz[steps] - trans_steps_xyz[0]) * (Real)full_spins;
        }

        // The actual angles of the desired rotation, using the approximation from the
        // stepping stones to prevent wraparound of the angle values
        Vector<3,Real> rot(0, 0, 0);
        rot[axis] = axis_angle;
        trans_xyz_matrix = left_m * rotation_matrix_xyz(rot, in_order) * inv_left_m * trans_xyz_matrix;
        trans_xyz = matrix_to_xyz_rotation(trans_xyz_matrix, out_order);
        find_best_xyz_rotation(trans_xyz, match_xyz, out_order);
    }

    return trans_xyz;
}

// Transforms a rotation XYZ by M such that
//   Rot(XYZ') = M * Rot(XYZ) * M^-1
// with XYZ' preserving the amount of spinning done by HPB.
// This gives a rotation in coordinate system M^-1 which is equivalent to rotating by XYZ in world coordinates.
template <class Real>
Vector<3,Real> transform_xyz_rotation(const Matrix<4,Real>& m, const Vector<3,Real>& xyz, RotationOrder order)
{
    return transform_xyz_rotation(m, xyz, order, inverse(m), order);
}

// Transforms a rotation HPB such that
//   Rot(HPB') = LeftM * Rot(HPB) * RightM
//   [or:  Rot(HPB') * RightM^-1 = LeftM * Rot(HPB) ]
// with HPB' preserving the amount of spinning done by HPB.
template <class Real>
Vector<3,Real> transform_hpb_rotation(const Matrix<4,Real>& left_m, const Vector<3,Real>& hpb, const Matrix<4,Real>& right_m)
{
    Vector<3,Real> xyz_out = transform_xyz_rotation(left_m, Vector<3,Real>(hpb[1], hpb[0], hpb[2]), eRotationOrderHPB, right_m, eRotationOrderHPB);
    return Vector<3,Real>(xyz_out[1], xyz_out[0], xyz_out[2]);
}

// Transforms a rotation HPB by M such that
//   Rot(HPB') = M * Rot(HPB) * M^-1
//   [or:  Rot(HPB') * M = M * Rot(HPB) ]
// with HPB' preserving the amount of spinning done by HPB.
// This gives a rotation in coordinate system M^-1 which is equivalent to rotating by HPB in world coordinates.
template <class Real>
Vector<3,Real> transform_hpb_rotation(const Matrix<4,Real>& m, const Vector<3,Real>& hpb)
{
    return transform_hpb_rotation(m, hpb, inverse(m));
}


// Finds a rotation Rot(XYZ') such that:
//   Rot(XYZ') = Rot(XYZ1) Rot(XYZ2)
// which preserves the amount of spin of XYZ1 and XYZ2
template <class Real>
Vector<3,Real> add_xyz_rotations(const Vector<3,Real>& xyz1, const Vector<3,Real>& xyz2, RotationOrder order)
{
    Vector<3,Real> trans_xyz(xyz2);

    const int steps = 16;

    Matrix<4,Real> trans_xyz_matrix(rotation_matrix_xyz(trans_xyz, order));

    // First apply B, then P, then H
    for(int i = 2; i >= 0; i--)
    {
        const int axis = RotationPermutation[order][i];

        Vector<3,Real> trans_steps_xyz[steps+1] = { trans_xyz };

        Vector<3,Real> match_xyz(trans_steps_xyz[0]);

        const Real axis_angle = xyz1[axis];

        const Real step_angle = 2*PI/steps * ((axis_angle < 0) ? -1 : (axis_angle == 0) ? 0 : 1);

        if(step_angle != 0)
        {
            // Apply rotation in steps, using the previous step as a stepping stone
            // to prevent wraparound of the angle values
            int full_steps = (int)floor(axis_angle / step_angle + 1/(Real)2);
            int full_spins = full_steps / steps;
            int part_steps = full_steps % steps;
            int max_steps = (full_spins != 0) ? steps : part_steps;

            for(int s = 1; s <= max_steps; s++)
            {
                Vector<3,Real> rot(0, 0, 0);
                rot[axis] = s * step_angle;
                trans_steps_xyz[s] = matrix_to_xyz_rotation(rotation_matrix_xyz(rot, order) * trans_xyz_matrix, order);
                find_best_xyz_rotation(trans_steps_xyz[s], trans_steps_xyz[s-1], order);
            }

            // The angles near the desired rotation
            match_xyz = trans_steps_xyz[part_steps];
            if(full_spins != 0)
                match_xyz += (trans_steps_xyz[steps] - trans_steps_xyz[0]) * (Real)full_spins;
        }

        // The actual angles of the desired rotation, using the approximation from the
        // stepping stones to prevent wraparound of the angle values
        Vector<3,Real> rot(0, 0, 0);
        rot[axis] = axis_angle;
        trans_xyz_matrix = rotation_matrix_xyz(rot, order) * trans_xyz_matrix;
        trans_xyz = matrix_to_xyz_rotation(trans_xyz_matrix, order);
        find_best_xyz_rotation(trans_xyz, match_xyz, order);
    }

    return trans_xyz;
}

// Finds a rotation Rot(HPB') such that:
//   Rot(HPB') = Rot(HPB1) Rot(HPB2)
// which preserves the amount of spin of HPB1 and HPB2
template <class Real>
Vector<3,Real> add_hpb_rotations(const Vector<3,Real>& hpb1, const Vector<3,Real>& hpb2)
{
    Vector<3,Real> xyz_out = add_xyz_rotation(Vector<3,Real>(hpb1[1], hpb1[0], hpb1[2]), Vector<3,Real>(hpb2[1], hpb2[0], hpb2[2]), eRotationOrderHPB);
    return Vector<3,Real>(xyz_out[1], xyz_out[0], xyz_out[2]);
}

template <class Real>
void transform_bounds(const Matrix<4,Real>& transform,
                      const Vector<3,Real>& l_bmin,
                      const Vector<3,Real>& l_bmax,
                      Vector<3,Real>& out_bmin,
                      Vector<3,Real>& out_bmax)
{
    Real xmin = l_bmin[0], xmax = l_bmax[0];
    Real ymin = l_bmin[1], ymax = l_bmax[1];
    Real zmin = l_bmin[2], zmax = l_bmax[2];

    const Vector<3,Real> corners[8] =
    {
        cart(transform * Vector<4,Real>(xmin, ymin, zmin, 1.0)),
        cart(transform * Vector<4,Real>(xmin, ymin, zmax, 1.0)),
        cart(transform * Vector<4,Real>(xmin, ymax, zmin, 1.0)),
        cart(transform * Vector<4,Real>(xmin, ymax, zmax, 1.0)),
        cart(transform * Vector<4,Real>(xmax, ymin, zmin, 1.0)),
        cart(transform * Vector<4,Real>(xmax, ymin, zmax, 1.0)),
        cart(transform * Vector<4,Real>(xmax, ymax, zmin, 1.0)),
        cart(transform * Vector<4,Real>(xmax, ymax, zmax, 1.0))
    };

    out_bmin = out_bmax = corners[0];
    for (int j = 1; j < 8; ++j)
    {
        const Vector<3,Real>& c = corners[j];
        out_bmin = vmin(out_bmin, c);
        out_bmax = vmax(out_bmax, c);
    }
}

#endif