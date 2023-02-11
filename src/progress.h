// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef PROGRESS
#define PROGRESS

#define GD_RCPP

#ifdef GD_RCPP
#include <Rcpp.h>
#endif

using namespace std;

class Progress {
public:
    Progress(int size): _lastPercent(-1), _size(size), _modulo(500), _beginCount(0) {
#ifdef GD_RCPP
        Function f("message");
        operator()(0);
#endif
    }
    void operator()(int n) {
#ifdef GD_RCPP
        if(n == 0 && _lastPercent == -1) {
            Function f("message");
            f("0%");
            _lastPercent = 0;
            return;
        }
        if(n == _size) {
            Function f("message");
            if(_lastPercent != 100) {
                f("100%");
                _lastPercent = 100;
            }
            return;
        }
        if(n == 0 && _beginCount % _modulo == 0) {
            Rcpp::checkUserInterrupt();
            _beginCount++;
            return;
        }
        
        if(n % _modulo == 0) {
            int percent = floor((float)n / (float)_size * 100.0f);
            if(percent != _lastPercent) {
                Function f("message");
                stringstream ss;
                ss << percent << "%";
                f(ss.str());
                
                _lastPercent = percent;
            }
            Rcpp::checkUserInterrupt();
        }
#endif
    }

private:
    int _lastPercent;
    int _size;
    int _modulo;
    int _beginCount;
};

#endif
