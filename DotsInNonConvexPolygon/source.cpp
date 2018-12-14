#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <type_traits>
#include <string>

namespace Geom {
	template < typename T >
	struct Point {
		T x, y;

		Point()
			: x(0)
			, y(0) {}

		Point(const T& _x, const T& _y)
			: x(_x)
			, y(_y) {}

		Point(const Point &other) = default;

		void Read(std::istream& in) {
			in >> x >> y;
		}
	};

	template <typename T>
	bool PointLowerThanSegment(const Point<T>& x, const Point<T>& a, const Point<T>& b) {
		Point<T> ax = x - a;
		Point<T> ab = b - a;
		return ax % ab > 0;
	}

	template <typename T>
	T operator*(const Point<T>& a, const Point<T>& b) {
		return a.x * b.x + a.y * b.y;
	}

	template <typename T>
	T operator%(const Point<T>& a, const Point<T>& b) {
		return a.x * b.y - a.y * b.x;
	}

	template <typename T>
	Point<T> operator+(const Point<T>& a, const Point<T>& b) {
		return Point<T>(a.x + b.x, a.y + b.y);
	}

	template <typename T>
	Point<T> operator-(const Point<T>& a, const Point<T>& b) {
		return Point<T>(a.x - b.x, a.y - b.y);
	}

	template <typename T>
	bool operator==(const Point<T>& first, const Point<T>& second) {
		return first.x == second.x && first.y == second.y;
	}

	template <typename T>
	bool LexicographicallyLess(const Point<T>& first, const Point<T>& second) {
		return first.x < second.x || first.x == second.x && first.y < second.y;
	}

	template < typename T >
	struct Segment {
		Segment()
			: A()
			, B() {}

		explicit Segment(const Point<T> &point)
			: A(point)
			, B(point) {}

		Segment(const Point<T>& _A, const Point<T>& _B)
			: A(_A)
			, B(_B) {}

		Segment(const Segment &other) = default;

		void Read(std::istream &in) {
			A.Read(in);
			B.Read(in);
		}

		Point<T> LeftPoint() const& {
			return A.x < B.x ? A : B;
		}

		Point<T> RightPoint() const& {
			return A + B - LeftPoint();
		}

		Point<T> A, B;
	};

	template <typename T>
	bool operator==(const Segment<T>& first, const Segment<T>& second) {
		return first.A == second.A && first.B == second.B ||
			first.B == second.A && first.A == second.B;
	}

	template <typename T>
	bool Below(const Segment<T> &first, const Segment<T> &second) {
		Point<T> P = first.LeftPoint(), Q = first.RightPoint();
		Point<T> R = second.LeftPoint(), S = second.RightPoint();
		Point<T> PQ = Q - P, RS = S - R;
		
		if (P == Q) 
			return PointLowerThanSegment(P, R, S);
		if (R == S) 
			return PointLowerThanSegment(R, Q, P);
		
		if (P == R) 
			return PQ % RS > 0;
		if (Q == S) 
			return RS % PQ > 0;

		if (Q.x < S.x) 
			return PointLowerThanSegment(Q, R, S);
		else
			return PointLowerThanSegment(S, Q, P);
	}
}

template < typename T >
class CartesianTree {
private:
	class Node {
	public:
		typedef Node* PNode;
		typedef std::pair<PNode, PNode> TwoNodes;

		explicit Node(const T& key)
			: key_(key)
			, priority_(MyRand_())
			, size_(1)
			, left_(nullptr)
			, right_(nullptr) {}

		Node(const Node& other) = default;

		~Node() {}

		Node& operator=(const Node& other) {
			key_ = other.key_;
			priority_ = other.priority_;
			size_ = other.size_;
			left_ = other.left_;
			right_ = other.right_;
			return *this;
		}

		static PNode Merge(PNode tree_left, PNode tree_right) {
			if (!tree_left)
				return tree_right;
			if (!tree_right)
				return tree_left;
			if (tree_left->priority_ > tree_right->priority_) {
				tree_left->right_ = Merge(tree_left->right_, tree_right);
				Update_(tree_left);
				return tree_left;
			}
			else {
				tree_right->left_ = Merge(tree_left, tree_right->left_);
				Update_(tree_right);
				return tree_right;
			}
		}

		template <typename Comparator>
		static TwoNodes Split(PNode tree, const T& value, Comparator comp) {
			if (!tree)
				return std::make_pair(tree, tree);
			if (comp(tree->key_, value)) {
				TwoNodes tmp = Split(tree->right_, value, comp);
				tree->right_ = tmp.first;
				Update_(tree);
				return std::make_pair(tree, tmp.second);
			}
			else {
				TwoNodes tmp = Split(tree->left_, value, comp);
				tree->left_ = tmp.second;
				Update_(tree);
				return std::make_pair(tmp.first, tree);
			}
		}

		static void Delete(PNode node) {
			if (node->left_)
				Delete(node->left_);
			if (node->right_)
				Delete(node->right_);
			delete node;
		}

		static void Copy(PNode from, PNode to) {
			if (!to)
				to = new Node(from->key_);
			Copyson_(from->left_, to->left_);
			Copyson_(from->right_, to->right_);
		}

