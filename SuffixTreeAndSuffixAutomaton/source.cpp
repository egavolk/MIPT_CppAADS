#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <string>
#include <fstream>

namespace suff_tree {

    const int INF = 1e9;

    struct SuffTreePosition {
        SuffTreePosition()
                : v(-1)
                , dist(0) {}

        explicit SuffTreePosition(int _v, int _dist)
                : v(_v), dist(_dist) {}

        SuffTreePosition(const SuffTreePosition& other) = default;

        SuffTreePosition(SuffTreePosition&& other) = default;

        SuffTreePosition& operator=(const SuffTreePosition& other) = default;

        SuffTreePosition& operator=(SuffTreePosition&& other) = default;

        ~SuffTreePosition() = default;

        int v;
        size_t dist;
    };

    struct Node {
        explicit Node(size_t alphabet_size, int _parent, int _parent_symbol, size_t _edge_end, int _link = -1)
                : children(alphabet_size, SuffTreePosition())
                , link(_link)
                , edge_end(_edge_end)
                , parent(_parent)
                , parent_symbol(_parent_symbol) {}

        Node(const Node& other) = default;

        Node(Node&& other) = default;

        Node& operator=(const Node& other) = default;

        Node& operator=(Node&& other) = default;

        ~Node() = default;

        std::vector<SuffTreePosition> children;
        int link, parent;
        size_t edge_end;
        int parent_symbol;
    };

    class SuffTree {
    private:
        std::vector<Node> nodes_;
        std::vector<int> sequence_;
        SuffTreePosition pos_last_not_leaf_;
        size_t alphabet_size_;
        int first_symbol_;

        int MakeNewNode_(SuffTreePosition pos) {
            if (pos.dist == 0)
                return pos.v;
            int v = pos.v, pv = nodes_[v].parent;
            size_t edge_end = nodes_[v].edge_end;
            size_t edge_begin = edge_end - nodes_[pv].children[nodes_[v].parent_symbol].dist;
            size_t edge_pos = edge_end - pos.dist;
            nodes_.push_back(Node(alphabet_size_, pv, sequence_[edge_begin], edge_pos));
            int new_v = nodes_.size() - 1;
            nodes_[new_v].children[sequence_[edge_pos]] = SuffTreePosition(v, edge_end - edge_pos);
            nodes_[pv].children[sequence_[edge_begin]] = SuffTreePosition(new_v, edge_pos - edge_begin);
            nodes_[v].parent = new_v;
            nodes_[v].parent_symbol = sequence_[edge_pos];
            return new_v;
        }

        int FindLink_(int v) {
            if (v <= 0)
                return -1;
            int pv = nodes_[v].parent;
            SuffTreePosition pos(GetLink(pv), 0);
            size_t edge_end = nodes_[v].edge_end;
            size_t edge_begin = nodes_[v].edge_end - nodes_[pv].children[nodes_[v].parent_symbol].dist + (pv == 0);
            while (edge_begin < edge_end && edge_begin < sequence_.size()) {
                if (pos.dist == 0)
                    pos = nodes_[pos.v].children[sequence_[edge_begin]];
                size_t left = std::min(sequence_.size() - edge_begin, std::min(edge_end - edge_begin, pos.dist));
                edge_begin += left;
                pos.dist -= left;
            }
            return MakeNewNode_(pos);
        }

    public:
        explicit SuffTree(size_t alphabet_size, int first_symbol)
                : nodes_(1, Node(alphabet_size, -1, -1, 0, 0))
                , alphabet_size_(alphabet_size)
                , first_symbol_(first_symbol)
                , pos_last_not_leaf_(0, 0)
                , sequence_() {}

        SuffTree(const SuffTree& other) = default;

        SuffTree(SuffTree&& other) = default;

        SuffTree& operator=(const SuffTree& other) = default;

        SuffTree& operator=(SuffTree&& other) = default;

        ~SuffTree() = default;

        bool CanGo(SuffTreePosition pos, int c) const {
            if (pos.dist == 0)
                return nodes_[pos.v].children[c].v != -1;
            else
                return c == sequence_[nodes_[pos.v].edge_end - pos.dist];
        }

