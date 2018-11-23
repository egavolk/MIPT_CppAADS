#include <iostream>
#include <vector>
#include <iterator>
#include <list>
#include <algorithm>
#include <queue>
#include <cmath>
#include <cstdio>

enum EdgesDirection {
	Incoming = 0,
	Outcoming = 1
};

template <typename T>
struct Edge {
	unsigned from, to;
	T flow, capacity;

	explicit Edge(unsigned _from, unsigned _to, T _capacity)
		: from(_from)
		, to(_to)
		, flow(0)
		, capacity(_capacity) {}

	Edge(const Edge& other)
		: from(other.from)
		, to(other.to)
		, flow(other.flow)
		, capacity(other.capacity) {}

	Edge<T> operator=(const Edge<T>& other) {
		from = other.from;
		to = other.to;
		flow = other.flow;
		capacity = other.capacity;
		return *this;
	}

	T GetResidualCapacity() const& {
		return capacity - flow;
	}
};

template < typename T >
class Network;

template < typename T >
class NetworkIterator {
public:
	
	typedef Edge<T> MyEdge;

	typedef MyEdge value_type;
	typedef size_t difference_type;
	typedef MyEdge* pointer;
	typedef MyEdge& reference;
	typedef std::forward_iterator_tag iterator_category;

	typedef NetworkIterator<T> MyIt;

	NetworkIterator()
		: network_()
		, edge_index_it_()
		, direction_() {}

	NetworkIterator(const MyIt& other)
		: network_(other.network_)
		, edge_index_it_(other.edge_index_it_)
		, direction_(other.direction_) {}

	NetworkIterator(Network<T>* network, std::list<unsigned>::iterator edge_index_it, EdgesDirection direction)
		: network_(network)
		, edge_index_it_(edge_index_it)
		, direction_(direction) {}

	MyIt& operator=(const MyIt& other)
	{
		network_ = other.network_;
		edge_index_it_ = other.edge_index_it_;
		direction_ = other.direction_;
		return *this;
	}

	const MyEdge& operator*() const &
	{
		return network_->GetEdge(*edge_index_it_);
	}

	const MyEdge* operator->() const &
	{
		return &(network_->GetEdge(*edge_index_it_));
	}

	MyIt& operator++()
	{
		++edge_index_it_;
		return (*this);
	}

	MyIt operator++(int)
	{
		MyIt tmp = *this;
		++(*this);
		return tmp;
	}

	Network<T>* GetNetwork() const & {
		return network_;
	}

	std::list<unsigned>::iterator GetEdgeIndexIt() const & {
		return edge_index_it_;
	}

	unsigned GetEdgeIndex() const & {
		return *edge_index_it_;
	}

	unsigned GetDirection() const & {
		return direction_;
	}

	unsigned GetNearVertex() const & {
		if (direction_)
			return network_->GetEdge(*edge_index_it_).from;
		return network_->GetEdge(*edge_index_it_).to;
	}

	unsigned GetFarVertex() const & {
		if (direction_)
			return network_->GetEdge(*edge_index_it_).to;
		return network_->GetEdge(*edge_index_it_).from;
	}

private:
	Network<T>* network_;
	std::list<unsigned>::iterator edge_index_it_;
	EdgesDirection direction_;
};

template<typename T>
bool operator==(const NetworkIterator<T>& first, const NetworkIterator<T>& second)
{
	return first.GetNetwork() == second.GetNetwork() && first.GetEdgeIndexIt() == second.GetEdgeIndexIt() && first.GetDirection() == second.GetDirection();
}

template<typename T>
bool operator!=(const NetworkIterator<T>& first, const NetworkIterator<T>& second)
{
	return !(first == second);
}

template < typename T >
class Network {
	std::vector<Edge<T>> all_edges_;             //back edge to all_edges_[i]  is all_edges_[i ^ 1]
	std::vector<std::vector<std::list<unsigned>>> edges_lists_;  //[0][v] - incoming, [1][v] - outcoming

	unsigned source_, sink_;
	unsigned n_, m_;

