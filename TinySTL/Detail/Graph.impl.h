#include "../Graph.h"

namespace TinySTL{
	namespace Detail{
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::node&
			graph<Index, Value, EqualFunc>::new_node(const Index& index, const Value& val){
			auto ptr = nodeAllocator::allocate();
			nodeAllocator::construct(ptr, node(index, val));
			return *ptr;
		}
		template<class Index, class Value, class EqualFunc>
		void graph<Index, Value, EqualFunc>::del_node(node *p){
			nodeAllocator::destroy(p);
			nodeAllocator::deallocate(p);
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::node& 
			graph<Index, Value, EqualFunc>::get_node(const Index& index){
			for (auto& pair : nodes_){
				if (equal_func(pair.first.first, index))
					return pair.first;
			}
			return node();
		}
		template<class Index, class Value, class EqualFunc>
		void graph<Index, Value, EqualFunc>::cleanup(){
			for (auto out = nodes_.begin(); out != nodes_.end(); ++out){
				for (auto in = (out->second).begin(); in != (out->second).end(); ++in){
					del_node(&(*in));
				}
			}
		}
		template<class Index, class Value, class EqualFunc>
		bool graph<Index, Value, EqualFunc>::is_contained(const Index& index){
			for (auto& pair : nodes_){
				if (equal_func(pair.first.first, index))
					return true;
			}
			return false;
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::node_sets
			graph<Index, Value, EqualFunc>::empty_node_set(){
			return node_sets();
		}
		template<class Index, class Value, class EqualFunc>
		bool graph<Index, Value, EqualFunc>::empty()const{
			return nodes_.empty();
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::inner_iterator
			graph<Index, Value, EqualFunc>::begin(const Index& index){
			for (auto& pair : nodes_){
				if (equal_func(pair.first.first, index))
					return inner_iterator(this, (pair.second).begin());
			}
			return end(index);
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::inner_iterator
			graph<Index, Value, EqualFunc>::end(const Index& index){
			for (auto& pair : nodes_){
				if (equal_func(pair.first.first, index))
					return inner_iterator(this, (pair.second).end());
			}
			//throw std::exception("return end error");
			return inner_iterator();
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::iterator graph<Index, Value, EqualFunc>::begin(){
			return iterator(this, nodes_.begin());
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::iterator graph<Index, Value, EqualFunc>::end(){
			return iterator(this, nodes_.end());
		}
		template<class Index, class Value, class EqualFunc>
		size_t graph<Index, Value, EqualFunc>::size()const{
			return size_;
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::node_sets 
			graph<Index, Value, EqualFunc>::adjacent_nodes(const Index& index){
			node_sets s;
			for (auto it = begin(index); it != end(index); ++it){
				s.push_back(*it);
			}
			return s;
		}
		template<class Index, class Value, class EqualFunc>
		typename graph<Index, Value, EqualFunc>::node_sets
			graph<Index, Value, EqualFunc>::adjacent_nodes(const node& n){
			return adjacent_nodes(n.first);
		}
		template<class Index, class Value, class EqualFunc>
		void graph<Index, Value, EqualFunc>::DFS(const Index& index, visiter_func_type func){
			node *start = &(get_node(index));
			Unordered_set<Index, std::hash<Index>, EqualFunc> visited(7);

			auto nodes = adjacent_nodes(start->first);
			func(*start);
			visited.insert(start->first);
			for (auto& n : nodes){
				if (visited.count(n.first) == 0)//has not visited
					DFS(n.first, func);
			}
		}
		//********************************************************************************
		template<class Index, class Value, class EqualFunc>
		inner_iterator<Index, Value, EqualFunc>& inner_iterator<Index, Value, EqualFunc>::operator ++(){
			++inner_it_;
			return *this;
		}
		template<class Index, class Value, class EqualFunc>
		const inner_iterator<Index, Value, EqualFunc> inner_iterator<Index, Value, EqualFunc>::operator ++(int){
			auto temp = *this;
			++*this;
			return temp;
		}
		template<class Index, class Value, class EqualFunc>
		bool operator ==(const inner_iterator<Index, Value, EqualFunc>& lhs,
			const inner_iterator<Index, Value, EqualFunc>& rhs){
			return lhs.container_ == rhs.container_ && lhs.inner_it_ == rhs.inner_it_;
		}
		template<class Index, class Value, class EqualFunc>
		bool operator !=(const inner_iterator<Index, Value, EqualFunc>& lhs,
			const inner_iterator<Index, Value, EqualFunc>& rhs){
			return !(lhs == rhs);
		}
		//*********************************************************************************
		template<class Index, class Value, class EqualFunc>
		outter_iterator<Index, Value, EqualFunc>& outter_iterator<Index, Value, EqualFunc>::operator ++(){
			++outter_it_;
			return *this;
		}
		template<class Index, class Value, class EqualFunc>
		const outter_iterator<Index, Value, EqualFunc> outter_iterator<Index, Value, EqualFunc>::operator ++(int){
			auto temp = *this;
			++*this;
			return temp;
		}
		template<class Index, class Value, class EqualFunc>
		bool operator ==(const outter_iterator<Index, Value, EqualFunc>& lhs,
			const outter_iterator<Index, Value, EqualFunc>& rhs){
			return lhs.container_ == rhs.container_ && lhs.outter_it_ == rhs.outter_it_;
		}
		template<class Index, class Value, class EqualFunc>
		bool operator !=(const outter_iterator<Index, Value, EqualFunc>& lhs,
			const outter_iterator<Index, Value, EqualFunc>& rhs){
			return !(lhs == rhs);
		}
	}//end of Detail

	template<class Index, class Value, class EqualFunc>
	directed_graph<Index, Value, EqualFunc>::directed_graph():graph(){}
	template<class Index, class Value, class EqualFunc>
	void directed_graph<Index, Value, EqualFunc>::add_node(const node& n, const node_sets& nodes){
		if (!is_contained(n.first)){
			nodes_.push_front(make_pair(n, list<typename graph::node>()));
			++size_;
		}
		if (nodes.empty())
			return;
		//find node n's list
		list<typename graph::node>* l;
		for (auto& pair : nodes_){
			if (equal_func(pair.first.first, n.first))
				l = &(pair.second);
		}
		for (const auto& item : nodes){
			l->push_front(item);
			if (!is_contained(item.first)){
				add_node(item, empty_node_set());
			}
		}
	}
}