#include "MS3DModel.h"

#include <math/mathlib.h>
#include <defs.h>

#include "VertexBuffer.h"

MS3DModel::MS3DModel(void)
{
	m_meshes=0;
	m_joints=0;
	m_jointmatrices=0;
}

MS3DModel::~MS3DModel(void)
{
	delete [] m_meshes;
	delete [] m_joints;
	delete [] m_jointmatrices;
}

CMesh*	MS3DModel::createMesh()
{
	CMesh *mesh=new CMesh;

	for (int m=0; m<m_num_meshes; ++m)
	{
		MESH &msmesh=m_meshes[m];
		// each MS3D-mesh translates into one vertexbuffer
		CVertexBuffer *vbuffer=new CVertexBuffer();
		vbuffer->AllocArrays(INDEXARRAY|VERTEXARRAY|NORMALARRAY|TEXCOORDARRAY,msmesh.num_vertices,msmesh.num_indices);
		
		// copy over all texcoords
		for (int v=0; v<msmesh.num_vertices; ++v)
		{
			vbuffer->texcoords[v]=msmesh.vertices[v].tex;
		}

		vbuffer->num_vertices=msmesh.num_vertices;

		// copy the indices
		vbuffer->setIndices(msmesh.indices,msmesh.num_indices);

		// set material
		if (msmesh.materialindex!=-1)
		{
			vbuffer->setShader(m_materials[msmesh.materialindex]);
		}
		else
		{
			vbuffer->setShader(CShader::DefaultShader);
		}

		mesh->addVertexBuffer(vbuffer);
	}

	evaluateReferencePosture(mesh);
	mesh->calcBoundingBox();
	mesh->setName(getName());

	return mesh;
}

void MS3DModel::getMatrix(MATRIX34 &mat, const VECTOR3 &position, const VECTOR3 &angles)
{
	getRotation(mat,angles);
	(VECTOR3&)mat.a[3]=position;
}

void MS3DModel::getRotation(MATRIX34 &mat, const VECTOR3 &angles)
{
	RotationMatrix34Z(mat,angles[2]);
	Rotate34Y(mat,angles[1]);
	Rotate34X(mat,angles[0]);
}

void MS3DModel::accumulateBoneMatrices()
{
	for (int j=1; j<m_num_joints; ++j)
	{
		if (m_joints[j].parent!=-1)
		{
			MatrixMultiply34(m_jointmatrices[m_joints[j].parent], m_jointmatrices[j], m_jointmatrices[j]);
		}
	}
}

void MS3DModel::verticesToBonespace()
{
	// transform the vertices, given in objectspace into the bone´s space they belong to
	for (int j=0; j<m_num_joints; ++j)
	{
		m_jointmatrices[j]=m_joints[j].local;
	}

	accumulateBoneMatrices();

	for (int j2=0; j2<m_num_joints; ++j2)
	{
		SimpleInverse34(m_jointmatrices[j2]);
	}

	for (int m=0; m<m_num_meshes; ++m)
	{
		MESH	&msmesh=m_meshes[m];
		for (int v=0; v<msmesh.num_vertices; ++v)
		{
			msmesh.vertices[v].pos=m_jointmatrices[msmesh.vertices[v].boneindex]*msmesh.vertices[v].pos;
			msmesh.vertices[v].norm=(MATRIX3&)m_jointmatrices[msmesh.vertices[v].boneindex]*msmesh.vertices[v].norm;
		}
	}
}

void MS3DModel::evaluateReferencePosture(CMesh *mesh)
{
	for (int j=0; j<m_num_joints; ++j)
	{
		m_jointmatrices[j]=m_joints[j].local;
	}

	accumulateBoneMatrices();

	fillVertexData(mesh);
}

void MS3DModel::fillVertexData(CMesh *mesh)
{
	BBOX bbox;

	for (int m=0; m<m_num_meshes; ++m)
	{
		CVertexBuffer *vbuffer=mesh->getVBufferList()[m];
		VECTOR3 *vertices=vbuffer->vertices[0];
		VECTOR3 *normals=vbuffer->normals[0];

		MESH	&msmesh=m_meshes[m];
		const VERTEX	*skinverts=msmesh.vertices;

		for (int v=0; v<msmesh.num_vertices; ++v)
		{
			const MATRIX34 mat=m_jointmatrices[skinverts[v].boneindex];
			bbox|=vertices[v]=mat*skinverts[v].pos;
			normals[v]=(MATRIX3&)mat*skinverts[v].norm;
		}
	}

	mesh->setBoundingBox(bbox); //FIXME: is there a more elegant way?
}