	void AddOneSideEdge_(unsigned from, unsigned to, T capacity) {
		edges_lists_[0][to].push_back(all_edges_.size());
		edges_lists_[1][from].push_back(all_edges_.size());
		all_edges_.push_back(Edge<T>(from, to, capacity));
	}

public:
	typedef NetworkIterator<T> iterator;

	explicit Network(unsigned source = 0, unsigned sink = 0)
		: source_(source)
		, sink_(sink)
		, m_(0) {
		n_ = std::max<T>(source, sink) + 1;
		edges_lists_.resize(2);
		edges_lists_[0].resize(n_);
		edges_lists_[1].resize(n_);
	};

	Network(const Network<T>& other)
		: all_edges_(other.all_edges_)
		, edges_lists_(other.edges_lists_)
		, source_(other.source_)
		, sink_(other.sink_)
		, n_(other.n_)
		, m_(other.m_) {}

	Network<T> operator=(const Network<T>& other) {
		all_edges_ = other.all_edges_;
		edges_lists_ = other.edges_lists_;
		source_ = other.source_;
		sink_ = other.sink_;
		n_ = other.n_;
		m_ = other.m_;
		return *this;
	}

	void AddEdge(unsigned from, unsigned to, T capacity, bool is_directed) {
		unsigned max_vertex_index = std::max(to, from) + 1;
		if (max_vertex_index >= n_) {
			edges_lists_[0].resize(max_vertex_index);
			edges_lists_[1].resize(max_vertex_index);
			n_ = max_vertex_index;
		}
		AddOneSideEdge_(from, to, capacity);
		AddOneSideEdge_(to, from, is_directed ? 0 : capacity);
		m_ += 2;
	}

	const Edge<T>& GetEdge(unsigned index) {
		return all_edges_[index];
	}

	unsigned GetVerticesNumber() const {
		return n_;
	}

	unsigned GetEdgesNumber() const {
		return m_;
	}

	unsigned GetSource() const {
		return source_;
	}

	unsigned GetSink() const {
		return sink_;
	}

	void ExcludeFromIteratorView(iterator& it) {     ///after remove iterator point to the next edge
		auto cur_it = it;
		++it;
		edges_lists_[cur_it.GetDirection()][cur_it.GetNearVertex()].erase(cur_it.GetEdgeIndexIt());
	}

	void AddAllEdgesToIteratorView() {
		for (int i = 0; i < n_; ++i) {
			edges_lists_[0][i].clear();
			edges_lists_[1][i].clear();
		}
		for (int i = 0; i < m_; ++i) {
			edges_lists_[0][all_edges_[i].to].push_back(i);
			edges_lists_[1][all_edges_[i].from].push_back(i);
		}
	}

	void AddFlow(iterator it, T flow) {
		all_edges_[it.GetEdgeIndex()].flow += flow;
		all_edges_[it.GetEdgeIndex() ^ 1].flow -= flow;
	}

	iterator begin(unsigned index, EdgesDirection direction) {
		return iterator(this, edges_lists_[direction][index].begin(), direction);
	}

	iterator end(unsigned index, EdgesDirection direction) {
		return iterator(this, edges_lists_[direction][index].end(), direction);
	}

	bool bfs(std::vector<int> &dist) {
		std::fill(dist.begin(), dist.end(), 0);
		dist[source_] = 1;
		std::queue<int> q;
		q.push(source_);
		while (!q.empty()) {
			unsigned v = q.front();
			q.pop();
			for (auto it = begin(v, Outcoming); it != end(v, Outcoming); ++it)
				if ((!dist[it->to]) && (it->capacity > it->flow)) {
					dist[it->to] = dist[v] + 1;
					q.push(it->to);
				}
		}
		return dist[sink_];
	}
};

template <typename T>
class BlockingFlowSearchAlgorithms {
public:
	BlockingFlowSearchAlgorithms() {}

	BlockingFlowSearchAlgorithms(const BlockingFlowSearchAlgorithms<T>& other) {}

	BlockingFlowSearchAlgorithms<T>& operator=(const BlockingFlowSearchAlgorithms<T>& other) {
		return *this;
	}

	virtual void RunBlockingFlow(Network<T>& network) = 0;
};

