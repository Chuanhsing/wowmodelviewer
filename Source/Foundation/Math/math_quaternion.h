#ifndef MATH_QUATERNION_H
#define MATH_QUATERNION_H

#include "math_vector.h"
#include "math_matrix.h"

template <class Real>
class Quat : public Vector<4, Real>
{
public:
	Quat() : Vector<4, Real>(0,0,0,1)
	{}
	Quat(const Quat& rvec) : Vector<4, Real>(rvec)
	{}
	Quat(const Vector<4, Real>& rvec) : Vector<4, Real>(rvec)
	{}
	Quat(const Vector<3, Real>& ijk, Real s) : Vector<4, Real>(ijk[0],ijk[1],ijk[2], s)
	{}
	Quat(const Vector<3, Real>& p1, const Vector<3, Real>& p2) : Vector<4, Real>()
	{
		Vector<3,Real> axis(cross(p1, p2));
		normalize(axis);
		Real l = sqrt(dot(p1,p1) * dot(p2,p2));
		Real s = dot(p1, p2) / l;
		if(s < -1) s = -1;
		else if(s > 1) s = 1;
		Real ca = sqrt((1 + s) / 2);
		Real sa = sqrt((1 - s) / 2);
		axis *= sa;
		(*this)[0] = axis[0];
		(*this)[1] = axis[1];
		(*this)[2] = axis[2];
		(*this)[3] = ca;
	}
};

typedef Quat<float> Quatf;
typedef Quat<double> Quatd;

template <class Real>
Quat<Real> operator* (const Quat<Real>& lvec, const Quat<Real>& rvec)
{
	Quat<Real> prod;
	prod[0] = lvec[3] * rvec[0] + lvec[0] * rvec[3] + lvec[1] * rvec[2] - lvec[2] * rvec[1];
	prod[1] = lvec[3] * rvec[1] + lvec[1] * rvec[3] + lvec[2] * rvec[0] - lvec[0] * rvec[2];
	prod[2] = lvec[3] * rvec[2] + lvec[2] * rvec[3] + lvec[0] * rvec[1] - lvec[1] * rvec[0];
	prod[3] = lvec[3] * rvec[3] - lvec[0] * rvec[0] - lvec[1] * rvec[1] - lvec[2] * rvec[2];

	return prod;
}

template <class Real>
Vector<3,Real> operator* (const Quat<Real>& lvec, const Vector<3,Real>& rvec)
{
	Real t2 = lvec[3] * lvec[0];
	Real t3 = lvec[3] * lvec[1];
	Real t4 = lvec[3] * lvec[2];
	Real t5 = -lvec[0] * lvec[0];
	Real t6 = lvec[0] * lvec[1];
	Real t7 = lvec[0] * lvec[2];
	Real t8 = -lvec[1] * lvec[1];
	Real t9 = lvec[1] * lvec[2];
	Real t10 = -lvec[2] * lvec[2];
	return Vector<3,Real>(2*( (t8 + t10)*rvec[0] + (t6 -  t4)*rvec[1] + (t3 + t7)*rvec[2] ) + rvec[0],
							2*( (t4 +  t6)*rvec[0] + (t5 + t10)*rvec[1] + (t9 - t2)*rvec[2] ) + rvec[1],
							2*( (t7 -  t3)*rvec[0] + (t2 +  t9)*rvec[1] + (t5 + t8)*rvec[2] ) + rvec[2]);
}

template <class Real>
Quat<Real> conjugate (const Quat<Real>& quat)
{
	Quat<Real> conj;
	conj[0] = -quat[0];
	conj[1] = -quat[1];
	conj[2] = -quat[2];
	conj[3] =  quat[3];

	return conj;
}

template <class Real>
Quat<Real> inverse (const Quat<Real>& quat)
{
	Quat<Real> conj = conjugate(quat);
	Real d = dot(quat, quat);
	return conj / d;
}

template <class Real>
Quat<Real> operator/ (const Quat<Real>& lvec, const Quat<Real>& rvec)
{
	Quat<Real> rinv = inverse(rvec);
	return lvec * rinv;
}

