#pragma once
#include <iostream>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <climits>
#include <numeric>
#include <random>

class CartesianTree {
private:

    class Node {
    public:
        typedef Node* PNode;
        typedef std::pair < PNode, PNode > TwoNodes;

        enum PermutationType { Next = 0, Prev = 1 };

        explicit Node(long long key = 0)
                : key_(key)
                , number_(1)
                , left_(nullptr)
                , right_(nullptr)
                , first_(key)
                , last_(key)
                , sum_(key)
                , isNotIncreasing_(true)
                , isNotDecreasing_(true)
                , isReversed_(false)
                , promisedAddValue_(0)
                , promisedAssignValue_(LLONG_MAX)
                , isPromisedAssignValue_(false) {
            y_ = MyRand_();
        }

        Node(const Node& other)
                : key_(other.key_)
                , number_(other.number_)
                , left_(nullptr)
                , right_(nullptr)
                , first_(other.first_)
                , last_(other.last_)
                , sum_(other.sum_)
                , isNotIncreasing_(other.isNotIncreasing_)
                , isNotDecreasing_(other.isNotDecreasing_)
                , isReversed_(other.isReversed_)
                , promisedAddValue_(other.promisedAddValue_)
                , promisedAssignValue_(other.promisedAssignValue_)
                , isPromisedAssignValue_(other.isPromisedAssignValue_) {

        }

        ~Node()
        {

        }

        Node& operator=(const Node& other) {
            key_ = other.key_;
            number_ = other.number_;
            left_ = nullptr;
            right_ = nullptr;
            first_ = other.first_;
            last_ = other.last_;
            sum_ = other.sum_;
            isNotIncreasing_ = other.isNotIncreasing_;
            isNotDecreasing_ = other.isNotDecreasing_;
            isReversed_ = other.isReversed_;
            promisedAddValue_ = other.promisedAddValue_;
            promisedAssignValue_ = other.promisedAssignValue_;
            isPromisedAssignValue_ = other.isPromisedAssignValue_;
            return *this;
        }

        template<class Function>
        static void DoOperation(PNode &Tree, size_t l, size_t r, Function OperationType) {
            if (!Tree)
                return;
            TwoNodes tmp1 = Node::Split(Tree, r);
            TwoNodes tmp2 = Node::Split(tmp1.first, l - 1);
            OperationType(tmp2.second);
            Tree = Merge(tmp2.first, Merge(tmp2.second, tmp1.second));
        }

        static long long GetSum(PNode &Tree, size_t l, size_t r) {
            long long ans = 0;
            DoOperation(Tree, l, r, [&ans](PNode &node)->void { ans = GetSum_(node); });
            return ans;
        }

        static void AssignValue(PNode &Tree, size_t l, size_t r, long long x) {
            DoOperation(Tree, l, r, [x](PNode &node)->void {
                VertexAssignValue_(node, x);
                UpdateNodeByPromisedAssignValue_(node, x); 
            });
        }

        static void AddValue(PNode &Tree, size_t l, size_t r, long long x) {
            DoOperation(Tree, l, r, [x](PNode &node)->void {
                VertexAddValue_(node, x);
                UpdateNodeByPromisedAddValue_(node, x);
            });
        }

        static void Erase(PNode& Tree, size_t k) {
            DoOperation(Tree, k, k, [](PNode &node)->void {
                Delete(node);
                node = nullptr;
            });
        }

        static void NextPermutation(PNode &Tree, size_t l, size_t r) {
            DoOperation(Tree, l, r, [](PNode &node)->void {
                DoPermutation_(node, Next);
            });
        }

        static void PrevPermutation(PNode &Tree, size_t l, size_t r) {
            DoOperation(Tree, l, r, [](PNode &node)->void {
                DoPermutation_(node, Prev);
            });
        }

        void Print(std::vector < long long > &out) {
            if (this == nullptr)
                return;
            Push_();
            if (left_ != nullptr)
                left_->Print(out);
            out.push_back(key_);
            if (right_ != nullptr)
                right_->Print(out);
            Update_();
        }

