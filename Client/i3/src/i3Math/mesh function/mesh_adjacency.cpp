#include "i3MathType.h"

#include "i3Math/mesh_function/mesh_adjacency.h"

#include "i3Base/smart_ptr/scoped_array.h"
#include "i3Math/structure/vec3.h"
#include "i3Math/structure/vec3_function.h"

//
// DirecXMesh 라이브러리 소스코드를 참조하고 i3Engine쪽 타입으로 적당이 수정한다.
//

namespace i3
{
	namespace
	{
		struct vertexHashEntry
		{
			vec3				v;
			i3::uint32_t			index;
			vertexHashEntry *   next;
		};

		struct edgeHashEntry
		{
			i3::uint32_t      v1;
			i3::uint32_t      v2;
			i3::uint32_t      vOther;
			i3::uint32_t      face;
			edgeHashEntry*	next;
		};
		
		void MakeXHeap(i3::uint32_t* out_index_arr, const vec3* vert_arr, size_t num_vert)
		{
			// 첫 초기화로 자기자신의 인덱스를 밀어넣음...
			for (i3::uint32_t vert = 0; vert < num_vert; ++vert)
			{
				out_index_arr[vert] = vert;
			}

			if (num_vert <= 1)
				return;
						
			// Create the heap
			i3::uint32_t iulLim = i3::uint32_t(num_vert);

			for (i3::uint32_t vert = i3::uint32_t(num_vert >> 1); --vert != -1; )
			{
				// Percolate down
				i3::uint32_t iulI = vert;
				i3::uint32_t iulJ = vert + vert + 1;
				i3::uint32_t ulT = out_index_arr[iulI];

				while (iulJ < iulLim)
				{
					i3::uint32_t ulJ = out_index_arr[iulJ];

					if (iulJ + 1 < iulLim)
					{
						i3::uint32_t ulJ1 = out_index_arr[iulJ + 1];
						if (vert_arr[ulJ1].x <= vert_arr[ulJ].x)
						{
							iulJ++;
							ulJ = ulJ1;
						}
					}

					if (vert_arr[ulJ].x > vert_arr[ulT].x)
						break;

					out_index_arr[iulI] = out_index_arr[iulJ];
					iulI = iulJ;
					iulJ += iulJ + 1;
				}

				out_index_arr[iulI] = ulT;
			}

			// Sort the heap
			while (--iulLim != -1)
			{
				i3::uint32_t ulT = out_index_arr[iulLim];
				out_index_arr[iulLim] = out_index_arr[0];

				// Percolate down
				i3::uint32_t iulI = 0;
				i3::uint32_t iulJ = 1;

				while (iulJ < iulLim)
				{
					i3::uint32_t ulJ = out_index_arr[iulJ];

					if (iulJ + 1 < iulLim)
					{
						i3::uint32_t ulJ1 = out_index_arr[iulJ + 1];
						if (vert_arr[ulJ1].x <= vert_arr[ulJ].x)
						{
							iulJ++;
							ulJ = ulJ1;
						}
					}

					if (vert_arr[ulJ].x > vert_arr[ulT].x)
						break;

					out_index_arr[iulI] = out_index_arr[iulJ];
					iulI = iulJ;
					iulJ += iulJ + 1;
				}
#ifdef _DEBUG
				assert(iulI < num_vert);
#else
				__assume(iulI < num_vert);
#endif
				out_index_arr[iulI] = ulT;
			}
			
		}

		//
		// 각 인덱스버퍼의 0-1, 1-2, 2-0에 해당하는 삼각형의 인접 삼각형 인덱스(인덱스버퍼의 나누기 3)을 얻는 함수이다...
		// 
		// 여기서 pointRep은 인접삼각형정보와 딱 직결되는 입력정보는 아니며, 정점배열에서 동일한 위치가 중복된 버텍스들 가운데 대표 버텍스인덱스를 알려주는 정보일 뿐임..
		//