template <typename T>
class DinicBlockingFlowSearch :
	public BlockingFlowSearchAlgorithms<T> {
private:
	T dfs_(Network<T>& network, int v, T flow) const {
		if (!flow || v == network.GetSink())
			return flow;
		for (auto it = network.begin(v, Outcoming); it != network.end(v, Outcoming); network.ExcludeFromIteratorView(it)) {
			T add_flow;
			if (v == network.GetSource())
				add_flow = dfs_(network, it->to, it->GetResidualCapacity());
			else
				add_flow = dfs_(network, it->to, std::min(flow, it->GetResidualCapacity()));
			network.AddFlow(it, add_flow);
			if (add_flow)
				return add_flow;
		}
		return 0;
	}
public:
	void RunBlockingFlow(Network<T>& network) {
		T add_flow = 1;
		while (add_flow)
			add_flow = dfs_(network, network.GetSource(), 1);
	}
};

template <typename T>
class MalhotraKumarMaheshwariBlockingFlowSearch :
	public BlockingFlowSearchAlgorithms<T> {
private:
	T GetPotential_(Network<T>& network, const std::vector<T>& potential_in, const std::vector<T>& potential_out, unsigned i, EdgesDirection direction) const {
		if (i == network.GetSource() && direction == Outcoming || i == network.GetSink() && direction == Incoming)
			return potential_out[i];
		if (i == network.GetSource() && direction == Incoming || i == network.GetSink() && direction == Outcoming)
			return potential_in[i];
		return std::min<T>(potential_in[i], potential_out[i]);
	}

	void CountPotential_(Network<T>& network, std::vector<T>& potential_in, std::vector<T>& potential_out, std::vector<bool>& zero_potential) const {
		std::fill(potential_in.begin(), potential_in.end(), 0);
		std::fill(potential_out.begin(), potential_out.end(), 0);
		for (unsigned i = 0; i < network.GetVerticesNumber(); ++i)
			for (auto it = network.begin(i, Outcoming); it != network.end(i, Outcoming); ++it) {
				if (!zero_potential[i] && !zero_potential[it->to]) {
					potential_out[it->from] += it->GetResidualCapacity();
					potential_in[it->to] += it->GetResidualCapacity();
				}
			}
	}

	void CountZeroPotential_(Network<T>& network, std::vector<T>& potential_in, std::vector<T>& potential_out, std::vector<bool>& zero_potential) const {
		for (unsigned i = 0; i < potential_in.size(); ++i)
			if (GetPotential_(network, potential_in, potential_out, i, Outcoming) == 0)
				zero_potential[i] = true;
	}

	int FindMinPotentialVertex_(Network<T>& network, const std::vector<T>& potential_in, const std::vector<T>& potential_out) const {
		int min_index = -1;
		for (unsigned i = 0; i < potential_in.size(); ++i) {
			if (GetPotential_(network, potential_in, potential_out, i, Outcoming) > 0 &&
				(min_index == -1 || GetPotential_(network, potential_in, potential_out, min_index, Outcoming) > GetPotential_(network, potential_in, potential_out, i, Outcoming)))
				min_index = i;
		}
		return min_index;
	}

	void UpdateFlowAndPotential_(Network<T>& network, std::vector<T>& extra_flow, std::vector<T>& potential1, std::vector<T>& potential2,
		typename Network<T>::iterator it, T flow, unsigned from, unsigned to) const {
		network.AddFlow(it, flow);
		extra_flow[from] -= flow;
		extra_flow[to] += flow;
		potential1[to] -= flow;
		potential2[from] -= flow;
	}

	void PushFlow_(Network<T>& network, std::vector<T>& potential1, std::vector<T>& potential2,
		std::vector<bool>& zero_potential, unsigned min_potential_vertex, EdgesDirection direction, unsigned added_flow) const {
		std::vector<T> extra_flow(potential1.size());
		extra_flow[min_potential_vertex] = added_flow;
		std::queue<int> q;
		std::vector<bool> used(potential1.size());
		q.push(min_potential_vertex);
		used[min_potential_vertex] = true;
		while (!q.empty()) {
			unsigned v = q.front();
			q.pop();
			for (auto it = network.begin(v, direction); it != network.end(v, direction); network.ExcludeFromIteratorView(it)) {
				T residual_capacity = it->GetResidualCapacity();
				if (!extra_flow[v])
					break;
				if (zero_potential[it.GetFarVertex()])
					continue;
				if (residual_capacity) {
					UpdateFlowAndPotential_(network, extra_flow, potential1, potential2, it,
						residual_capacity <= extra_flow[v] ? residual_capacity : extra_flow[v], v, it.GetFarVertex());
					if (!used[it.GetFarVertex()]) {
						used[it.GetFarVertex()] = true;
						q.push(it.GetFarVertex());
					}
					if (!extra_flow[v])
						break;
				}
			}
		}
	}

	void DeleteEdgesToZeroPotentialVertices_(Network<T>& network, std::vector<T>& potential_in, std::vector<T>& potential_out, std::vector<bool>& zero_potential) const {
		std::queue<int> q;
		for (unsigned i = 0; i < potential_in.size(); ++i) {
			if (GetPotential_(network, potential_in, potential_out, i, Outcoming) == 0 && !zero_potential[i])
				q.push(i);
		}
		std::vector<bool> used(potential_in.size());
		while (!q.empty()) {
			unsigned v = q.front();
			q.pop();
			if (GetPotential_(network, potential_in, potential_out, v, Outcoming) == 0 && !zero_potential[v]) {
				zero_potential[v] = true;
				for (unsigned direction = 0; direction < 2; ++direction)
					for (auto it = network.begin(v, EdgesDirection(direction)); it != network.end(v, EdgesDirection(direction)); network.ExcludeFromIteratorView(it)) {
						if (zero_potential[it.GetFarVertex()])
							continue;
						T residual_capacity = it->GetResidualCapacity();
						T other_potential = (direction ? potential_in[it->to] : potential_out[it->from]) -= residual_capacity;

						if (!other_potential && !used[it.GetFarVertex()]) {
							used[it.GetFarVertex()] = true;
							q.push(it.GetFarVertex());
						}
					}
			}
		}
	}
public:
	void RunBlockingFlow(Network<T>& network) {
		std::vector<T> potential_in(network.GetVerticesNumber()), potential_out(network.GetVerticesNumber());
		std::vector<bool> zero_potential(network.GetVerticesNumber());
		CountPotential_(network, potential_in, potential_out, zero_potential);

		DeleteEdgesToZeroPotentialVertices_(network, potential_in, potential_out, zero_potential);

		int min_index;
		while ((min_index = FindMinPotentialVertex_(network, potential_in, potential_out)) != -1) {
			T added_flow = GetPotential_(network, potential_in, potential_out, min_index, Outcoming);
			PushFlow_(network, potential_in, potential_out, zero_potential, min_index, Outcoming, added_flow);
			PushFlow_(network, potential_out, potential_in, zero_potential, min_index, Incoming, added_flow);

			potential_in[min_index] = potential_out[min_index] = 0;
			if (min_index == network.GetSink() || min_index == network.GetSource())
				break;

			DeleteEdgesToZeroPotentialVertices_(network, potential_in, potential_out, zero_potential);
			potential_in[min_index] = potential_out[min_index] = 0;
		}
	}
};

