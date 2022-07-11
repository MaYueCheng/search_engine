#ifndef CANDIDATE_RESULT_HH
#define CANDIDATE_RESULT_HH

#include "../cpp.hh"

using std::string;

struct CandidateResult
{
    string word;
    int frequency;
    int distance;
};

#endif