		template<class index_t>
		bool convert_pointreps_to_adjacency_imp(const index_t* index_arr, size_t num_face, 
			const vec3* vert_arr, size_t num_vert, const i3::uint32_t* pointRep, i3::uint32_t* out_adjacency)
		{
			size_t hashSize = num_vert / 3;

			i3::scoped_array<edgeHashEntry*> hashTable(new (std::nothrow) edgeHashEntry*[hashSize]);

			if (!hashTable)
				return false;

			memset(hashTable.get(), 0, sizeof(edgeHashEntry*) * hashSize);

			i3::scoped_array<edgeHashEntry> hashEntries(new (std::nothrow) edgeHashEntry[3 * num_face]);

			if (!hashEntries)
				return false;

			i3::uint32_t freeEntry = 0;

			// add face edges to hash table and validate indices
			for (size_t face = 0; face < num_face; ++face)
			{
				index_t i0 = index_arr[face * 3];
				index_t i1 = index_arr[face * 3 + 1];
				index_t i2 = index_arr[face * 3 + 2];

				if (i0 == index_t(-1) || i1 == index_t(-1) || i2 == index_t(-1))
					continue;

				if (i0 >= num_vert || i1 >= num_vert || i2 >= num_vert)
					continue;

				i3::uint32_t v1 = pointRep[i0];
				i3::uint32_t v2 = pointRep[i1];
				i3::uint32_t v3 = pointRep[i2];

				// filter out degenerate triangles
				if (v1 == v2 || v1 == v3 || v2 == v3)
					continue;

				for (uint32_t point = 0; point < 3; ++point)
				{
					i3::uint32_t va = pointRep[index_arr[face * 3 + point]];
					i3::uint32_t vb = pointRep[index_arr[face * 3 + ((point + 1) % 3)]];
					i3::uint32_t vOther = pointRep[index_arr[face * 3 + ((point + 2) % 3)]];

					i3::uint32_t hashKey = va % hashSize;
#ifdef _DEBUG
					assert(freeEntry < (3 * num_face));
#else
					__assume(freeEntry < (3 * num_face));
#endif
					edgeHashEntry* newEntry = &hashEntries[freeEntry];
					++freeEntry;

					newEntry->v1 = va;
					newEntry->v2 = vb;
					newEntry->vOther = vOther;
					newEntry->face = i3::uint32_t(face);
					newEntry->next = hashTable[hashKey];
					hashTable[hashKey] = newEntry;
				}
			}

			assert(freeEntry <= (3 * num_face));

			memset(out_adjacency, 0xff, sizeof(i3::uint32_t) * num_face * 3);

			for (size_t face = 0; face < num_face; ++face)
			{
				index_t i0 = index_arr[face * 3];
				index_t i1 = index_arr[face * 3 + 1];
				index_t i2 = index_arr[face * 3 + 2];

				// filter out unused triangles
				if (i0 == index_t(-1) || i1 == index_t(-1) || i2 == index_t(-1))
					continue;

				if (i0 >= num_vert || i1 >= num_vert || i2 >= num_vert)
					continue;
/*
#ifdef _DEBUG
				assert(i0 < num_vert);
				assert(i1 < num_vert);
				assert(i2 < num_vert);
#else
				__assume(i0 < num_vert);
				__assume(i1 < num_vert);
				__assume(i2 < num_vert);
#endif
*/
				i3::uint32_t v1 = pointRep[i0];
				i3::uint32_t v2 = pointRep[i1];
				i3::uint32_t v3 = pointRep[i2];

				// filter out degenerate triangles
				if (v1 == v2 || v1 == v3 || v2 == v3)
					continue;

				for (uint32_t point = 0; point < 3; ++point)
				{
					if (out_adjacency[face * 3 + point] != UNUSED32)
						continue;

					// see if edge already entered, if not then enter it
					uint32_t va = pointRep[index_arr[face * 3 + ((point + 1) % 3)]];
					uint32_t vb = pointRep[index_arr[face * 3 + point]];
					uint32_t vOther = pointRep[index_arr[face * 3 + ((point + 2) % 3)]];

					uint32_t hashKey = va % hashSize;

					edgeHashEntry* current = hashTable[hashKey];
					edgeHashEntry* prev = nullptr;

					uint32_t foundFace = UNUSED32;

					while (current != 0)
					{
						if ((current->v2 == vb) && (current->v1 == va))
						{
							foundFace = current->face;
							break;
						}

						prev = current;
						current = current->next;
					}

					edgeHashEntry* found = current;
					edgeHashEntry* foundPrev = prev;

					float bestDiff = -2.f;

					// Scan for additional matches
					if (current != 0)
					{
						prev = current;
						current = current->next;

						// find 'better' match
						while (current != 0)
						{
							if ((current->v2 == vb) && (current->v1 == va))
							{
								vec3 pB1 = vert_arr[vb];
								vec3 pB2 = vert_arr[va];
								vec3 pB3 = vert_arr[vOther];

								vec3 v12b = pB1;	v12b -= pB2;
								vec3 v13b = pB1;	v13b -= pB3;

								vec3 v12b_13b_cross = i3::vec3_cross(v12b, v13b);
								vec3 bnormal = i3::vec3_normalize(v12b_13b_cross);

								if (bestDiff == -2.f)
								{
									vec3 pA1 = vert_arr[found->v1];
									vec3 pA2 = vert_arr[found->v2];
									vec3 pA3 = vert_arr[found->vOther];

									vec3 v12a = pA1;	v12a -= pA2;
									vec3 v13a = pA1;	v13a -= pA3;
									vec3 v12a_13a_cross = i3::vec3_cross(v12a, v13a);
									vec3 anormal = i3::vec3_normalize(v12a_13a_cross);

									bestDiff = i3::vec3_dot(anormal, bnormal);
								}

								vec3 pA1 = vert_arr[current->v1];
								vec3 pA2 = vert_arr[current->v2];
								vec3 pA3 = vert_arr[current->vOther];

								vec3 v12a = pA1; v12a -= pA2;
								vec3 v13a = pA1; v13a -= pA3;

								vec3 v12a_13a_cross = i3::vec3_cross(v12a, v13a);
								vec3 anormal = i3::vec3_normalize(v12a_13a_cross);

								float diff = i3::vec3_dot(anormal, bnormal);

								// if face normals are closer, use new match
								if (diff > bestDiff)
								{
									found = current;
									foundPrev = prev;
									foundFace = current->face;
									bestDiff = diff;
								}
							}

							prev = current;
							current = current->next;
						}
					}

					if (foundFace != UNUSED32)
					{
						assert(found != 0);

						// remove found face from hash table
						if (foundPrev != 0)
						{
							foundPrev->next = found->next;
						}
						else
						{
							hashTable[hashKey] = found->next;
						}

						assert(out_adjacency[face * 3 + point] == UNUSED32);
						out_adjacency[face * 3 + point] = foundFace;

						// Check for other edge
						uint32_t hashKey2 = vb % hashSize;

						current = hashTable[hashKey2];
						prev = nullptr;

						while (current != 0)
						{
							if ((current->face == uint32_t(face)) && (current->v2 == va) && (current->v1 == vb))
							{
								// trim edge from hash table
								if (prev != 0)
								{
									prev->next = current->next;
								}
								else
								{
									hashTable[hashKey2] = current->next;
								}
								break;
							}

							prev = current;
							current = current->next;
						}

						// mark neighbor to point back
						bool linked = false;

						for (i3::uint32_t point2 = 0; point2 < point; ++point2)
						{
							if (foundFace == out_adjacency[face * 3 + point2])
							{
								linked = true;
								out_adjacency[face * 3 + point] = UNUSED32;
								break;
							}
						}

						if (!linked)
						{
							i3::uint32_t point2 = 0;
							for (; point2 < 3; ++point2)
							{
								index_t k = index_arr[foundFace * 3 + point2];
								if (k == index_t(-1))
									continue;
#ifdef _DEBUG
								assert(k < num_vert);
#else
								__assume(k < num_vert);
#endif
								if (pointRep[k] == va)
									break;
							}

							if (point2 < 3)
							{
#ifdef _DEBUG
								i3::uint32_t testPoint = index_arr[foundFace * 3 + ((point2 + 1) % 3)];
								testPoint = pointRep[testPoint];
								assert(testPoint == vb);
								assert(out_adjacency[foundFace * 3 + point2] == UNUSED32);
#endif

								// update neighbor to point back to this face match edge
								out_adjacency[foundFace * 3 + point2] = i3::uint32_t(face);
							}
						}
					}
				}
			}

			return true;
		}