template <typename T>
class MaximumFlowAlgorithms {
public:
	MaximumFlowAlgorithms()
		: network_() {}

	MaximumFlowAlgorithms(const Network<T>& network)
		: network_(network) {}

	MaximumFlowAlgorithms(const MaximumFlowAlgorithms<T>& other)
		: network_(other.network_) {}

	MaximumFlowAlgorithms<T>& operator=(const MaximumFlowAlgorithms<T>& other) {
		network_ = other.network_;
		return *this;
	}

	virtual T RunMaximumFlow() = 0;

protected:
	T CountCurrentOutcomingSourceFlow_() {
		T ans = 0;
		for (auto it = network_.begin(network_.GetSource(), Outcoming);
			it != network_.end(network_.GetSource(), Outcoming); ++it)
			ans += it->flow;
		return ans;
	}

	Network<T> network_;
};

template <typename T, typename BlockingSearchAlgorithm >
class BlockingFlowUsingAlgorithms :
	public MaximumFlowAlgorithms<T> {
private:
	typedef MaximumFlowAlgorithms<T> MyBase;
	typedef BlockingFlowUsingAlgorithms<T, BlockingSearchAlgorithm> MyClass;

	void MakeLayeredNetwork(const std::vector<int>& dist) {
		for (unsigned i = 0; i < MyBase::network_.GetVerticesNumber(); ++i) {
			for (unsigned direction = 0; direction < 2; ++direction)
				for (auto it = MyBase::network_.begin(i, EdgesDirection(direction)); it != MyBase::network_.end(i, EdgesDirection(direction)); )
					if ((int)dist[it->to] - (int)dist[it->from] != 1)
						MyBase::network_.ExcludeFromIteratorView(it);
					else
						++it;
		}
	}

public:

	BlockingFlowUsingAlgorithms()
		: MaximumFlowAlgorithms<T>() {}

	BlockingFlowUsingAlgorithms(const Network<T>& network)
		: MaximumFlowAlgorithms<T>(network) {}

	BlockingFlowUsingAlgorithms(const MyClass& other)
		: MaximumFlowAlgorithms<T>(other) {}

	MyClass& operator=(const MyClass& other) {
		MyBase::network_ = other.MyBase::network_;
		return *this;
	}

	T RunMaximumFlow() {
		std::vector<int> dist(MyBase::network_.GetVerticesNumber());
		while (MyBase::network_.bfs(dist)) {
			MakeLayeredNetwork(dist);
			BlockingSearchAlgorithm blocking_search_algorithm;
			blocking_search_algorithm.RunBlockingFlow(MyBase::network_);
			MyBase::network_.AddAllEdgesToIteratorView();
		}
		return MyBase::CountCurrentOutcomingSourceFlow_();
	}
};

