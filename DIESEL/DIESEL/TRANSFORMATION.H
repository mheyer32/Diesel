/*
This file is part of Diesel
(c) 2002 by Mathias Heyer
email: sonode@gmx.de

Diesel is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Diesel is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#pragma once

#include <math/vector3.h>
#include <math/matrix4.h>

#define V_UP 1
#define V_RIGHT 0
#define V_FORWARD 2


class Transformation  
{
public:
	Transformation();
	virtual ~Transformation();

	inline bool hasChanged(int &old_counter) const
	{
		if (change_count!=old_counter)
		{
			old_counter=change_count;
			return true;
		}
		return false;
	}
	inline void Transformation::setTranslation(const VECTOR3 &t)
	{
		(VECTOR3&)matrix.a[3]=t;
		inverse_matrix_good=matrix_identity=false;
		++change_count;
	}

	inline void Transformation::getTranslation(VECTOR3 &t) const
	{
		t=(VECTOR3&)matrix.a[3];
	}

	inline const VECTOR3& Transformation::getTranslation() const
	{
		return (VECTOR3&)matrix.a[3];
	}

	inline bool Transformation::isIdentity() const
	{
		return matrix_identity;
	}
	inline void Transformation::setIdentity()
	{
		matrix=inverse_matrix=IdentityMatrix4;
		angles=NullVector3;
		scale=1.0f;
		angles_good=matrix_identity=inverse_matrix_good=normalized_axes=true;
		++change_count;
	}

	inline bool hasOrthonormalAxes() const {return normalized_axes;};

	inline void Transformation::setMatrix(const MATRIX4 &M, bool orthonormal_axes=false)
	{
		matrix=M;
		inverse_matrix_good=angles_good=matrix_identity=false;
		normalized_axes=orthonormal_axes;
		++change_count;
	}

	inline const MATRIX4& Transformation::getMatrix() const
	{
		return matrix;
	}

	inline const MATRIX4& Transformation::getInverseMatrix() 
	{
		if (!inverse_matrix_good) evaluateInverseMatrix();
		return inverse_matrix;
	}

	inline void Transformation::setAngles(const VECTOR3 &a)
	{
		angles=a;
		angles_good=true;
		inverse_matrix_good=matrix_identity=false;
		++change_count;
		evaluateMatrix();
	}

	inline const VECTOR3& Transformation::getAngles() const
	{
		//FIXME: check for angles_good, extract from matrix when needed
		return angles;
	}

	inline void Transformation::getAngles(VECTOR3 &anglevec) const
	{
		//FIXME: check for angles_good, extract from matrix when needed
		anglevec=angles;
	}

	inline void Transformation::setScale(float s)
	{
		scale=s;
		inverse_matrix_good=matrix_identity=normalized_axes=false;
		++change_count;
		evaluateMatrix();
	}

	inline float Transformation::getScale() const
	{
		return scale;
	}

	inline void Transformation::setRotation(const MATRIX3 &R)
	{
		//FIXME: scaling is overwritten here, assumes R is orthonormal!!!
		//need to extract angles from R
		matrix=R;
		matrix_identity=inverse_matrix_good=angles_good=false;
		normalized_axes=true;
		++change_count;
	}

	inline void Transformation::getRotation(MATRIX3 &R) const
	{
		R=matrix; // extract rotational part
	}

	inline const VECTOR3& Transformation::getColumnVector(const int c) const
	{
		return (VECTOR3&)matrix.a[c];
	}

	Transformation& operator = (const Transformation &transformation2);

protected:
	void evaluateInverseMatrix();
	void evaluateMatrix();

	MATRIX4	matrix;
	MATRIX4 inverse_matrix;
	VECTOR3	angles; // yaw,pitch,roll
	float scale;
	int change_count;

	bool inverse_matrix_good;
	bool angles_good;
	bool matrix_identity;
	bool normalized_axes;
};