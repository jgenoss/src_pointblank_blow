#include "i3CommonType.h"
#include "class_meta/class_meta.h"
#include "class_meta/class_meta_system.h"
#include "itl/container_util.h"

namespace i3
{

	class_meta::~class_meta() 
	{ 
		
	}

	void			class_meta::register_meta()
	{
		i3::class_meta_system::i()->register_meta(this);
	}

	void			class_meta::set_class_meta_style(class_meta_style t) 
	{   
		if ( t == m_cms) return;

		switch (t)
		{
		case cms_normal:
			{
				if ( has_concrete_creator() )		// concrete가 쓰인 경우..m_cms --> abstract인데...원래것으로 교체시도한다..
				{
					m_creation_meta->set_class_meta_style(t);
					return;
				}

				switch(m_cms)
				{
				case cms_abstract:
					{
						if (m_is_abstract_class == false)
							m_cms = cms_normal;
					}
					break;
				case cms_concrete:		// 이 경우엔 부모쪽을 normal로 변경시도..
					{
						class_meta_type* p = class_meta::find_concrete_parent_meta();
						
						if ( p != nullptr)
						{
							p->m_creation_meta = p;
							if ( p->m_is_abstract_class == false) p->m_cms = cms_normal;
							m_cms = cms_normal;
						}
					}
					break;

				case cms_ghost:
					{
						class_meta_type* p = class_meta::find_ghost_parent_meta();
						
						if ( p != nullptr)
						{
							p->m_ghost_meta = p;
							m_cms = cms_normal;												
						}
					}
					break;
				}
			}
			break;
		case cms_abstract:
			{
				m_cms = cms_abstract;
			}
			break;
		case cms_concrete:
			{
				class_meta::set_class_meta_style_concrete( m_parent );
			}
			break;

		case cms_ghost:
			{
				class_meta::set_class_meta_style_ghost( m_parent );
			}
			break;
		}

	}

	void	class_meta::set_class_meta_style_concrete(class_meta_type* parent_meta)
	{
		if ( m_is_abstract_class == true ) return;			// 불가능..
		if ( check_parents(parent_meta) == false) return;	// 불가능..
				
		parent_meta->m_creation_meta = this;		// 자식것을 어떤 부모로..
		parent_meta->m_cms = cms_abstract;					
		m_cms = cms_concrete;
	}

	void	class_meta::set_class_meta_style_ghost( class_meta_type* parent_meta)
	{
		if ( m_is_abstract_class == true ) return;			// 불가능..
		if ( check_parents(parent_meta) == false) return;	// 불가능..
		
		parent_meta->m_ghost_meta = this;		// 자식것을 어떤 부모의 고스트로 등록..
		m_cms = cms_ghost;
	}
	
	bool	class_meta::check_parents( class_meta_type* may_be_parent_meta) const
	{
		class_meta* p = m_parent;
		for ( ; p != nullptr; p = p->m_parent )
			if ( may_be_parent_meta == p ) return true;
		return false;
	}

	void	class_meta::get_all_derived_metas(i3::vector<class_meta_type*>& l, bool recursive)
	{
		if (!recursive) { l.insert(l.end(), m_childs.begin(), m_childs.end());  return; }

		const size_t num = m_childs.size();
		for (size_t i = 0; i < num ; ++i)
		{
			l.push_back(m_childs[i]);
			m_childs[i]->get_all_derived_metas(l, true);
		}
	}

	bool			class_meta::add_child(class_meta_type* child) 
	{
		if (child->m_parent) return false;		// 고정되기 때문에, 이런 경우가 있으면 안됨..
		m_childs.push_back(child);		child->m_parent = this;		return true;
	}

	
	const i3::fixed_string&		class_meta::persist_class_name() const // 파일 입출력 용도로 보인다..구체클래스 이름을 쓰지 않도록 조절..
	{
		if ( m_cms == cms_concrete )  
			return m_parent->persist_class_name();		// cms_concrete가 m_parent!=nullptr을 보증해야함..
		return m_class_name;
	}
	
	class_meta*		class_meta::find_concrete_parent_meta() const
	{
		class_meta* p = m_parent;
		for ( ; p != nullptr && p->m_creation_meta != this ; p = p->m_parent );
		return p;
	}

	class_meta*		class_meta::find_ghost_parent_meta() const
	{
		class_meta* p = m_parent;
		for ( ; p != nullptr && p->m_ghost_meta != this ; p = p->m_parent );
		return p;
	}

}