		template<class index_t>
		bool generate_pointreps_imp(const index_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert,
			float epsilon, i3::uint32_t* out_rep)
		{
			i3::scoped_array<i3::uint32_t> temp(new (std::nothrow) i3::uint32_t[num_vert + num_face * 3]);

			if (!temp)
				return false;

			i3::uint32_t* vertexToCorner = temp.get();
			i3::uint32_t* vertexCornerList = temp.get() + num_vert;

			memset(vertexToCorner, 0xff, sizeof(i3::uint32_t) * num_vert);
			memset(vertexCornerList, 0xff, sizeof(i3::uint32_t) * num_face * 3);

			const size_t num_index = num_face * 3;

			for (size_t i = 0; i < num_index; ++i)
			{
				index_t k = index_arr[i];
				if (k == index_t(-1))		// 인덱스값이 -1이거나,
					continue;
				if (k >= num_vert)			// 엉뚱하게 큰값이면 일단 건너뛴다..
					continue;

				vertexCornerList[i] = vertexToCorner[k];	// 맨 첫값은 -1이 들어가게됨..담번에 같은 버텍스를 만나면 이전 인덱스를 넣게 됨..
				vertexToCorner[k] = uint32_t(i);			// 인덱스버퍼에서 얻은 버텍스인덱스에 인덱스값을 밀어넣어봄..
			}

			if (epsilon == 0.f)
			{
				const size_t hashSize = num_vert / 3;

				i3::scoped_array<vertexHashEntry*> hash_table(new (std::nothrow)
					vertexHashEntry*[hashSize]);

				if (!hash_table)
					return false;

				memset(hash_table.get(), 0, sizeof(vertexHashEntry*) * hashSize);

				i3::scoped_array<vertexHashEntry> hash_entries(new (std::nothrow)
					vertexHashEntry[hashSize]);

				if (!hash_entries)
					return false;

				i3::uint32_t freeEntry = 0;

				for (size_t vert = 0; vert < num_vert; ++vert)
				{
					i3::uint32_t hashKey = (*reinterpret_cast<const i3::uint32_t*>(&vert_arr[vert].x)
						+ *reinterpret_cast<const i3::uint32_t*>(&vert_arr[vert].y)
						+ *reinterpret_cast<const i3::uint32_t*>(&vert_arr[vert].z)) % hashSize;

					i3::uint32_t found = UNUSED32;

					for (vertexHashEntry* current = hash_table[hashKey];
					current != 0; current = current->next)
					{
						if (current->v.x == vert_arr[vert].x
							&& current->v.y == vert_arr[vert].y
							&& current->v.z == vert_arr[vert].z)		// 해시테이블검색시 float값상 동일한 정점 발견시..
						{
							uint32_t head = vertexToCorner[vert];

							bool ispresent = false;

							while (head != UNUSED32)
							{
								i3::uint32_t face = head / 3;
#ifdef _DEBUG
								assert(face < num_face);
#else
								__assume(face < num_face);
#endif
								assert((index_arr[face * 3] == vert) ||
									(index_arr[face * 3 + 1] == vert) ||
									(index_arr[face * 3 + 2] == vert));

								if ((index_arr[face * 3] == current->index) ||
									(index_arr[face * 3 + 1] == current->index) ||
									(index_arr[face * 3 + 2] == current->index))
								{
									ispresent = true;			// 이미 존재하는 버텍스인덱스
									break;
								}

								head = vertexCornerList[head];	// 담번 인덱스버퍼 인덱스로 계속...
							}

							if (!ispresent)
							{
								found = current->index;
								break;
							}
						}
					}

					if (found != UNUSED32)
					{
						out_rep[vert] = found;
					}
					else
					{
#ifdef _DEBUG
						assert(freeEntry < num_vert);
#else
						__assume(freeEntry < num_vert);
#endif

						vertexHashEntry* newEntry = &hash_entries[freeEntry];
						++freeEntry;

						newEntry->v = vert_arr[vert];
						newEntry->index = i3::uint32_t(vert);
						newEntry->next = hash_table[hashKey];
						hash_table[hashKey] = newEntry;

						out_rep[vert] = i3::uint32_t(vert);
					}
				}

				assert(freeEntry <= num_vert);
			}
			else
			{
				i3::scoped_array<i3::uint32_t> xorder(new (std::nothrow) i3::uint32_t[num_vert]);

				MakeXHeap(xorder.get(), vert_arr, num_vert);

				memset(out_rep, 0xff, sizeof(i3::uint32_t) * num_vert);

				float eps_sq = epsilon * epsilon;

				i3::uint32_t head = 0;
				i3::uint32_t tail = 0;

				while (tail < num_vert)
				{
					// move head until just out of epsilon
					while ((head < num_vert)
						&& ((vert_arr[tail].x - vert_arr[head].x) <= epsilon))
					{
						++head;
					}

					// check new tail against all points up to the head
					i3::uint32_t tailIndex = xorder[tail];
#ifdef _DEBUG
					assert(tailIndex < num_vert);
#else
					__assume(tailIndex < num_vert);
#endif
					if (out_rep[tailIndex] == UNUSED32)
					{
						out_rep[tailIndex] = tailIndex;

						vec3 outer = vert_arr[tailIndex];

						for (size_t current = tail + 1; current < head; ++current)
						{
							uint32_t curIndex = xorder[current];
#ifdef _DEBUG
							assert(curIndex < num_vert);
#else
							__assume(curIndex < num_vert);
#endif
							// if the point is already assigned, ignore it
							if (out_rep[curIndex] == UNUSED32)
							{
								vec3 inner = vert_arr[curIndex];
								vec3 sub = inner;	sub -= outer;

								float diff = i3::vec3_length_sq(sub);

								if (diff < eps_sq)
								{
									i3::uint32_t headvc = vertexToCorner[tailIndex];

									bool ispresent = false;

									while (headvc != UNUSED32)
									{
										i3::uint32_t face = headvc / 3;
#ifdef _DEBUG
										assert(face < num_face);
#else
										__assume(face < num_face);
#endif
										assert((index_arr[face * 3] == tailIndex) ||
											(index_arr[face * 3 + 1] == tailIndex) ||
											(index_arr[face * 3 + 2] == tailIndex));

										if ((index_arr[face * 3] == curIndex) ||
											(index_arr[face * 3 + 1] == curIndex) ||
											(index_arr[face * 3 + 2] == curIndex))
										{
											ispresent = true;
											break;
										}

										headvc = vertexCornerList[headvc];
									}

									if (!ispresent)
									{
										out_rep[curIndex] = tailIndex;
									}
								}		// diff < eps_sq
							}		// out_rep[curIndex] == UNUSED32
						}		// current < head loop
					}		// out_rep[tailIndex] == UNUSED32

					++tail;
				}	// tail < num_vert loop
			}  // epsilon != 0.f

			return true;
		}