        SuffTreePosition Go(SuffTreePosition pos, int c) {
            if (pos.dist == 0)
                pos = nodes_[pos.v].children[c];
            --pos.dist;
            return pos;
        }

        int GetLink(int v) {
            if (nodes_[v].link == -1) {
                int link = FindLink_(v);
                nodes_[v].link = link;
            }
            return nodes_[v].link;
        }

        void Add(int c) {
            c -= first_symbol_;
            sequence_.push_back(c);
            while (!CanGo(pos_last_not_leaf_, c)) {
                int v = MakeNewNode_(pos_last_not_leaf_);
                nodes_[v].children[c] = SuffTreePosition(nodes_.size(), INF - sequence_.size() + 1);
                nodes_.push_back(Node(alphabet_size_, v, c, INF));
                pos_last_not_leaf_ = SuffTreePosition(GetLink(v), 0);
                if (!v)
                    return;
            }
            pos_last_not_leaf_ = Go(pos_last_not_leaf_, c);
        }

        void Print() const {
            for (int i = 0; i < nodes_.size(); ++i) {
                printf("%d link = %d : ", i, nodes_[i].link);
                for (int j = 0; j < nodes_[i].children.size(); ++j)
                    if (nodes_[i].children[j].v != -1)
                        printf("%d -> (%d, %d); ", j + first_symbol_, nodes_[i].children[j].v, nodes_[i].children[j].dist);
                printf("\n");
            }
        }

        template <typename T>
        void Build(const T &sequence) {
            pos_last_not_leaf_ = SuffTreePosition(0, 0);
            nodes_.assign(1, Node(alphabet_size_, -1, -1, 0, 0));
            for (int i = 0; i < sequence.size(); ++i) {
                Add(sequence[i]);
            }
        }

        size_t GetNodesNumber() const {
            return nodes_.size();
        }

        size_t GetSequenceSize() const {
            return sequence_.size();
        }

        Node GetNode(int v) const {
            return nodes_[v];
        }

        template <typename T>
        T GetSubSequence(int v) const {
            T answer;
            for (int u = v; u > 0; u = nodes_[u].parent) {
                int pv = nodes_[u].parent;
                size_t end = nodes_[u].edge_end;
                int begin = end - nodes_[pv].children[nodes_[u].parent_symbol].dist;
                for (int i = std::min(end, sequence_.size()) - 1; i >= begin; --i)
                    answer.push_back(sequence_[i] + first_symbol_);
            }
            std::reverse(answer.begin(), answer.end());
            return answer;
        }
    };

    struct NodesData {
        NodesData()
                : cnt(-1)
                , len(-1)
                , terminal(false) {}

        int len, cnt;
        bool terminal;
    };

    class SuffTreeUser {
    private:
        void DfsCountLen_(int v, int len) {
            nodes_data_[v].len = len;
            if (len > tree_.GetSequenceSize())
                nodes_data_[v].len = tree_.GetSequenceSize() - (suff_tree::INF - len);
            for (auto u : tree_.GetNode(v).children)
                if (u.v != -1)
                    DfsCountLen_(u.v, len + u.dist);
        }

        int DfsCountCnt_(int v) {
            if (nodes_data_[v].cnt != -1)
                return nodes_data_[v].cnt;
            nodes_data_[v].cnt = nodes_data_[v].terminal;
            for (auto u : tree_.GetNode(v).children)
                if (u.v != -1)
                    nodes_data_[v].cnt += DfsCountCnt_(u.v);
            return nodes_data_[v].cnt;
        }

        bool CheckCorrect_() {
            return tree_.GetSequenceSize() == begin_size;
        }

        SuffTree &tree_;
        std::vector<NodesData> nodes_data_;
        size_t begin_size;

    public:
        explicit SuffTreeUser(SuffTree& tree)
            : tree_(tree)
            , begin_size(tree.GetSequenceSize()) {
            nodes_data_.resize(tree_.GetNodesNumber());
        }