void MS3DModel::evaluateRuntimePosture(float frame, CMesh *mesh, MS3DAnimation *anim)
{
	if (!m_num_joints==anim->m_num_joints)
	{
		throw CException("MS3DModel::evaluateRuntimePosture() animation and model joint count do not match");
	}

	MATRIX34 temp;

	for (int j=0; j<m_num_joints; ++j)
	{
		const JOINT &joint=m_joints[j];
		const MS3DAnimation::JOINTKEYFRAMES &jointanim=anim->m_jointkeyframes[j];

		if (jointanim.num_rotkeys)
		{
			float jointframe=frame*(float)jointanim.num_rotkeys/(float)anim->m_num_frames;
			const int frame1=((int)jointframe)%jointanim.num_poskeys;
			const int frame2=(frame1+1)%jointanim.num_poskeys;
			const float frac=jointframe-floorf(jointframe); 

			QUATERNION slerp;

			slerpQuaternion(frac,jointanim.rotkeys[frame1].rotkey,jointanim.rotkeys[frame2].rotkey,slerp);

			getMatrix3(slerp, temp);
		}
		else
		{
			temp=IdentityMatrix34;
		}
		if (jointanim.num_poskeys)
		{
			float jointframe=frame*(float)jointanim.num_poskeys/(float)anim->m_num_frames;
			const int frame1=((int)jointframe)%jointanim.num_poskeys;
			const int frame2=(frame1+1)%jointanim.num_poskeys;
			const float frac=jointframe-floorf(jointframe);
			const float one_minus_frac=1.0f-frac;

			VECTOR3 position=jointanim.poskeys[frame1].poskey*one_minus_frac+jointanim.poskeys[frame2].poskey*frac;
			(VECTOR3&)temp.a[3]=position;
		}
		else
		{
			(VECTOR3&)temp.a[3]=NullVector3;
		}

		MatrixMultiply34(joint.local, temp, m_jointmatrices[j]);
	}

	accumulateBoneMatrices();

	fillVertexData(mesh);
}

void MS3DModel::evaluateRuntimePosture(CMesh *mesh, const FRAMEINFOS &animframes)
{
	int num_animframes=animframes.size();

	for (int j=0; j<m_num_joints; ++j)
	{
		const JOINT &joint=m_joints[j];
	
		QUATERNION	rotation;
		QUATERNION	slerp;
		VECTOR3		position=NullVector3;

		for (int af=0; af<num_animframes; ++af)
		{
			const FRAMEINFO &animframe=animframes[af];
			const MS3DAnimation::JOINTKEYFRAMES &jointkeyframes=animframe.anim->m_jointkeyframes[j];

			if (!m_num_joints==animframe.anim->m_num_joints)
			{
				throw CException("MS3DModel::evaluateRuntimePosture() animation and model joint count do not match");
			}

			if (jointkeyframes.num_rotkeys)
			{
				float jointframe=animframe.time-jointkeyframes.rot_starttime;
				if (jointframe<0) jointframe=0;
				const int frame1=((int)jointframe)%jointkeyframes.num_rotkeys;
				const int frame2=(frame1+1)%jointkeyframes.num_rotkeys;
				const float frac=jointframe-floorf(jointframe); 

				slerpQuaternion(frac,jointkeyframes.rotkeys[frame1].rotkey,jointkeyframes.rotkeys[frame2].rotkey,slerp);
				rotation=rotation*slerp;
			}

			if (jointkeyframes.num_poskeys)
			{
				float jointframe=animframe.time-jointkeyframes.pos_starttime;
				if (jointframe<0) jointframe=0;
				const int frame1=((int)jointframe)%jointkeyframes.num_poskeys;
				const int frame2=(frame1+1)%jointkeyframes.num_poskeys;
				const float frac=jointframe-floorf(jointframe);
		
				position+=jointkeyframes.poskeys[frame2].poskey*frac+jointkeyframes.poskeys[frame1].poskey*(1.0f-frac);
			}
		}

		MATRIX34 temp;
		getMatrix3(rotation,temp);
		(VECTOR3&)temp.a[3]=position;

		MatrixMultiply34(joint.local, temp, m_jointmatrices[j]);
	}

	accumulateBoneMatrices();
	fillVertexData(mesh);
}

int MS3DModel::getJointIndex(const char *name)
{
	for (int j=0; j<m_num_joints; ++j)
	{
		if (EQ(m_joints[j].name, name)) return j;
	}

	return -1; // joint not found
}
