#ifndef NO_COPY_HH
#define NO_COPY_HH

//对象语义，禁止复制和赋值
class NoCopy
{
protected:
    NoCopy() = default;
    ~NoCopy() = default;

    NoCopy(const NoCopy &rhs) = delete;
    NoCopy &operator=(const NoCopy &rhs) = delete;
};

#endif