template <typename T>
class RelabelToFrontAlgorithm :
	public MaximumFlowAlgorithms<T> {
private:
	typedef RelabelToFrontAlgorithm<T> MyClass;
	typedef MaximumFlowAlgorithms<T> MyBase;

	void InitializePreflow_(std::vector<unsigned>& height, std::vector<T>& excess_flow) {
		for (auto it = MyBase::network_.begin(MyBase::network_.GetSource(), Outcoming); it != MyBase::network_.end(MyBase::network_.GetSource(), Outcoming); ++it) {
			MyBase::network_.AddFlow(it, it->capacity);
			excess_flow[it->from] -= it->capacity;
			excess_flow[it->to] += it->capacity;
		}
		height[MyBase::network_.GetSource()] = MyBase::network_.GetVerticesNumber();
	}

	void Push_(std::vector<T>& excess_flow, typename Network<T>::iterator& it) {
		T d = std::min<T>(excess_flow[it->from], it->GetResidualCapacity());
		MyBase::network_.AddFlow(it, d);
		excess_flow[it->from] -= d;
		excess_flow[it->to] += d;
	}

	void Relabel_(std::vector<unsigned>& height, unsigned v) {
		height[v] = 2 * MyBase::network_.GetVerticesNumber();
		for (auto it = MyBase::network_.begin(v, Outcoming); it != MyBase::network_.end(v, Outcoming); ++it)
			if (it->GetResidualCapacity() > 0)
				height[v] = std::min<T>(height[v], height[it->to]);
		++height[v];
	}

	void Discharge_(std::vector<typename Network<T>::iterator>& current_iterators, std::vector<unsigned>& height, std::vector<T>& excess_flow, unsigned v) {
		while (excess_flow[v] > 0) {
			auto it = current_iterators[v];
			if (it == MyBase::network_.end(v, Outcoming)) {
				Relabel_(height, v);
				current_iterators[v] = MyBase::network_.begin(v, Outcoming);
			}
			else {
				if (it->GetResidualCapacity() > 0 && height[it->from] == height[it->to] + 1)
					Push_(excess_flow, it);
				else
					++current_iterators[v];
			}
		}
	}
public:


	RelabelToFrontAlgorithm()
		: RelabelToFrontAlgorithm<T>() {}

	RelabelToFrontAlgorithm(const Network<T>& network)
		: MaximumFlowAlgorithms<T>(network) {}

	RelabelToFrontAlgorithm(const MyClass& other)
		: MaximumFlowAlgorithms<T>(other) {}

	MyClass& operator=(const MyClass& other) {
		MyBase::network_ = other.MyBase::network_;
		return *this;
	}

	T RunMaximumFlow() {
		std::vector<unsigned> height(MyBase::network_.GetVerticesNumber());
		std::vector<T> excess_flow(MyBase::network_.GetVerticesNumber());
		InitializePreflow_(height, excess_flow);
		std::vector<typename Network<T>::iterator> current_iterators(MyBase::network_.GetVerticesNumber());
		std::list<unsigned> vertices_list;
		for (unsigned i = 0; i < MyBase::network_.GetVerticesNumber(); ++i) {
			current_iterators[i] = MyBase::network_.begin(i, Outcoming);
			if (i != MyBase::network_.GetSource() && i != MyBase::network_.GetSink()) {
				vertices_list.push_back(i);
			}
		}
		std::list<unsigned>::iterator it = vertices_list.begin();
		while (it != vertices_list.end()) {
			unsigned v = *it;
			unsigned old_height = height[v];
			Discharge_(current_iterators, height, excess_flow, v);
			if (height[v] > old_height) {
				vertices_list.erase(it);
				vertices_list.push_front(v);
				it = vertices_list.begin();
			}
			++it;
		}
		return MyBase::CountCurrentOutcomingSourceFlow_();
	}
};

