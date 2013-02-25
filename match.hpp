#pragma once
#include<vector>
#include<string>

class match
{
public:
    int start;
    int pattern;
    match(int s,int p):
        start(s),
        pattern(p)
    {}
    bool operator==(const match& o) const
    {
        return o.start == start &&
               o.pattern == pattern;
    }
    bool operator<(const match& o) const {
        if(o.start==start)return pattern<o.pattern;
        return start<o.start;
    }
};

typedef std::vector<match> match_vector;

class multiple_matcher
{
public:
    virtual void match(const std::string& s,
                       const std::vector<std::string>& patterns,
                       match_vector& out) const = 0;
    virtual std::string name() const = 0;
    virtual ~multiple_matcher()
    {
    }
    mutable double pp_time;
    mutable double match_time;
};