        static PNode Merge(PNode TreeLeft, PNode TreeRight) {
            if (TreeLeft == nullptr)
                return TreeRight;
            if (TreeRight == nullptr)
                return TreeLeft;
            TreeLeft->Push_();
            TreeRight->Push_();
            if (TreeLeft->y_ > TreeRight->y_)
            {
                TreeLeft->right_ = Merge(TreeLeft->right_, TreeRight);
                TreeLeft->Update_();
                return TreeLeft;
            }
            else
            {
                TreeRight->left_ = Merge(TreeLeft, TreeRight->left_);
                TreeRight->Update_();
                return TreeRight;
            }
        }

        static TwoNodes Split(PNode Tree, long long k) {
            if (Tree == nullptr)
                return std::make_pair(Tree, Tree);
            Tree->Push_();
            if (GetNumber_(Tree->left_) >= k)
            {
                TwoNodes tmp = Split(Tree->left_, k);
                Tree->left_ = tmp.second;
                Tree->Update_();
                return std::make_pair(tmp.first, Tree);
            }
            else
            {
                TwoNodes tmp = Split(Tree->right_, k - GetNumber_(Tree->left_) - 1);
                Tree->right_ = tmp.first;
                Tree->Update_();
                return std::make_pair(Tree, tmp.second);
            }
        }

        static void Delete(PNode node)
        {
            if (node->left_)
                Delete(node->left_);
            if (node->right_)
                Delete(node->right_);
            delete node;
        }

        static void Copy(PNode from, PNode to)
        {
            if (to == nullptr)
                to = new Node(*from);
            CopySon_(from->left_, to->left_);
            CopySon_(from->right_, to->right_);
        }

    private:
        long long key_;
        int y_;
        size_t number_;
        long long sum_;
        Node *left_, *right_;
        long long first_, last_;
        bool isNotIncreasing_, isNotDecreasing_;
        bool isReversed_;
        long long promisedAddValue_, promisedAssignValue_;
        bool isPromisedAssignValue_;

        static int MyRand_() {
            static std::mt19937 generator;
            static std::uniform_int_distribution <size_t> distributor;
            return distributor(generator);
        }

        static void CopySon_(PNode from, PNode to)
        {
            if (from)
                Copy(from, to);
            else
                to = nullptr;
        }

        static const size_t GetNumber_(PNode Tree) {
            return Tree ? Tree->number_ : 0;
        }

        static const long long GetSum_(PNode Tree) {
            return Tree ? Tree->sum_ : 0;
        }

        void UpdateNumber_() {
            number_ = 1 + GetNumber_(left_) + GetNumber_(right_);
        }

        void UpdateSum_() {
            sum_ = key_ + GetSum_(left_) + GetSum_(right_);
        }

        void UpdateLast_() {
            last_ = right_ ? right_->last_ : key_;
        }

        void UpdateFirst_() {
            first_ = left_ ? left_->first_ : key_;
        }

        void UpdateIsNotIncreasing_() {
            isNotIncreasing_ = (left_ ? left_->isNotIncreasing_ && left_->last_ >= key_ : true) &&
                    (right_ ? right_->isNotIncreasing_ && key_ >= right_->first_ : true);
        }

        void UpdateIsNotDecreasing_() {
            isNotDecreasing_ = (left_ ? left_->isNotDecreasing_ && left_->last_ <= key_ : true) &&
                    (right_ ? right_->isNotDecreasing_ && key_ <= right_->first_ : true);
        }

        void Update_() {
            if (this == nullptr)
                return;
            PushSons_();
            UpdateNumber_();
            UpdateSum_();

            UpdateFirst_();
            UpdateLast_();

            UpdateIsNotDecreasing_();
            UpdateIsNotIncreasing_();
        }

        static void VertexAddValue_(PNode Tree, long long x) {
            if (Tree == nullptr)
                return;
            if (Tree->isPromisedAssignValue_)
                Tree->promisedAssignValue_ += x;
            else
                Tree->promisedAddValue_ += x;
        }

        static void VertexAssignValue_(PNode Tree, long long x) {
            if (Tree == nullptr)
                return;
            Tree->promisedAddValue_ = 0;
            Tree->promisedAssignValue_ = x;
            Tree->isPromisedAssignValue_ = true;
        }