struct MathanData {      //zero-numeration
	std::vector<std::vector<unsigned>> dependences;
	std::vector<int> usefulness;

	MathanData(const std::vector<std::vector<unsigned>>& _dependences, const std::vector<int>& _usefulness)
		: usefulness(_usefulness)
		, dependences(_dependences) {}
};

MathanData read(std::istream& stream) {
	unsigned n;
	stream >> n;
	std::vector<int> usefulness(n);
	for (unsigned i = 0; i < n; ++i)
		stream >> usefulness[i];
	std::vector<std::vector<unsigned>> dependences(n);
	for (unsigned i = 0; i < n; ++i) {
		unsigned m;
		stream >> m;
		for (unsigned j = 0; j < m; ++j) {
			unsigned to;
			stream >> to;
			--to;
			dependences[i].push_back(to);
		}
	}
	return MathanData(dependences, usefulness);
}

template <typename Algorithm>
long long solve(const MathanData& data) {
	long long INF_ = 1e9;
	unsigned n = data.usefulness.size();
	unsigned source = n, sink = n + 1;
	Network<long long> network(source, sink);

	std::vector<std::vector<bool>> edges_matrix(n, std::vector<bool>(n));
	for (unsigned i = 0; i < n; ++i)
		for (unsigned j = 0; j < data.dependences[i].size(); ++j)
			edges_matrix[i][data.dependences[i][j]] = true;

	long long sum = 0;
	for (int i = 0; i < n; ++i) {
		if (data.usefulness[i] >= 0) {
			sum += data.usefulness[i];
			network.AddEdge(source, i, data.usefulness[i], true);
		}
		else {
			network.AddEdge(i, sink, -data.usefulness[i], true);
		}
	}

	for (int i = 0; i < n; ++i)
		for (int j = i + 1; j < n; ++j) {
			if (edges_matrix[i][j] && edges_matrix[j][i])
				network.AddEdge(i, j, INF_, false);
			else {
				if (edges_matrix[i][j])
					network.AddEdge(i, j, INF_, true);
				if (edges_matrix[j][i])
					network.AddEdge(j, i, INF_, true);
			}
		}

	Algorithm algorithm(network);
	return sum - algorithm.RunMaximumFlow();
}

template <typename Algorithm>
void run() {
	std::cout << solve<Algorithm>(read(std::cin)) << "\n";
}

void e_olymp() {
	int n;
	scanf("%d", &n);
	while (n) {
		int s, t, m;
		scanf("%d%d%d", &s, &t, &m);
		Network<long long> network(s, t);
		for (int i = 0; i < m; ++i) {
			int x, y, c;
			scanf("%d%d%d", &x, &y, &c);
			network.AddEdge(x, y, c, false);
		}
		//BlockingFlowUsingAlgorithms<long long, MalhotraKumarMaheshwariBlockingFlowSearch<long long>> algo(network);
		RelabelToFrontAlgorithm<long long> algo(network);

		printf("%lld\n", algo.RunMaximumFlow());
	}
}