template <class Real>
Quat<Real> exp (const Quat<Real>& quat)
{
	Vector<3, Real> qv(quat);
	Real m = length(qv);
	Real eq = exp(quat[3]);
	Real eqsmm = eq * sin(m) / m;
	Quat<Real> expquat(qv * eqsmm, eq * cos(m));
	return expquat;
}

template <class Real>
Quat<Real> axis_angle_to_quat(const Vector<3, Real>& axis, Real angle)
{
	Quat<Real> q(axis, 0);
	normalize(q);
	Real ca = cos(angle / 2);
	Real sa = sin(angle / 2);
	q *= sa;
	q[3] = ca;

	return q;
}

template <class Real>
Quat<Real> hpb_to_quat(const Vector<3, Real>& hpb)
{
	Quat<Real> hq(axis_angle_to_quat(Vector<3,Real>(0,1,0), hpb[0]));
	Quat<Real> pq(axis_angle_to_quat(Vector<3,Real>(1,0,0), hpb[1]));
	Quat<Real> bq(axis_angle_to_quat(Vector<3,Real>(0,0,1), hpb[2]));

	return hq * pq * bq;
}

template <class Real>
Vector<3, Real> quat_to_hpb(const Quat<Real>& q)
{
	Vector<3, Real> hpb;
	hpb[0] = atan2(2 * (q[0]*q[2] + q[1]*q[3]), 1 - 2 * (q[0]*q[0] + q[1]*q[1]));
	Real sp = 2*(q[0]*q[3] - q[1]*q[2]);
	if(sp < -1) sp = -1;
	else if(sp > 1) sp = 1;
	hpb[1] = asin(sp);
	hpb[2] = atan2(2 * (q[0]*q[1] + q[2]*q[3]), 1 - 2 * (q[0]*q[0] + q[2]*q[2]));

	return hpb;
}

template <class Real>
Real quat_to_axis_angle(const Quat<Real>& quat, Vector<3,Real>& axis)
{
	Real angle = acos(quat[3]) * 2;
	Real sa = sqrt(1 - quat[3]*quat[3]);
	if(fabs(sa) < std::numeric_limits<Real>::epsilon())
		sa = 1;
	axis = quat;
	axis /= sa;
	return angle;
}

template <class Real>
Quat<Real> slerp(const Quat<Real>& q1, const Quat<Real>& q2, Real t)
{
	Real c = dot(q1, q2);
	Quat<Real> to(q2);
	if(c < 0)
	{
		c = -c;
		to = -to;
	}
	Real s0 = 1 - t;
	Real s1 = t;
	if((1 - c) > std::numeric_limits<Real>::epsilon())
	{
		Real om = acos(c);
		Real som = sin(om);
		s0 = sin((1 - t) * om) / som;
		s1 = sin(t * om) / som;
	}

	return q1 * s0 + to * s1;
}

template <class Real>
Matrix<4, Real> quat_to_matrix(const Quat<Real>& q)
{
	Vector<3, Real> vx(1 - 2 * (q[1]*q[1] + q[2]*q[2]),
							2 * (q[0]*q[1] + q[2]*q[3]),
							2 * (q[0]*q[2] - q[1]*q[3]));
	Vector<3, Real> vy(	2 * (q[0]*q[1] - q[2]*q[3]),
						1 - 2 * (q[0]*q[0] + q[2]*q[2]),
							2 * (q[1]*q[2] + q[0]*q[3]));
	Vector<3, Real> vz(	2 * (q[0]*q[2] + q[1]*q[3]),
							2 * (q[1]*q[2] - q[0]*q[3]),
						1 - 2 * (q[0]*q[0] + q[1]*q[1]));
	return coordsys_matrix(vx, vy, vz);
}

template <class Real>
Quat<Real> matrix_to_quat(const Matrix<4, Real>& mat)
{
	// Somewhat roundabout way of doing it, but it uses code that has been reasonably well tested
	// Do a matrix_to_hpb, then convert the Euler rotation to quaternion

	const Vector<3, Real> hpb = matrix_to_hpb(mat);
	return hpb_to_quat(hpb);
}

#endif