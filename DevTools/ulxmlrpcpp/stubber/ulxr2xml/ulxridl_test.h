/* This is a comment */

#ifndef ULXR_IDL_TEST_H
#define ULXR_IDL_TEST_H

#include <string>

#define ulxr_function
#define ulxr_constructor
#define ulxr_methods

#define mockpp_constructor
#define mockpp_methods
#define K_DCOP
#define Q_OBJECT
#define k_dcop public
#define k_dcop_hidden public
#define k_dcop_signals public
#define slots
#define signals

namespace ext {

class IdlInterface
{
    K_DCOP
  public:
    IdlInterface(const std::string &s)
    {}

    virtual ~IdlInterface()
    {}
};

}

namespace myname {

class UlxrIdlTest : public ext::IdlInterface
{
    Q_OBJECT

public ulxr_constructor:
    UlxrIdlTest();

public ulxr_constructor:
    UlxrIdlTest(int i, const std::string &s);

    UlxrIdlTest(int i, const std::string &s, long last);

public ulxr_methods:
    virtual ~UlxrIdlTest();

k_dcop:
k_dcop_signals:

private slots:
private signals:

public ulxr_methods:
    virtual void first_url4();
    virtual std::string * first_url4(int i);
    virtual std::string * first_url4(long li);
//ulxr_methods:
    virtual const char * first_url4(float li);
    virtual wchar_t * const first_url4(double li);

public ulxr_methods:
    virtual std::string constTest() const;
//ulxr_methods:
    virtual std::string & constRefTest() const;

public:
    virtual long* not_firstViewConst() const;
    virtual float* *not_firstView();
    virtual double* not_nextView();
    virtual bool* not_nextView2();

public ulxr_methods:
    virtual long* firstViewConst() const;
    virtual std::string* firstView();
// ulxr_methods:
    virtual std::wstring* nextView1(int i, long * l, std::string &s, const bool * b, char c) const;
    virtual std::basic_string<char>* nextView2(int i, long l, std::string &s, bool * const b, char c) const;

    void  getNumObjects(std::string s);
    const std::basic_string<wchar_t>* getObject(const std::string &s = std::string("iuuiu", 1));
    int   getNumPages(const std::wstring &s = L"asdf") throw(int, long);

    static const std::string * last_url4();
    virtual std::string * last_url4(int i);
    virtual std::string * last_url4(long li) throw(int, float);
};

}

namespace funcs {

ulxr_function
  void free_function1(int i);

ulxr_function
  long free_function1(long i, float f);

}

namespace inline_func {

//  inline free_function2(long i, float f);
ulxr_function
  inline std::string * free_function2(long * i, float f);


std::string * free_function2(long * i, float f)
{
  static std::string s;
  return &s;
}

typedef double Scalar;
inline static Scalar rad2deg( Scalar angle ) { return angle * (180./ 3.1415); }

}

#endif // ULXR_IDL_TEST_H