void informatics() {
	int n, m;
	scanf("%d%d", &n, &m);
	int s = 1, t = n;
	Network<long long> network(s, t);
	for (int i = 0; i < m; ++i) {
		int x, y, c;
		scanf("%d%d%d", &x, &y, &c);
		network.AddEdge(x, y, c, true);
	}
	BlockingFlowUsingAlgorithms<long long, MalhotraKumarMaheshwariBlockingFlowSearch<long long>> algo1(network);
	printf("%lld\n", algo1.RunMaximumFlow());
	/*BlockingFlowUsingAlgorithms<long long, DinicBlockingFlowSearch<long long>> algo2(network);
	printf("%lld\n", algo2.RunMaximumFlow());*/
	/*RelabelToFrontAlgorithm<long long> algo3(network);
	printf("%lld\n", algo3.RunMaximumFlow());*/
}

void tester() {
	int K = 6;
	std::vector<Edge<long long>> e;
	for (int i = 0; i < K; ++i)
		for (int j = i + 1; j < K; ++j)
			e.push_back(Edge<long long>(i, j, rand() % 10 + 1));
	int S = e.size();
	for (int mask = 1; mask < (1 << S); ++mask) {
		if (mask % 100000 == 0)
			printf("%d\n", mask);
		Network<long long> network(0, K - 1);
		for (int i = 0; i < S; ++i) {
			if (mask & (1 << i))
				network.AddEdge(e[i].from, e[i].to, e[i].capacity, true);
		}

		//BlockingFlowUsingAlgorithms<long long, MalhotraKumarMaheshwariBlockingFlowSearch<long long>> algo1(network);
		BlockingFlowUsingAlgorithms<long long, DinicBlockingFlowSearch<long long>> algo2(network);
		RelabelToFrontAlgorithm<long long> algo3(network);
		//long long ans1 = algo1.RunMaximumFlow();
		long long ans2 = algo2.RunMaximumFlow();
		long long ans3 = algo3.RunMaximumFlow();
		if (ans3 != ans2) {
			int m = 0;
			for (int i = 0; i < S; ++i)
				if (mask & (1 << i))
					++m;
			printf("%d %d\n", K, m);
			for (int i = 0; i < S; ++i) {
				if (mask & (1 << i))
					printf("%d %d %d\n", e[i].from + 1, e[i].to + 1, e[i].capacity);
			}
			printf("%lld\n\n", ans2);
		}
	}
	printf("0\n");
}

void big_tester() {
	freopen("big_tests.txt", "wt", stdout);
	for (int q = 0; q < 10000; ++q) {
		int K = 20;
		std::vector<Edge<long long>> e;
		for (int i = 0; i < K; ++i)
			for (int j = i + 1; j < K; ++j) {
				int random = rand() % 10000;
				if (random < 3000)
					e.push_back(Edge<long long>(i, j, rand() % 10));
			}
		int M = e.size();
		Network<long long> network(0, K - 1);
		for (int i = 0; i < M; ++i) {
			network.AddEdge(e[i].from, e[i].to, e[i].capacity, true);
		}
		BlockingFlowUsingAlgorithms<long long, DinicBlockingFlowSearch<long long>> algo2(network);
		long long ans2 = algo2.RunMaximumFlow();
		printf("%d %d\n", K, M);
		for (int i = 0; i < M; ++i) {
			printf("%d %d %d\n", e[i].from + 1, e[i].to + 1, e[i].capacity);
		}
		printf("%lld\n\n", ans2);
	}
	printf("0\n");
}

int main() {
	/*freopen("input.txt", "rt", stdin);
	run<RelabelToFrontAlgorithm<long long>>();
	freopen("input.txt", "rt", stdin);*/
	run<BlockingFlowUsingAlgorithms<long long, MalhotraKumarMaheshwariBlockingFlowSearch<long long>>>();
	/*freopen("input.txt", "rt", stdin);
	run<BlockingFlowUsingAlgorithms<long long, DinicBlockingFlowSearch<long long>>>();*/
	return 0;
}