        static void UpdateNodeByPromisedAssignValue_(PNode Tree, long long x) {
            if (Tree == nullptr)
                return;
            Tree->key_ = x;
            Tree->last_ = x;
            Tree->first_ = x;
            Tree->sum_ = 1ll * x * GetNumber_(Tree);
            Tree->isNotIncreasing_ = true;
            Tree->isNotDecreasing_ = true;
        }

        static void UpdateNodeByPromisedAddValue_(PNode Tree, long long x) {
            if (Tree == nullptr)
                return;
            Tree->key_ += x;
            Tree->last_ += x;
            Tree->first_ += x;
            Tree->sum_ += 1ll * x * GetNumber_(Tree);
        }

        static void Reverse_(PNode Tree) {
            if (Tree == nullptr)
                return;
            Tree->isReversed_ ^= true;
        }

        void PushPromisedAssignValue_() {
            if (isPromisedAssignValue_) {
                UpdateNodeByPromisedAssignValue_(left_, promisedAssignValue_);
                UpdateNodeByPromisedAssignValue_(right_, promisedAssignValue_);
                VertexAssignValue_(left_, promisedAssignValue_);
                VertexAssignValue_(right_, promisedAssignValue_);
                promisedAssignValue_ = LLONG_MAX;
                isPromisedAssignValue_ = false;
                promisedAddValue_ = 0;
            }
        }

        void PushPromisedAddValue_() {
            if (promisedAddValue_) {
                UpdateNodeByPromisedAddValue_(left_, promisedAddValue_);
                UpdateNodeByPromisedAddValue_(right_, promisedAddValue_);
                VertexAddValue_(left_, promisedAddValue_);
                VertexAddValue_(right_, promisedAddValue_);
                promisedAddValue_ = 0;
            }
        }

        void PushIsReversed_() {
            if (isReversed_) {
                Reverse_(this);
                std::swap(left_, right_);
                std::swap(last_, first_);
                std::swap(isNotIncreasing_, isNotDecreasing_);
                Reverse_(left_);
                Reverse_(right_);
            }
        }

        void Push_() {
            if (this == nullptr)
                return;
            PushPromisedAssignValue_();
            PushPromisedAddValue_();
            PushIsReversed_();
        }

        void PushSons_() {
            if (left_)
                left_->Push_();
            if (right_)
                right_->Push_();
        }

        static bool IsOrdered_(PNode Tree, PermutationType permutationType) {
            return !Tree ? true :
                (permutationType == Next ? Tree->isNotIncreasing_ : Tree->isNotDecreasing_);
        }

        static bool CompareDependingOnPermutationType_(long long first, long long second, PermutationType permutationType) {
            return permutationType == Next ? first < second : first > second;
        }

        static long long GetNumberOfElementsBeforeStrictlyOrderedSuffix_(PNode Tree, PermutationType permutationType) {
            if (Tree == nullptr)
                return 0;
            Tree->Push_();
            Tree->PushSons_();
            if ((Tree->right_) && (!IsOrdered_(Tree->right_, permutationType))) {
                return 1 + GetNumber_(Tree->left_) + GetNumberOfElementsBeforeStrictlyOrderedSuffix_(Tree->right_, permutationType);
            }
            if ((Tree->right_) && (CompareDependingOnPermutationType_(Tree->key_, Tree->right_->first_, permutationType))) {
                return 1 + GetNumber_(Tree->left_);
            }
            if ((Tree->left_) && (CompareDependingOnPermutationType_(Tree->left_->last_, Tree->key_, permutationType))) {
                return GetNumber_(Tree->left_);
            }
            return GetNumberOfElementsBeforeStrictlyOrderedSuffix_(Tree->left_, permutationType);
        }

        static bool GoRightInGetIndexOfElementsMoreOrLessXDependingOnPermutation_(PNode Tree, long long x, PermutationType permutationType) {
            if (permutationType == Next)
                return !(Tree->left_ && (Tree->left_->last_ > x)) && !(Tree->key_ > x);
            else
                return (Tree->right_) && (Tree->right_->first_ < x);
        }

