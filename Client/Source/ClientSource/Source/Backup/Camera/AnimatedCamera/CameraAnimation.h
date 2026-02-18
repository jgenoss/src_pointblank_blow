#ifndef _CAMERA_ANIMATION_H_
#define _CAMERA_ANIMATION_H_

#ifdef	DOMI_ANIM_CAM

class CameraAnimation
{
public:
	CameraAnimation(const std::string file);
	virtual ~CameraAnimation();

	void		SetFile(const std::string file);
	const char* GetFile() const { return m_file.c_str(); }

	i3Node*		GetNode() const { return m_node;		 }

private:
	i3Node*		m_node;
	std::string m_file;
};

#endif

#endif