		template<class index_t>
		void propagate_face_vertices_imp(const i3::uint32_t* adj_arr, const index_t* index_arr, index_t* inout_new_index_arr,
			size_t curr_face_idx, size_t num_face, i3::uint32_t* inout_pointreps)
		{
			i3::uint32_t curr_vert_base_idx = curr_face_idx * 3;

			for (size_t edge = 0; edge < 3; ++edge)
			{
				i3::uint32_t adj_idx = adj_arr[curr_vert_base_idx + edge];

				if (adj_idx == index_t(-1))		// 인접면이 없음..
					continue;

				if (adj_idx >= num_face)		// 이상한 값...인접면이 없는 것으로 일단 간주..
					continue;

				i3::uint32_t adj_vert_base_idx = adj_idx * 3;

				size_t opp_edge = 0;

				for ( ; opp_edge < 3; ++opp_edge)
				{
					i3::uint32_t opp_vert_idx = adj_vert_base_idx + opp_edge;

					if (adj_arr[opp_vert_idx] == curr_face_idx)
						break;		// opp_edge를 찾음..
				}

				if (opp_edge == 3)					// 이상한 값..
					continue;

				for (size_t i = 0; i < 2; ++i)		// 같은 위치의 두 정점의 인덱스값이 만약 다르다면 인덱스가 낮은 놈을 선택하고, reps에 반영..
				{
					i3::uint32_t from = curr_vert_base_idx + (edge + (1 - i)) % 3;
					i3::uint32_t to = adj_vert_base_idx + (opp_edge + i) % 3;

					if (inout_new_index_arr[to] > inout_new_index_arr[from])
					{
						inout_new_index_arr[to] = inout_new_index_arr[from];
						inout_pointreps[ index_arr[to] ] = inout_new_index_arr[from];
					}
				}

			}
		}