        static bool GoLeftInGetIndexOfElementsMoreOrLessXDependingOnPermutation_(PNode Tree, long long x, PermutationType permutationType) {
            if (permutationType == Next)
                return ((Tree->left_) && (Tree->left_->last_ > x));
            else
                return !(Tree->right_ && (Tree->right_->first_ < x)) && !(Tree->key_ < x);
        }

        static long long GetIndexOfElementsMoreOrLessXDependingOnPermutation_(PNode Tree, long long x, PermutationType permutationType) {
            if (Tree == nullptr)
                return 0;
            Tree->Push_();
            Tree->PushSons_();

            if (GoRightInGetIndexOfElementsMoreOrLessXDependingOnPermutation_(Tree, x, permutationType))
                return GetIndexOfElementsMoreOrLessXDependingOnPermutation_(Tree->right_, x, permutationType) +
                        GetNumber_(Tree->left_) + 1;

            if (GoLeftInGetIndexOfElementsMoreOrLessXDependingOnPermutation_(Tree, x, permutationType))
                return GetIndexOfElementsMoreOrLessXDependingOnPermutation_(Tree->left_, x, permutationType);
           
            return GetNumber_(Tree->left_) + 1;
        }

        static long long GetKeyByIndex_(PNode Tree, long long k) {
            if (Tree == nullptr)
                return 0;
            if (GetNumber_(Tree->left_) + 1 == k)
                return Tree->key_;
            if (GetNumber_(Tree->left_) >= k)
                return GetKeyByIndex_(Tree->left_, k);
            else
                return GetKeyByIndex_(Tree->right_, k - GetNumber_(Tree->left_) - 1);
        }

        static void DoPermutation_(PNode& Tree, PermutationType permutationType) {
            if (IsOrdered_(Tree, permutationType)) {
                Reverse_(Tree);
            }
            else {
                size_t n1 = GetNumberOfElementsBeforeStrictlyOrderedSuffix_(Tree, permutationType);
                TwoNodes tmp3 = Split(Tree, n1);
                long long x1 = tmp3.first->last_;
                if (permutationType == Next)
                    Reverse_(tmp3.second);
                size_t n2 = GetIndexOfElementsMoreOrLessXDependingOnPermutation_(tmp3.second, x1, permutationType);
                long long x2 = GetKeyByIndex_(tmp3.second, n2);
                AssignValue(tmp3.first, n1, n1, x2);
                AssignValue(tmp3.second, n2, n2, x1);
                if (permutationType == Prev)
                    Reverse_(tmp3.second);
                Tree = Merge(tmp3.first, tmp3.second);
            }
        }
    };

    typedef Node* PNode;
    typedef std::pair < PNode, PNode > TwoNodes;

    PNode Tree_;
public:
    CartesianTree()
            : Tree_(nullptr) {

    }

    explicit CartesianTree(std::vector < long long > a) {
        Tree_ = new Node(a[0]);
        for (long long i = 1; i < a.size(); ++i) {
            PNode v = new Node(a[i]);
            Tree_ = Node::Merge(Tree_, v);
        }
    }

    CartesianTree(const CartesianTree& other) {
        PNode cur = other.Tree_;
    }

    CartesianTree(CartesianTree&& other) {
        Tree_ = other.Tree_;
        other.Tree_ = nullptr;
    }

    ~CartesianTree()
    {
        Node::Delete(Tree_);
        Tree_ = nullptr;
    }

    CartesianTree& operator=(const CartesianTree& other) {
        Node::Delete(Tree_);
        Node::Copy(other.Tree_, Tree_);
        return *this;
    }

    CartesianTree& operator=(CartesianTree&& other) {
        Node::Delete(Tree_);
        Node::Copy(other.Tree_, Tree_);
        other.Tree_ = nullptr;
        return *this;
    }

    void Insert(long long x, size_t k) {
        ++k;
        PNode v = new Node(x);
        if (Tree_ == nullptr) {
            Tree_ = v;
        }
        else {
            TwoNodes tmp1 = Node::Split(Tree_, k - 1);
            Tree_ = Node::Merge(tmp1.first, Node::Merge(v, tmp1.second));
        }
    }

    long long Sum(size_t l, size_t r) {
        return Tree_ ? Node::GetSum(Tree_, l + 1, r + 1) : 0;
    }