		template <typename Comparator>
		static size_t GetNumberBefore(PNode tree, const T& value, Comparator comp) {
			TwoNodes tmp = Node::Split(tree, value, comp);
			size_t k = GetSize_(tmp.first);
			tree = Node::Merge(tmp.first, tmp.second);
			return k;
		}

		template <typename U, typename Comparator>
		static bool Find(PNode tree, const U& value, Comparator comp) {
			if (!tree)
				return false;
			if (!comp(tree->key_, value) && !comp(value, tree->key_))
				return true;
			if (comp(tree->key_, value))
				return Find(tree->right_, value, comp);
			else
				return Find(tree->left_, value, comp);
		}

		template <typename Comparator>
		static PNode Erase(PNode tree, const T& value, Comparator comp) {
			if (!tree)
				return tree;
			TwoNodes tmp1 = Split(tree, value, comp);
			TwoNodes tmp2 = Split(tmp1.second, value, [](const T& first, const T& second) {return first == second; });
			Delete(tmp2.first);
			return Merge(tmp1.first, tmp2.second);
		}

		static size_t Size(PNode tree) {
			return GetSize_(tree);
		}

	private:
		T key_;
		int priority_;
		size_t size_;
		PNode left_, right_;

		static int MyRand_() {
			static std::mt19937 generator;
			static std::uniform_int_distribution<size_t> distributor;
			return distributor(generator);
		}

		static void CopySon_(PNode from, PNode to) {
			if (from)
				Copy(from, to);
			else
				to = nullptr;
		}

		static size_t GetSize_(PNode tree) {
			return tree ? tree->size_ : 0;
		}

		static void UpdateSize_(PNode tree) {
			tree->size_ = 1 + GetSize_(tree->left_) + GetSize_(tree->right_);
		}

		static void Update_(PNode tree) {
			if (!tree)
				return;
			UpdateSize_(tree);
		}
	};

	typedef Node* PNode;
	typedef std::pair<PNode, PNode> TwoNodes;
	typedef std::pair<CartesianTree<T>, CartesianTree<T>> TwoTrees;

	CartesianTree(PNode node)
		: tree_(node) {}

	PNode tree_;
public:

	CartesianTree()
		: tree_(nullptr) {}

	CartesianTree(const CartesianTree& other) = default;

	~CartesianTree() {
		if (tree_)
			Node::Delete(tree_);
		tree_ = nullptr;
	}

	CartesianTree& operator=(const CartesianTree& other) {
		Node::Delete(tree_);
		Node::Copy(other.tree_, tree_);
		return *this;
	}

	template <typename Comparator>
	void Insert(const T& value, Comparator comp) {
		PNode v = new Node(value);
		if (!tree_) {
			tree_ = v;
		}
		else {
			TwoNodes tmp1 = Node::Split(tree_, value, comp);
			tree_ = Node::Merge(tmp1.first, Node::Merge(v, tmp1.second));
		}
	}

	template <typename Comparator>
	void Erase(const T& value, Comparator comp) {
		tree_ = Node::Erase(tree_, value, comp);
	}

	template <typename Comparator>
	size_t GetNumberBefore(const T& value, Comparator comp) const {
		return Node::GetNumberBefore(tree_, value, comp);
	}

	template <typename U, typename Comparator>
	bool Find(const U& value, Comparator comp) const {
		return Node::Find(tree_, value, comp);
	}

	size_t Size() const& {
		return tree_ ? Node::Size(tree_) : 0;
	}
};

namespace ReadSolveWrite {

	enum EEventType {
		EET_CheckPointInside = 0,
		EET_CheckPointOnBorder1 = 1,
		EET_SegmentEnd = 2,
		EET_SegmentBegin = 3,
		EET_CheckPointOnBorder2 = 4,
		EET_VerticalSegmentBegin = 6,
		EET_CheckPointOnVerticalSegment = 7,
		EET_VerticalSegmentEnd = 5
	};

	enum EPointPosition {
		EPP_Border = 0,
		EPP_Inside = 1,
		EPP_Outside = 2
	};

	std::string PointPositionToString(EPointPosition position) {
		switch (position)
		{
		case EPP_Border:
			return "BORDER";
		case EPP_Inside:
			return "INSIDE";
		case EPP_Outside:
			return "OUTSIDE";
		}
	}

	template <typename T>
	struct Event {
		Geom::Point<T> point;
		size_t id;           //point or segment id
		EEventType event_type;

		Event()
			: point()
			, id(0)
			, event_type() {}

		Event(Geom::Point<T> _point, size_t _id, EEventType _event_type)
			: point(_point)
			, id(_id)
			, event_type(_event_type) {}

		Event(const Event& other) = default;
	};

	template <typename T>
	bool operator<(const Event<T>& first, const Event<T>& second) {
		if (first.event_type >= EET_VerticalSegmentEnd && second.event_type >= EET_VerticalSegmentEnd)
			return Geom::LexicographicallyLess<T>(first.point, second.point) || first.point == second.point && 
				first.event_type < second.event_type;
		return first.point.x < second.point.x || first.point.x == second.point.x && first.event_type < second.event_type;
	}