		template<class index_t>
		void convert_adjacency_to_pointreps_imp(const index_t* index_arr, size_t num_face, size_t num_vert, const i3::uint32_t* in_adj, i3::uint32_t* out_rep)
		{
			if (num_face == 0)
				return ;

			i3::scoped_array<index_t>	new_index_arr_ptr( new index_t[num_face * 3] );

			index_t* new_index_arr = new_index_arr_ptr.get();

			::memcpy(new_index_arr, index_arr, sizeof(index_t) * num_face * 3);		// 일단 그대로 복사...

			for (size_t i = 0; i < num_vert; ++i)
			{
				out_rep[i] = i;							// 일단 자기자신의 인덱스로 초기화..
			}

			for (size_t i = 0; i < num_face; ++i)
			{
				propagate_face_vertices_imp<index_t>(in_adj, index_arr, new_index_arr, i, num_face, out_rep);
			}

			// 반드시 역순으로 한번 더 해줘야함... ( 면 순회하면서 해당 면의 인접면쪽만 갱신하고, 자기 자신은 갱신하지 않으므로 )
			for (size_t i = 0; i < num_face; ++i)
			{
				size_t opp_i = (num_face - 1) - i;
				propagate_face_vertices_imp<index_t>(in_adj, index_arr, new_index_arr, opp_i, num_face, out_rep);
			}

		}
		

	}
	
