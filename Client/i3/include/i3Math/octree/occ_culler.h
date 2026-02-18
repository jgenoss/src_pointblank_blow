#pragma once


namespace i3
{
	class octree;

	class occ_mesh_set;
	class occ_quad_set;
	class occ_quad;
	struct occ_object;

	class octree_item;
	class frustum_plane;
	struct mat4;
	struct vec3;
	// 매니저 인스턴스로 만들까도 생각해봤지만...그냥 단일 객체로 생성가능하도록 함...
	/*
	class occ_statistics_callback
	{
	public:
		virtual ~occ_statistics_callback();
		virtual void on_start_occ_culling(    )
		virtual void on_occluder_occluded(const occ_quad* q) const;
		virtual void on_occluder_visible(const occ_quad* q) const;
	};
	*/

	struct occ_statistics_info
	{
		int		tot_occluder_count;
		int		tot_occludee_count;

		int		octree_visible_occluder_count;
		int		octree_visible_occludee_count;

		int		geom_occ_visible_occluder_count;
		int		geom_occ_visible_occludee_count;

		int		geom_occ_surely_visible_occludee_count;

		int		hw_query_calc_occluder_count;
		int		hw_query_visible_occludee_count;
	};

	class occ_statistics_callback
	{
	public:
		virtual void on_change(const occ_statistics_info& info) = 0;
		virtual ~occ_statistics_callback() {}
	};


	class I3_EXPORT_MATH occ_hw_query_callback
	{
	public:
		virtual int on_additional_process(	const i3::vector<occ_quad*>& occluder_list, 
											const i3::vector<octree_item*>& occludee_list, 
											const i3::mat4& mat_view_proj) = 0;
		virtual ~occ_hw_query_callback() {}
	};

	class I3_EXPORT_MATH occ_culler
	{
	public:
		occ_culler(octree* arg_octree);				// 일단은 옥트리 의존적으로 처리..
		~occ_culler();

//		void set_occ_statistics_callback(  )

		bool register_occ_quads(const occ_mesh_set* mesh_set);

		void update_occludee_item(octree_item* item);
		void add_occludee_item(octree_item* item);
		void remove_occludee_item(octree_item* item);


		void cull_frustum(const frustum_plane& fr, const mat4& matView, 
			const mat4& matProj, const vec3& cam_pos);			// 내부에서 옥트리 업데이트를 병행..
		const octree*	get_octree() const { return m_octree;  }		// 옥트리에 대한 수정 접근 차단 합니다.

		const occ_quad_set*			get_occ_quad_set(size_t idx) const { return m_vec_occ_quad_set[idx]; }
		size_t						get_num_occ_quad_set() const { return m_vec_occ_quad_set.size();  }
		void						set_hw_query_callback(occ_hw_query_callback* cb) { m_query_callback = cb;  }
		void						set_statistics_callback(occ_statistics_callback* cb) { m_statistics_callback = cb;  }

		
	private:
		void _add_octree_visible_quad(occ_quad* quad);
		void _add_octree_visible_item(octree_item* item);

		octree*	m_octree;

		i3::vector<occ_quad_set*>	m_vec_occ_quad_set;		// 웬만하면 1개.. 이건 그냥 소거용으로만 두고 딱히 더 조작하진 않음..
		i3::vector<occ_object*>		m_vec_occ_object;		// 실시간 사용..매번 클리어됨..(cull_frustum 호출 마지막 위치.)
		i3::vector<occ_object*>		m_vec_invisible_object;	//

		i3::vector<occ_object*>		m_vec_processed_object;

		i3::vector<occ_quad*>		m_vecRemainQuads;
		i3::vector<occ_object*>		m_vecRenderableObjects_GeomOCC;	// 실시간 사용 .. 매번 클리어됨..(cull_frusutm 호출 처음위치.)

		i3::vector<octree_item*>	m_vecUnsurelyRenderableItem_OnGeomOCC;

		occ_hw_query_callback*		m_query_callback;
		occ_statistics_callback*	m_statistics_callback;

		occ_statistics_info			m_statistics_info;		

		friend class occluder_item_static;
		friend class occludee_item_dynamic;
	};
	
}