    void Erase(size_t x) {
        Node::Erase(Tree_, x + 1);
    }

    void Assign(size_t l, size_t r, long long x) {
        Node::AssignValue(Tree_, l + 1, r + 1, x);
    }

    void Add(size_t l, size_t r, long long x) {
        Node::AddValue(Tree_, l + 1, r + 1, x);
    }

    void next_permutation(size_t l, size_t r) {
        Node::NextPermutation(Tree_, l + 1, r + 1);
    }

    void prev_permutation(size_t l, size_t r) {
        Node::PrevPermutation(Tree_, l + 1, r + 1);
    }

    void Print(std::vector < long long > &out) {
        Tree_->Print(out);
    }
};

class Query
{
public:
    enum OperationType {
        Sum = 1,
        Insert = 2,
        Erase = 3,
        Assign = 4,
        Add = 5,
        Next_p = 6,
        Prev_p = 7
    };
    virtual Query* copy() = 0;
    virtual const OperationType GetType() = 0;
    virtual void Read(std::istream &stream) = 0;
    virtual ~Query() {}
};

class SegmentSimpleQuery
    : public Query
{
public:
    size_t left, right;
    OperationType operationType;

    virtual SegmentSimpleQuery* copy() {
        SegmentSimpleQuery* queryPtr = new SegmentSimpleQuery(*this);
        return queryPtr;
    }

    explicit SegmentSimpleQuery(OperationType operation)
            : operationType(operation) {
    
    }

    OperationType const GetType() {
        return operationType;
    }

    void Read(std::istream &in) {
        in >> left >> right;
    }
};

class ElementSimpleQuery
    : public Query
{
public:
    size_t position;
    OperationType operationType;

    virtual ElementSimpleQuery* copy() {
        ElementSimpleQuery* queryPtr = new ElementSimpleQuery(*this);
        return queryPtr;
    }

    explicit ElementSimpleQuery(OperationType operation) 
            : operationType(operation) {

    }

    OperationType const GetType() {
        return operationType;
    }

    void Read(std::istream &in) {
        in >> position;
    }
};

class SegmentChangeDataQuery
    : public SegmentSimpleQuery
{
public:
    long long value;

    virtual SegmentChangeDataQuery* copy() {
        SegmentChangeDataQuery* queryPtr = new SegmentChangeDataQuery(*this);
        return queryPtr;
    }

    explicit SegmentChangeDataQuery(OperationType operation) 
            : SegmentSimpleQuery(operation) {

    }

    void Read(std::istream &in) {
        in >> value;
        SegmentSimpleQuery::Read(in);
    }
};

class ElementChangeDataQuery
    : public ElementSimpleQuery
{
public:
    long long value;

    virtual ElementChangeDataQuery* copy() {
        ElementChangeDataQuery* queryPtr = new ElementChangeDataQuery(*this);
        return queryPtr;
    }

    explicit ElementChangeDataQuery(OperationType operation)
        : ElementSimpleQuery(operation) {

    }

    void Read(std::istream &in) {
        in >> value;
        ElementSimpleQuery::Read(in);
    }
};

struct Answers
{
    std::vector < size_t > queryNumbers;
    std::vector < long long > queryResults;
    std::vector < long long > finalState;
};

class QueryList {
public:
    std::vector < Query* > queries;
    std::vector < long long > firstState;

    QueryList() {

    }

    QueryList(const QueryList &other) 
            : firstState(other.firstState) {
        for (size_t i = 0; i < other.queries.size(); ++i)
            queries.push_back(other.queries[i]->copy());
    }

    ~QueryList() {
        for (size_t i = 0; i < queries.size(); ++i)
            delete queries[i];
    }

    void ReadQuerires(std::istream& in) {
        size_t q;
        in >> q;
        for (size_t i = 0; i < q; ++i)
            queries.push_back(ReadQuery(in));
    }