	// 생성 헬퍼들...
	i3::uint32_t*		new_pointreps(size_t num_vert)
	{
		return new (std::nothrow) i3::uint32_t[num_vert];
	}

	void			delete_pointreps(i3::uint32_t* pointreps)
	{
		delete[] pointreps;
	}

	i3::uint32_t*		new_adjacency(size_t num_face)
	{
		return new (std::nothrow) i3::uint32_t[num_face * 3];		// 인덱스버퍼크기이므로 곱하기 3
	}

	void			delete_adjacency(i3::uint32_t* adj)
	{
		delete[] adj;
	}
	

	bool generate_adjacency_and_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon,
		i3::uint32_t* out_adj, i3::uint32_t* out_rep)
	{
		i3::scoped_array<i3::uint32_t> temp;
		if (out_rep == nullptr)
		{
			temp.reset(new_pointreps(num_vert));
			if (!temp)
				return false;

			out_rep = temp.get();
		}
		
		if (generate_pointreps_imp<i3::uint16_t>(index_arr, num_face, vert_arr, num_vert, epsilon, out_rep) == false)
			return false;
		if (out_adj == nullptr)
			return true;

		return convert_pointreps_to_adjacency_imp<i3::uint16_t>(index_arr, num_face, vert_arr, num_vert, out_rep, out_adj);
	}

	bool generate_adjacency_and_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon,
		i3::uint32_t* out_adj, i3::uint32_t* out_rep)
	{
		i3::scoped_array<i3::uint32_t> temp;
		if (out_rep == nullptr)
		{
			temp.reset(new_pointreps(num_vert));
			if (!temp)
				return false;
			out_rep = temp.get();
		}

		if (generate_pointreps_imp<i3::uint32_t>(index_arr, num_face, vert_arr, num_vert, epsilon, out_rep) == false)
			return false;
		if (out_adj == nullptr)
			return true;

		return convert_pointreps_to_adjacency_imp<i3::uint32_t>(index_arr, num_face, vert_arr, num_vert, out_rep, out_adj);
	}

	bool generate_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_rep)
	{
		return generate_adjacency_and_pointreps(index_arr, num_face, vert_arr, num_vert, epsilon, nullptr, out_rep);
	}

	bool generate_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_rep)
	{
		return generate_adjacency_and_pointreps(index_arr, num_face, vert_arr, num_vert, epsilon, nullptr, out_rep);
	}
	
	bool generate_adjacency_with_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, const i3::uint32_t* pointreps, i3::uint32_t* out_adj)
	{
		i3::scoped_array<i3::uint32_t> temp;

		if (pointreps == nullptr)
		{
			temp.reset(new_pointreps(num_vert));
			if (!temp)
				return false;

			for (size_t i = 0; i < num_vert; ++i)
			{
				temp[i] = i3::uint32_t(i);
			}

			pointreps = temp.get();
		}

		return convert_pointreps_to_adjacency_imp<i3::uint16_t>(index_arr, num_face, vert_arr, num_vert, pointreps, out_adj);
	}

	bool generate_adjacency_with_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, const i3::uint32_t* pointreps, i3::uint32_t* out_adj)
	{
		i3::scoped_array<i3::uint32_t> temp;

		if (pointreps == nullptr)
		{
			temp.reset(new_pointreps(num_vert));
			if (!temp)
				return false;

			for (size_t i = 0; i < num_vert; ++i)
			{
				temp[i] = i3::uint32_t(i);
			}

			pointreps = temp.get();
		}

		return convert_pointreps_to_adjacency_imp<i3::uint32_t>(index_arr, num_face, vert_arr, num_vert, pointreps, out_adj);
	}

	bool generate_adjacency_with_no_pointreps(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, i3::uint32_t* out_adj)
	{
		return generate_adjacency_with_pointreps(index_arr, num_face, vert_arr, num_vert, nullptr, out_adj);
	}

	bool generate_adjacency_with_no_pointreps(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, i3::uint32_t* out_adj)
	{
		return generate_adjacency_with_pointreps(index_arr, num_face, vert_arr, num_vert, nullptr, out_adj);
	}

	bool generate_adjacency(const i3::uint16_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_adj)
	{
		return generate_adjacency_and_pointreps(index_arr, num_face, vert_arr, num_vert, epsilon, out_adj, nullptr);
	}

	bool generate_adjacency(const i3::uint32_t* index_arr, size_t num_face, const vec3* vert_arr, size_t num_vert, float epsilon, i3::uint32_t* out_adj)
	{
		return generate_adjacency_and_pointreps(index_arr, num_face, vert_arr, num_vert, epsilon, out_adj, nullptr);
	}


	void extract_pointreps_from_adjacency(const i3::uint16_t* index_arr, size_t num_face, size_t num_vert, const i3::uint32_t* in_adj, i3::uint32_t* out_rep)
	{
		convert_adjacency_to_pointreps_imp<i3::uint16_t>(index_arr, num_face, num_vert, in_adj, out_rep);
	}

	void extract_pointreps_from_adjacency(const i3::uint32_t* index_arr, size_t num_face, size_t num_vert, const i3::uint32_t* in_adj, i3::uint32_t* out_rep)
	{
		convert_adjacency_to_pointreps_imp<i3::uint32_t>(index_arr, num_face, num_vert, in_adj, out_rep);
	}



}