        void CountTerminalVertices() {
            if (!CheckCorrect_())
                return;
            for (int v = 1; v > 0; v = tree_.GetLink(v)) {
                if (v >= nodes_data_.size())
                    nodes_data_.resize(v + 1);
                nodes_data_[v].terminal = true;
            }
        }

        void CountLen() {
            if (!CheckCorrect_())
                return;
            DfsCountLen_(0, 0);
        }

        void CountCnt() {
            if (!CheckCorrect_())
                return;
            DfsCountCnt_(0);
        }

        void CountNodesData() {
            CountTerminalVertices();
            CountCnt();
            CountLen();
        }

        size_t GetSize() const {
            return nodes_data_.size();
        }

        NodesData GetNodesData(int v) const {
            return nodes_data_[v];
        }

        template <typename T>
        T GetSubSequence(int v) const {
            return tree_.GetSubSequence<T>(v);
        }
    };
}

namespace suff_automaton {
    struct Node {
        Node() = default;

        explicit Node(size_t _alphabet_size, int _longest_parent,
                      int _longest_parent_symbol = -1, size_t _len = 0, int _link = -1)
                : len(_len)
                , link(_link)
                , longest_parent(_longest_parent)
                , longest_parent_symbol(_longest_parent_symbol)
                , next(_alphabet_size, -1) {}

        Node(const Node& other) = default;

        Node(Node&& other) = default;

        Node& operator=(const Node& other) = default;

        Node& operator=(Node&& other) = default;

        ~Node() = default;

        int link, longest_parent, longest_parent_symbol;
        size_t len;
        std::vector<int> next;
    };

    class Automaton {
    private:
        std::vector<Node> nodes_;
        size_t alphabet_size_;
        int first_symbol_;
        size_t last_;

    public:
        Automaton() = default;

        explicit Automaton(size_t alphabet_size, int first_symbol)
                : alphabet_size_(alphabet_size)
                , first_symbol_(first_symbol)
                , last_(0)
                , nodes_(1, Node(alphabet_size, 0)) {}

        Automaton(const Automaton &other) = default;

        Automaton(Automaton&& other) = default;

        Automaton& operator=(const Automaton& other) = default;

        Automaton& operator=(Automaton&& other) = default;

        ~Automaton() = default;

        void AddSymbol(int c) {
            c -= first_symbol_;
            size_t cur = nodes_.size();
            nodes_.push_back(Node(alphabet_size_, last_, c, nodes_[last_].len + 1));
            int v;
            for (v = last_; v != -1 && (nodes_[v].next[c] == -1); v = nodes_[v].link)
                nodes_[v].next[c] = cur;
            if (v == -1)
                nodes_[cur].link = 0;
            else {
                int u = nodes_[v].next[c];
                if (nodes_[v].len + 1 == nodes_[u].len)
                    nodes_[cur].link = u;
                else {
                    size_t clone = nodes_.size();
                    nodes_.push_back(Node(alphabet_size_, v, c, nodes_[v].len + 1, nodes_[u].link));
                    nodes_.back().next = nodes_[u].next;
                    for (; v != -1 && nodes_[v].next[c] == u; v = nodes_[v].link)
                        nodes_[v].next[c] = clone;
                    nodes_[u].link = nodes_[cur].link = clone;
                }
            }
            last_ = cur;
        }

        template <typename T>
        void Build(const T &numbers) {
            last_ = 0;
            nodes_.assign(1, Node(alphabet_size_, -1, 0));
            for (auto c : numbers)
                AddSymbol(c);
        }

        void Print() {
            for (int i = 0; i < nodes_.size(); ++i) {
                printf("%d len = %d: ", i, nodes_[i].len);
                for (int j = 0; j < nodes_[i].next.size(); ++j)
                    if (nodes_[i].next[j] != -1)
                        printf("%d -> %d ; ", j + first_symbol_, nodes_[i].next[j]);
                printf("\n");
            }
        }

        template  <typename T>
        T GetSubSequence(int v) {
            T answer;
            while (v != 0) {
                int c = nodes_[v].longest_parent_symbol;
                answer.push_back(c + first_symbol_);
                v = nodes_[v].longest_parent;
            }
            std::reverse(answer.begin(), answer.end());
            return answer;
        }

