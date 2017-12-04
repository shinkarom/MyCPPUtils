#pragma once
#pragma warning(disable:4018)

#include <tuple>
#include <string>
#include <random>
#include <stdexcept>
#include <sstream>
#include <map>

namespace Markov
{

	using namespace std;

	static mt19937 gen(random_device{}());
	static auto randnum = [](int From, int To) {uniform_int_distribution<int> dist(From, To); return dist(gen);};

	using MarkovSequence = vector<wstring>;
	using MarkovSubindex = map<MarkovSequence, int>;
	using MarkovIndex = map<MarkovSequence, MarkovSubindex>;
	enum class MarkovType { Characters, Words };

	constexpr int DefaultDepth = 2;
	constexpr MarkovType DefaultType = MarkovType::Words;

	class MarkovChain
	{
	public:
		class insufficient_index_error : public runtime_error {
		public:
			insufficient_index_error() : runtime_error("insufficient index") {};
		};
		void ParseItem(const wstring& item, int depth = DefaultDepth);
		wstring Generate(int depth = DefaultDepth);
		MarkovType GetType();
		void SetType(const MarkovType& item);
	private:
		MarkovIndex index;
		MarkovType Type{ DefaultType };
		MarkovSubindex get_subindex(const MarkovSequence & Bef);
		MarkovSequence get_aft(const MarkovSequence & Bef);
		MarkovSequence GenerateSequence(int depth = DefaultDepth);
		MarkovSequence split(const wstring& item);
		wstring assemble(const MarkovSequence& item);
		MarkovSequence slice(const MarkovSequence& item, int b, int e);
		MarkovSequence slice_till_end(const MarkovSequence& item, int b);
		wstring DebugSequence(const MarkovSequence& item);
	};

	//

	inline void MarkovChain::ParseItem(const wstring& item, int depth)
	{
		auto seq = split(item);
		auto len = seq.size();
		MarkovSequence bef, aft;
		if (len == 0) return;
		else
			if (len < depth) depth = len;
		//
		aft = slice(seq, 0, 1);
		index[MarkovSequence{ L"" }][aft]++;
		//
		for (int dep = depth; dep >= 1; dep--)
		{
			int lim = len - dep - 1;
			for (int pos = 0; pos <= lim; pos++)
			{
				bef = slice(seq, pos, dep);
				aft = slice(seq, pos + dep, 1);
				index[bef][aft]++;
			}
			bef = slice(seq, len - dep, 1);
			index[bef][MarkovSequence{ L"" }]++;
		}
	}

	inline wstring MarkovChain::Generate(int depth)
	{
		auto res = GenerateSequence(depth);
		return assemble(res);
	}

	inline MarkovType MarkovChain::GetType()
	{
		return Type;
	}

	inline void MarkovChain::SetType(const MarkovType& item)
	{
		if (item != Type) {
			index.clear();
			Type = item;
		}
	}

	inline MarkovSubindex MarkovChain::get_subindex(const MarkovSequence & Bef)
	{
		MarkovSubindex i;
		if (Bef.size() > 0)
		{
			MarkovSequence b = Bef;
			while (index.count(b) == 0)
			{
				if (b.size() > 1)
					b = slice_till_end(b, 1);
				else
				{
					wcout << "Checking \"" << assemble(Bef) << "\"";
					throw insufficient_index_error();
				}
			}
			i = index[b];
		}
		else
		{
			i = index[Bef];
			if (i.empty()) throw insufficient_index_error();
		}
		return i;
	}

	inline MarkovSequence MarkovChain::get_aft(const MarkovSequence & Bef)
	{
		MarkovSubindex i = get_subindex(Bef);
		auto sum = 0;
		for (auto it : i)sum += it.second;
		auto r = randnum(1, sum);
		auto it = i.cbegin();
		while (r > 0)
		{
			r -= it->second;
			++it;
		}
		--it;
		return it->first;
	}

	inline MarkovSequence MarkovChain::GenerateSequence(int depth)
	{
		MarkovSequence result;
		auto h = get_aft(MarkovSequence{ L"" });
		result.insert(end(result), begin(h), end(h));
		MarkovSequence aft;
		do
		{
			MarkovSequence j;
			if (result.size() > depth)
				j = slice(result, result.size() - depth, depth);
			else
				j = slice(result, 0, depth);
			aft = get_aft(j);
			result.insert(end(result), begin(aft), end(aft));
		} while (aft != MarkovSequence{ L"" });
		result.pop_back();
		return result;
	}

	inline MarkovSequence MarkovChain::split(const wstring & item)
	{
		MarkovSequence result;
		wistringstream ss(item);
		wstring s;
		switch (Type)
		{
		case MarkovType::Characters:
			for (auto& x : item)
				result.push_back(wstring{ x });
			break;
		case MarkovType::Words:
			while (ss >> s)
				result.push_back(s);
			break;
		default:
			break;
		}
		return result;
	}

	inline wstring MarkovChain::assemble(const MarkovSequence & item)
	{
		wstring result;
		switch (Type)
		{
		case MarkovType::Characters:
			for (auto& x : item)
				result += x;
			break;
		case MarkovType::Words:
			for (auto& x : item)
				result += x + L" ";
			result.erase(result.length() - 1);
			break;
		default:
			break;
		}
		return result;
	}

	inline MarkovSequence MarkovChain::slice(const MarkovSequence & item, int a, int b)
	{
		if (a == item.size())
		{
			return MarkovSequence{ L"" };
		}
		else if (a+b>=item.size())
		{
			b = item.size() - a;
		}
		auto beg = begin(item) + a;
		auto en = beg + b;
		return MarkovSequence(beg, en);
	}

	inline MarkovSequence MarkovChain::slice_till_end(const MarkovSequence & item, int b)
	{
		return slice(item, b, item.size() - b);
	}

	inline wstring MarkovChain::DebugSequence(const MarkovSequence & item)
	{
		wstring result;
		for (auto& i : item)
		{
			result += L"\"" + i + L"\" ";
		}
		return result;
	}

}
