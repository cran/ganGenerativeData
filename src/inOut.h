// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef IN_OUT
#define IN_OUT

#include <iostream>
#include <fstream>
#include <map>

using namespace std;

//const string cSeparator = "_";
//const string cPeriod = ".";

class InOut {
public:
	static void Write(ofstream& os, const bool& x) {
		os.write((const char *)&x, sizeof(x));
	}
	static 	void Read(ifstream& is, const bool& x) {
		is.read((char *)&x, sizeof(x));
	}

	static void Write(ofstream& os, const unsigned char& x) {
		os.write((const char *)&x, sizeof(x));
	}
	static void Read(ifstream& is, const unsigned char& x) {
		is.read((char *)&x, sizeof(x));
	}

	static void Write(ofstream& os, const int& x) {
		os.write((const char *)&x, sizeof(x));
	}
	static void Read(ifstream& is, const int& x) {
		is.read((char *)&x, sizeof(x));
	}

	static void Write(ofstream& os, const float& x) {
		os.write((const char *)&x, sizeof(x));
	}
	static void Read(ifstream& is, const float& x) {
		is.read((char *)&x, sizeof(x));
	}

	static void Write(ofstream& os, const char* p, size_t size) {
    if(size > 0) {
      os.write((const char *)p, sizeof(char) * size);
    }
  }
	static void Read(ifstream& is, const char* p, size_t size) {
    if(size > 0) {
      is.read((char *)p, sizeof(char) * size);
    }
  }

	static void Write(ofstream& os, const wchar_t* p, size_t size) {
		if(size > 0) {
			os.write((const char *)p, sizeof(wchar_t) * size);
		}
	}
	static void Read(ifstream& is, const wchar_t* p, size_t size) {
		if(size > 0) {
			is.read((char *)p, sizeof(wchar_t) * size);
		}
	}

	static void Write(ofstream& os, const string& x) {
    int size = x.size();
    Write(os, size);
    Write(os, x.c_str(), size);
  }
	static void Read(ifstream& is, string& x) {
    int size = 0;
    Read(is, size);
    x.resize(size);
    Read(is, x.c_str(), size);
  }

	static void Write(ofstream& os, const wstring& x) {
		int size = x.size();
		Write(os, size);
		Write(os, x.c_str(), size);
	}
	static void Read(ifstream& is, wstring& x) {
		int size = 0;
		Read(is, size);
		x.resize(size);
		Read(is, x.c_str(), size);
	}

	static void Write(ofstream& os, const vector<unsigned char>& x) {
		int size = x.size();
		Write(os, size);
		for(int i = 0; i < (int)x.size(); i++) {
			Write(os, x[i]);
		}
	}
	static void Read(ifstream& is, vector<unsigned char>& x) {
		int size = 0;
		Read(is, size);
		x.resize(size);
		for(int i = 0; i < (int)x.size(); i++) {
			Read(is, x[i]);
		}
	}

	static void Write(ofstream& os, const vector<int>& x) {
		int size = x.size();
		Write(os, size);
		for(int i = 0; i < (int)x.size(); i++) {
			Write(os, x[i]);
		}
	}
	static void Read(ifstream& is, vector<int>& x) {
		int size = 0;
		Read(is, size);
		x.resize(size);
		for(int i = 0; i < (int)x.size(); i++) {
			Read(is, x[i]);
		}
	}

	static void Write(ofstream& os, const vector<float>& x) {
		int size = x.size();
		Write(os, size);
		for(int i = 0; i < (int)x.size(); i++) {
			Write(os, x[i]);
		}
	}
	static void Read(ifstream& is, vector<float>& x) {
		int size = 0;
		Read(is, size);
		x.resize(size);
		for(int i = 0; i < (int)x.size(); i++) {
			Read(is, x[i]);
		}
	}

	static void Write(ofstream& os, const vector<wstring>& x) {
		int size = x.size();
		Write(os, size);
		for(int i = 0; i < (int)x.size(); i++) {
			Write(os, x[i]);
		}
	}
	static void Read(ifstream& is, vector<wstring>& x) {
		int size = 0;
		Read(is, size);
		x.resize(size);
		for(int i = 0; i < (int)x.size(); i++) {
			Read(is, x[i]);
		}
	}

	static void Write(ofstream& os, const map<wstring, int>& x) {
		int size = x.size();
		Write(os, size);
		map<wstring, int>::const_iterator iter(x.begin());
		while(iter != x.end()) {
			Write(os, iter->first);
			Write(os, iter->second);
			iter++;
		}
	}
	static void Read(ifstream& is, map<wstring, int>& x) {
		int size = x.size();
		Read(is, size);
		wstring k;
		for(int i = 0; i < (int)size; i++) {
			Read(is, k);
			Read(is, x[k]);
		}
	}

	static void Write(ofstream& os, const map<int, wstring>& x) {
		int size = x.size();
		Write(os, size);
		map<int, wstring>::const_iterator iter(x.begin());
		while(iter != x.end()) {
			Write(os, iter->first);
			Write(os, iter->second);
			iter++;
		}
	}
	static void Read(ifstream& is, map<int, wstring>& x) {
		int size = x.size();
		Read(is, size);
		int k = 0;
		for(int i = 0; i < (int)size; i++) {
			Read(is, k);
			Read(is, x[k]);
		}
	}
};

#endif