        size_t GetSize() const& {
            return nodes_.size();
        }

        int GetLast() const& {
            return last_;
        }

        Node GetNode(int v) const& {
            return nodes_[v];
        }
    };

    struct NodesData {
        NodesData()
                : cnt(-1)
                , len(-1)
                , terminal(false) {}

        int cnt, len;
        bool terminal;
    };

    class AutomatonUser {
    private:
        Automaton& automaton_;
        std::vector<NodesData> nodes_data_;
        size_t begin_size;

        int DfsCountCnt(int v) {
            if (nodes_data_[v].cnt != -1)
                return nodes_data_[v].cnt;
            nodes_data_[v].cnt = nodes_data_[v].terminal ? 1 : 0;
            for (auto u : automaton_.GetNode(v).next)
                if (u != -1)
                    nodes_data_[v].cnt += DfsCountCnt(u);
            return nodes_data_[v].cnt;
        }

        bool CheckCorrect_() {
            return automaton_.GetSize() == begin_size;
        }
    public:
        explicit AutomatonUser(Automaton& automaton)
                : automaton_(automaton)
                , begin_size(automaton.GetSize()) {
            nodes_data_.resize(automaton.GetSize());
        }

        void CountTerminalVertices() {
            if (!CheckCorrect_())
                return;
            for (int v = automaton_.GetLast(); v != 0; v = automaton_.GetNode(v).link)
                nodes_data_[v].terminal = true;
        }

        void CountCnt() {
            if (!CheckCorrect_())
                return;
            DfsCountCnt(0);
        }

        void CountLen() {
            if (!CheckCorrect_())
                return;
            for (int i = 0; i < automaton_.GetSize(); ++i)
                nodes_data_[i].len = automaton_.GetNode(i).len;
        }

        void CountNodesData() {
            CountTerminalVertices();
            CountCnt();
            CountLen();
        }

        size_t GetSize() const& {
            return nodes_data_.size();
        }

        NodesData GetNodesData(int v) const {
            return nodes_data_[v];
        }

        template  <typename T>
        T GetSubSequence(int v) {
            return automaton_.GetSubSequence<T>(v);
        }
    };
}

struct InputData {
    std::vector<int> sequence;
    int alphabet_size;
};

std::istream& operator>>(std::istream &in, InputData& data) {
    int n;
    in >> n >> data.alphabet_size;
    data.sequence.resize(n);
    for (int i = 0; i < n; ++i)
        in >> data.sequence[i];
    return in;
}

struct OutputData {
    std::vector<int> refren;
    long long refren_value;
};

std::ostream& operator<<(std::ostream &out, OutputData& data) {
    out << data.refren_value << '\n';
    out << data.refren.size() << '\n';
    for (int i = 0; i < data.refren.size(); ++i)
        out << data.refren[i] << ' ';
    out << '\n';
    return out;
}

template <typename SuffStructure, typename SuffStructureUser, typename NodesType>
OutputData Solve(const InputData& input) {
    SuffStructure structure(input.alphabet_size + 1, 1);
    structure.template Build<std::vector<int>>(input.sequence);
    //structure.Print();
    SuffStructureUser structure_user(structure);
    structure_user.CountNodesData();
    long long ans = 0;
    int ans_v = 0;
    for (int i = 0; i < structure_user.GetSize(); ++i) {
        NodesType node_data = structure_user.GetNodesData(i);
        long long cur = 1ll * node_data.len * node_data.cnt;
        if (cur > ans) {
            ans = cur;
            ans_v = i;
        }
    }
    return {structure_user.template GetSubSequence<std::vector<int>>(ans_v), ans};
}

void Run(std::istream& in, std::ostream& out) {
    InputData input;
    in >> input;
    OutputData output = Solve<suff_tree::SuffTree, suff_tree::SuffTreeUser, suff_tree::NodesData>(input);
    //OutputData output = Solve<suff_automaton::Automaton, suff_automaton::AutomatonUser, suff_automaton::NodesData>(input);
    out << output;
}

int main() {
    std::ifstream in("input.txt");
    Run(std::cin, std::cout);
    return 0;
}
