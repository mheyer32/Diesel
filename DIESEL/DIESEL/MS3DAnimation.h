#pragma once
#include <adt/RefCount.h>
#include <adt/SmartPointer.h>

#include "QUATERNION.H"

class MS3DAnimation : public CRefCount
{
friend class MS3DLoader;
friend class MS3DModel;

public:

typedef SmartPointer<MS3DAnimation> SMARTPTR;
	
	MS3DAnimation(void);

	inline int getNumFrames() const {return m_num_frames;};

protected:
	virtual ~MS3DAnimation(void);

	struct POSKEY
	{
		VECTOR3	poskey;
	};

	struct ROTKEY
	{
		QUATERNION rotkey;
	};

	struct JOINTKEYFRAMES
	{
		int			num_poskeys;
		int			num_rotkeys;
		float		pos_starttime;
		float		rot_starttime;
		POSKEY		*poskeys;
		ROTKEY		*rotkeys;
		JOINTKEYFRAMES():poskeys(0),rotkeys(0) {};
		~JOINTKEYFRAMES()
		{
			delete [] poskeys;
			delete [] rotkeys;
		}
	};

	int				m_num_joints;
	int				m_num_frames;
	JOINTKEYFRAMES	*m_jointkeyframes;
};