	template <typename T>
	struct ProblemData {
		std::vector<Geom::Segment<T>> segments;
		std::vector<Geom::Point<T>> points;
		std::vector<EPointPosition> point_positions;

		ProblemData<T>& operator=(const ProblemData<T>& other) {
			segments = other.segments;
			points = other.points;
			point_positions = other.point_positions;
			return *this;
		}

		void ReadData(std::istream& in) {
			size_t n;
			in >> n;

			segments.resize(n);
			segments[0].Read(in);
			Geom::Point<T> tmp;
			for (size_t i = 1; i < n - 1; ++i) {
				tmp.Read(in);
				segments[i] = Geom::Segment<T>(segments[i - 1].B, tmp);
			}
			segments[n - 1] = Geom::Segment<T>(segments[n - 2].B, segments[0].A);

			size_t k;
			in >> k;
			points.resize(k);
			point_positions.resize(k, EPP_Outside);
			for (size_t i = 0; i < k; ++i) {
				points[i].Read(in);
			}
		}

		void WritePositions(std::ostream& out) {
			for (auto position : point_positions) {
				out << PointPositionToString(position) << "\n";
			}
		}
	};

	template <typename T>
	std::vector<ProblemData<T>> MultiReadData(std::istream& in) {
		size_t t;
		in >> t;
		std::vector<ProblemData<T>> data(t);
		for (int i = 0; i < t; ++i)
			data[i].ReadData(in);
		return data;
	}

	template <typename T>
	class Solver {
	private:
		void BuildEvents() {
			events_.resize(0);

			for (size_t i = 0; i < data_.segments.size(); ++i) {
				Geom::Point<T> first = data_.segments[i].A;
				Geom::Point<T> second = data_.segments[i].B;
				if (first == second)
					continue;
				if (first.x > second.x)
					std::swap(first, second);
				if (first.x != second.x) {
					events_.push_back(Event<T>(first, i, EET_SegmentBegin));
					events_.push_back(Event<T>(second, i, EET_SegmentEnd));
				}
				else {
					if (first.y > second.y)
						std::swap(first, second);
					events_.push_back(Event<T>(first, i, EET_VerticalSegmentBegin));
					events_.push_back(Event<T>(second, i, EET_VerticalSegmentEnd));
				}
			}

			for (size_t i = 0; i < data_.points.size(); ++i) {
				events_.push_back(Event<T>(data_.points[i], i, EET_CheckPointInside));
				events_.push_back(Event<T>(data_.points[i], i, EET_CheckPointOnBorder1));
				events_.push_back(Event<T>(data_.points[i], i, EET_CheckPointOnBorder2));
				events_.push_back(Event<T>(data_.points[i], i, EET_CheckPointOnVerticalSegment));
			}
		}

		ProblemData<T> data_;
		std::vector<Event<T>> events_;

	public:
		void solve() {
			BuildEvents();
			std::sort(events_.begin(), events_.end());
			CartesianTree<Geom::Segment<T>> tree;
			bool open_vertical = false;
			for (auto evnt : events_) {
				switch (evnt.event_type) {
				case EET_SegmentBegin:
					tree.Insert(data_.segments[evnt.id], Geom::Below<T>);
					break;
				case EET_SegmentEnd:
					tree.Erase(data_.segments[evnt.id], Geom::Below<T>);
					break;
				case EET_CheckPointOnBorder1:
				case EET_CheckPointOnBorder2:
					if (tree.Find(Geom::Segment<T>(data_.points[evnt.id]), Geom::Below<T>))
						data_.point_positions[evnt.id] = EPP_Border;
					break;
				case EET_CheckPointInside:
					if (data_.point_positions[evnt.id] != EPP_Border &&
						tree.GetNumberBefore(Geom::Segment<T>(data_.points[evnt.id]), Geom::Below<T>) % 2)
						data_.point_positions[evnt.id] = EPP_Inside;
					break;
				case EET_VerticalSegmentBegin:
					open_vertical = true;
					break;
				case EET_CheckPointOnVerticalSegment:
					if (open_vertical)
						data_.point_positions[evnt.id] = EPP_Border;
					break;
				case EET_VerticalSegmentEnd:
					open_vertical = false;
					break;
				}
			}
		}

		void SetData(const ProblemData<T>& data){
			data_ = data;
		}

		ProblemData<T> GetData() {
			return data_;
		}
	};
}

void Run(std::istream& in, std::ostream& out) {
	std::vector<ReadSolveWrite::ProblemData<long long>> data_vector = ReadSolveWrite::MultiReadData<long long>(in);
	ReadSolveWrite::Solver<long long> solver;
	for (int i = 0; i < data_vector.size(); ++i) {
		solver.SetData(data_vector[i]);
		solver.solve();
		data_vector[i] = solver.GetData();
		data_vector[i].WritePositions(out);
	}
}

int main() {
	std::ifstream in("input.txt");
	Run(in, std::cout);
	return 0;
}
