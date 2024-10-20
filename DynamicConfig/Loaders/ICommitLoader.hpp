#ifndef MY_COMMIT_LOADER_HPP
#define MY_COMMIT_LOADER_HPP

class ICommitLoader
{
private:
    
public:
    ICommitLoader() {}

    virtual bool commit() = 0;

    ~ICommitLoader() {}
};

#endif