    Query* ReadQuery(std::istream& in)  {
        size_t numberType;
        in >> numberType;
        Query::OperationType operationType = static_cast < Query::OperationType > (numberType);
        Query* currentQuery = nullptr;
        if (operationType == Query::Sum)
            currentQuery = new SegmentSimpleQuery(operationType);
        if (operationType == Query::Insert)
            currentQuery = new ElementChangeDataQuery(operationType);
        if (operationType == Query::Erase)
            currentQuery = new ElementSimpleQuery(operationType);
        if (operationType == Query::Assign)
            currentQuery = new SegmentChangeDataQuery(operationType);
        if (operationType == Query::Add)
            currentQuery = new SegmentChangeDataQuery(operationType);
        if (operationType == Query::Next_p)
            currentQuery = new SegmentSimpleQuery(operationType);
        if (operationType == Query::Prev_p)
            currentQuery = new SegmentSimpleQuery(operationType);
        currentQuery->Read(in);
        return currentQuery;
    }

    void ReadArray(std::istream& in) {
        size_t n;
        long long cur;
        in >> n;
        firstState.resize(n);
        for (size_t i = 0; i < n; ++i) {
            in >> firstState[i];
        }
    }

    void RunQuery(CartesianTree& Tree, Query* query, Answers &answers, size_t index) {
        if (query->GetType() == Query::Sum) {
            long long ans = Tree.Sum(
                dynamic_cast <SegmentSimpleQuery*> (query)->left,
                dynamic_cast <SegmentSimpleQuery*> (query)->right
            );
            answers.queryResults.push_back(ans);
            answers.queryNumbers.push_back(index);
        }
        if (query->GetType() == Query::Insert)
            Tree.Insert(
                dynamic_cast <ElementChangeDataQuery*> (query)->value,
                dynamic_cast <ElementChangeDataQuery*> (query)->position
            );
        if (query->GetType() == Query::Erase)
            Tree.Erase(
                dynamic_cast <ElementSimpleQuery*> (query)->position
            );
        if (query->GetType() == Query::Assign)
            Tree.Assign(
                dynamic_cast <SegmentChangeDataQuery*> (query)->left,
                dynamic_cast <SegmentChangeDataQuery*> (query)->right,
                dynamic_cast <SegmentChangeDataQuery*> (query)->value
            );
        if (query->GetType() == Query::Add)
            Tree.Add(
                dynamic_cast <SegmentChangeDataQuery*> (query)->left,
                dynamic_cast <SegmentChangeDataQuery*> (query)->right,
                dynamic_cast <SegmentChangeDataQuery*> (query)->value
            );
        if (query->GetType() == Query::Next_p)
            Tree.next_permutation(
                dynamic_cast <SegmentSimpleQuery*> (query)->left,
                dynamic_cast <SegmentSimpleQuery*> (query)->right
            );
        if (query->GetType() == Query::Prev_p)
            Tree.prev_permutation(
                dynamic_cast <SegmentSimpleQuery*> (query)->left,
                dynamic_cast <SegmentSimpleQuery*> (query)->right
            );
    }
};

namespace ReadSolveWrite
{

    static QueryList ReadAll(std::istream& in) {
        QueryList data;
        data.ReadArray(in);
        data.ReadQuerires(in);
        return data;
    }

    template < typename TreeType >
    static Answers Solve(QueryList &data)
    {
        Answers answer;
        TreeType Tree(data.firstState);
        for (size_t i = 0; i < data.queries.size(); ++i)
            data.RunQuery(Tree, data.queries[i], answer, i);
        Tree.Print(answer.finalState);
        return answer;
    }

    template < typename T >
    void printAnswerFromVector(std::ostream& out, const std::vector < T > &v, char delimiter) {
        for (size_t i = 0; i < v.size(); ++i)
            out << v[i] << ((i + 1 != v.size()) ? delimiter : '\n');
    }

    void printAnswer(std::ostream& out, const Answers &answer)
    {
        printAnswerFromVector < long long >(out, answer.queryResults, '\n');
        printAnswerFromVector < long long >(out, answer.finalState, ' ');
    }
}

static void GlobalRun(std::istream& in, std::ostream& out)
{
    QueryList data = ReadSolveWrite::ReadAll(in);
    Answers answer = ReadSolveWrite::Solve<CartesianTree>(data);
    ReadSolveWrite::printAnswer(out, answer);
}

int main() {
    std::ifstream file;
    file.open("input.txt");
    GlobalRun(file, std::cout);
    return 0;
}