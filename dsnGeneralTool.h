#pragma once
//#include "dpu_Math.h"
#include "stdafx.h"
#include "afxdialogex.h"
#include "afxcmn.h"
#include <string>
#include <codecvt> 
#include "sqlite/sqlite3.h"
#include <memory>
#include <map>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include<sstream>
#include "windows.h"
#include "imagehlp.h"
#include <regex>
#include <cstdio>
#include <sys/stat.h>
#include <thread>
#include "MiniZip/zconf.h"
#include "MiniZip/zip.h"
#include "MiniZip/zlib.h"
#include "MiniZip/ioapi.h"
#pragma comment(lib,"imagehlp.lib")  

#define PRECISION6 1E-6//容差
#define PRECISION9 1E-9//容差
#define IF02RETURN(condition) if(!condition) return
#define _CRT_SECURE_NO_WARNINGS
#define mapKeyValue(key,value) {key,value}
#define mapFindKey(key,_map) {auto it=_map.find(key); return it!=_map.end()?it->second:"未知类型";}
///=================================================================================================
///根据类型生成智能指针
#undef  genSmartPtr
#define genSmartPtr(mType,mPtr)\
protected:\
  mType m_p##mPtr = {};\
public:\
   void set##mPtr(mType const &val){m_p##mPtr=val;}\
   mType get##mPtr(){return m_p##mPtr;}

#define VecFind2True(_contain,_value) find(_contain.begin(),_contain.end(),_value)!=_contain.end()

///=================================================================================================
///报告模板
#undef  REPORTTEMPLATE
#define REPORTTEMPLATE(DialogType,ReportType,DataBaseType,ReportName)\
std::shared_ptr<DataBaseType> pDataBase = std::make_shared<DataBaseType>();\
std::shared_ptr<DialogType> dlg = std::make_shared<DialogType>(pDataBase);\
AFX_MANAGE_STATE(AfxGetStaticModuleState());\
if (IDOK != dlg->DoModal()) { cmdPrint(_T("取消生成报告！")); return; }\
std::shared_ptr<ReportType> pReport = std::make_shared<ReportType>(pDataBase);\
pReport->setLanguageType(dpuApp::E_LT_CHS);\
pReport->setReportName(ReportName);\
pReport->SetTranslationName(STR2CSTR("EN_"#ReportName));\
pReport->setWordTemplate(CSTR2STR(getWordTemplatePath(E_PT_A3, STR2CSTR(g_strReportTemlateNameBase))));\
pReport->writeReport() == REPORT_OK ? cmdPrint(_T("生成完毕！")) : cmdPrint(_T("生成失败！"));


namespace dsnGeneralTool
{
  struct dsnCompare_Tool  //自定义比较规则
  {
    //函数调用
    //dsnCompare_Tool dsnCpmpare;
    //int nnn=dsnCompare(2);
    //第一步调用fr_t()，第二部调用func(a)
    using fr_t = int(*)(int);
    static int func(int a) { return a; }
    operator fr_t() { return func; }
    //dsnCompare_Tool(10)会调用
    int operator()(int a) { return a; }


    //仿函数定义
    bool operator() (const CString& str1, const CString& str2) const
    {
      if (str1.GetLength() < str2.GetLength()) { return true; }
      else if (str1.GetLength() > str2.GetLength()) { return false; }
      else { return str1 < str2; }
    }
    // 使用 SFINAE 限定 T1 和 T2 为 double, float, int 类型
    template<typename U1, typename U2>
    typename std::enable_if<
      (std::is_same<U1, double>::value || std::is_same<U1, float>::value || std::is_same<U1, int>::value) &&
      (std::is_same<U2, double>::value || std::is_same<U2, float>::value || std::is_same<U2, int>::value),bool>::type
    operator() (const U1& n1, const U2& n2) const{return n1 < n2;}
  };

  ///=================================================================================================
  ///数学判断
#pragma region 数学工具类
  class dsnMath_Tool
  {
  public:
    dsnMath_Tool() = delete;
    ~dsnMath_Tool() = delete;
  public:
    ///=================================================================================================
    /// double,float类型
    ///精度1e-10，T1<=T2
    template<class T_1, class T_2> static inline bool LE(const T_1 T1, const T_2 T2) { return ((double)T2 - T1 > 1e-10) || (fabs((double)T1 - T2) <= 1e-10); }
    template<class T_1, class T_2> static inline bool LE(const T_1 T1, const T_2 T2, double dPre) { return ((double)T2 - T1 > dPre) || (fabs((double)T1 - T2) <= dPre); }
    ///精度1e-10，T1<T2
    template<class T_1, class T_2> static inline bool LS(const T_1 T1, const T_2 T2) { return (double)T2 - T1 > 1e-10; }
    template<class T_1, class T_2> static inline bool LS(const T_1 T1, const T_2 T2, double dPre) { return T2 - T1 > dPre; }
    ///精度1e-10，T1>=T2
    template<class T_1, class T_2> static inline bool ME(const T_1& T1, const T_2& T2) { return ((double)T1 - T2 > 1e-10) || (fabs((double)T1 - T2) <= 1e-10); }
    template<class T_1, class T_2> static inline bool ME(const T_1 T1, const T_2 T2, double dPre) { return ((double)T1 - T2 > dPre) || (fabs((double)T1 - T2) <= dPre); }
    ///精度1e-10，T1>T2
    template<class T_1, class T_2> static inline bool MO(const T_1 T1, const T_2 T2) { return (double)T1 - T2 > 1e-10; }
    template<class T_1, class T_2> static inline bool MO(const T_1 T1, const T_2 T2, double dPre) { return (double)T1 - T2 > dPre; }
    ///精度1e-10，T_2处于T1~T3之间
    template<class T_1, class T_2, class T_3> static inline bool BT(const T_1 T1, const T_2 T2, const T_3 T3){return (ME(T1, T2) && ME(T2, T3)) || (ME(T3, T2) && ME(T2, T1));}
    template<class T_1, class T_2, class T_3> static inline bool BT(const T_1 T1, const T_2 T2, const T_3 T3, double dPre){ return (ME(T1, T2, dPre) && ME(T2, T3, dPre)) || (ME(T3, T2, dPre) && ME(T2, T1, dPre));}
    //接近哪一端，0接近T1，1-接近T2
    template<class T_1, class T_2, class T_3> static inline int BTBeside(const T_1 T1, const T_2 T2, const T_3 T3)
    {
      if (!BT(T1, T2, T3)) return -1;
      if (LE(T1, T3) && ME((T1 + T3) / 2, T2)) return 0;
      else if (ME(T1, T3) && LE((T1 + T3) / 2, T2)) return 0;
      return 1;
    }

    ///精度1e-10，T1==0
    template<class T_1> static inline bool EQ(const T_1 T1) { return fabs((double)T1) <= 1e-10; }
    //精度1e-10 T1==T2
    template<class T_1, class T_2> static inline bool EQ(const T_1 T1, const T_2 T2) { return fabs((double)T1 - T2) <= 1e-10; }
    template<class T_1, class T_2> static inline bool EQ(const T_1 T1, const T_2 T2, double dPre) { return fabs((double)T1 - T2) <= dPre; }
    //保留位数,四舍五入
    template<class T_1> static inline double Round(const T_1& T1, const int _n) { double _d = std::pow(10, _n); return (int)((T1 + 0.5 / _d) * _d) * 1.0 / _d; }
    //不保留精度和四舍五入，精度外将被截断
    template<class T_1> static inline double NoRound(const T_1& T1, const int _n) { double _d = std::pow(10, _n); return (int)(T1* _d) * 1.0 / _d; }
    //线性插值
    template<class T_1, class T_2> static inline double LineInteValue(const std::pair<T_1, T_2> &_pair1, const std::pair<T_1, T_2> &_pair2, T_1 key)
    {
      return _pair1.second + (_pair2.second - _pair1.second) / (_pair2.first - _pair1.first) * (key - _pair1.first);
    }
    //科学计数法表示,返回位数，目前仅支持十进制,保留一位整数_d,输出位数10或者-1的次方n
    template<class T_1> static inline int SciNote(const T_1& T1,double&_d)
    {
      if (LE(fabs(T1), 1.0 )) { return SciNote_01(T1, _d); }//绝对值小于1.0
      if (ME(fabs(T1), 10.0)) { return SciNote_10(T1, _d); }//绝对值大于等于10
      if (ME(fabs(T1), 1.0 )&& LS(fabs(T1), 10.0)) { return SciNote_1 (T1, _d); }//绝对值为大于等于1小于等于10
      return 0;
    }
  private:
    template<class T_1> static inline int SciNote_1(const T_1& T1, double& _d) {  _d = T1;return 0;}// 绝对值为等于1
    //绝对值为大于等于10
    template<class T_1> static inline int SciNote_10(const T_1& T1, double& _d)
    { 
      int _nNum{};
      while (true)
      {
        double _dCount = T1 * std::pow(10, _nNum);
        if (LE(1.0, fabs(_dCount)))
        {
          _d = _dCount;
          return -_nNum;
          break;
        }
        _nNum++;
      }
      return 0;
    }
    //绝对值为大于等于1小于等于10
    template<class T_1> static inline int SciNote_01(const T_1& T1, double& _d)//绝对值为大于等于1小于等于10
    { 
      int _nNum{};
      while (true)
      {
        double _dCount = T1 * std::pow(10, _nNum);
        if (LE(1.0, fabs(_dCount)))
        {
          _d = _dCount;
          return -_nNum;
          break;
        }
        _nNum++;
      }
      return 0;
    }//绝对值为大于等于10


  public:
    ///=================================================================================================
    ///平方、对数等
    template<class T_1, class T_2, class T_3> static inline const T_1 Exp(T_2 T2, T_3 T3) { return EQ(T2) ? 0 : pow(T2, T3); } // 以T2为底
    template<class T_1, class T_2, class T_3> static inline const T_1 Log(T_2 T2, T_3 T3) { return T3 = > 1e-10 && EQ(T3, 1)(T3 != 1) ? 0 : log(T2) / log(T3); }//以T3为底

    ///=================================================================================================
    ///除，T1/T2,若T2为0，结果输出为0
    template<class T_1, class T_2> static inline const double Div(T_1 T1, T_2 T2) { return !EQ(T2) ? (double)T1 / T2 : 0.0; }

    ///=================================================================================================
    ///int、double、float转化为bool
    template<class T>static inline bool Num2B(const T T1) { return EQ(T1) ? false : true; };
    template<class T>static inline int B2I(const T T1) { return T1 ? 1 : 0; };

    ///=================================================================================================
    ///一维数组1，赋值给数组2
    template<class T> static inline void Arr1To2(T arr1[], T arr2[]){memcpy(arr2, arr1, sizeof(arr1));}

    ///=================================================================================================
    ///一维数组初始化
    template<typename T1, typename T2>
    typename std::enable_if<!std::is_convertible<T1, T2>::value, void>::type
      static inline initializeArray(T1 _arr[], const int& _count, const T2& _iniValue)
    {
      for (auto i = 0; i < _count; i++)
      {
        _arr[i] = static_cast<T1>(_iniValue);
      }
    }
    template<typename T1, typename T2>
    typename std::enable_if<std::is_convertible<T1, T2>::value, void>::type
      static inline initializeArray(T1 _arr[], const int& _count, const T2& _iniValue)
    {
      for (auto i = 0; i < _count; i++)
      {
        _arr[i] = _iniValue;
      }
    }

    ///=================================================================================================
    ///BOOL->bool
    static const bool BOOL2bool(const BOOL& _b) { return _b ? true : false; }

    ///=================================================================================================
    ///求最大值
    template<typename T1, typename...T2>static const T1 maxValueCal(const T1& t1, const T2&...t2) { return max(t1, maxValueCal(t2...)); }
    template<typename T>static T maxValueCal(const T& t) {return t;}
    ///=================================================================================================
    ///类型转化
    template<typename T>
    static constexpr T TransConstexprValue(const T& t)
    {
      return t;
    }

    
    template<typename T>
    static auto TransConstexpr(const T& t) -> 
      typename std::enable_if<std::is_same<T, double>::value ||std::is_same<T, float>::value ||std::is_same<T, int>::value, T>::type
    {
      return TransConstexprValue(t); // 返回传入的参数值  
    }

  };
#pragma endregion

#pragma region 字符串处理
  class dsnCh_Tool
  {
  public:
    dsnCh_Tool() = delete;
    ~dsnCh_Tool() = delete;

  public:
#pragma region 数据转字符串
    //兼容float、double,若为0，则输出为“/”
    ///=================================================================================================
    ///将数据类型转化为CString
    template<class T_1> static inline CString DF2Str(const T_1 T1, const int nPrecision)
    {
      CString strTemp;
      strTemp.Format(TEXT("%.*f"), nPrecision, T1);
      return dsnMath_Tool::EQ(T1) ? TEXT("/") : 
        strTemp;
    }
    template<class T_1> static inline CString DF2Str_0(const T_1 T1, const int nPrecision){CString strTemp;strTemp.Format(TEXT("%.*f"), nPrecision, T1);return dsnMath_Tool::EQ(T1) ? TEXT("0.00") : strTemp;}
    static inline CString Int2Str(const int T1)
    {
      CString strTemp;strTemp.Format(TEXT("%d"), T1);
      return dsnMath_Tool::EQ(T1) ? TEXT("/") : strTemp ;
    }
    static inline CString Int2Str_(const int T1)
    {
      CString strTemp; strTemp.Format(TEXT("%d"), T1);
      return strTemp;
    }
    static inline CString Bool2Str(const bool T1){return T1 ? TEXT("是") : TEXT("否");}
    //数字转字符串，舍弃后面多余的0
    template<typename T> static std::string Num2String(const T& t){std::ostringstream oss; oss << t;return oss.str();}
    //省略数字后面的0，否则输出
    enum class IF0Output
    {
      IO_0        ,//输出“0”
      IO_Blank    ,//输出“”
      I0_Slash    ,//输出“/”
      IO_Dash     ,//输出“-”
      IO_DoublDash,//输出“--”
    };
    template<class T_1> static inline std::string Number2Str(const T_1& T1,const IF0Output&_n)
    {
      std::map<IF0Output, std:string> _map
      {
        {IF0Output::IO_0        ,"0"},
        {IF0Output::IO_Blank    ," "},
        {IF0Output::I0_Slash    ,"/"},
        {IF0Output::IO_Dash     ,"-"},
        {IF0Output::IO_DoublDash,"--"},
      };
      std::string _strSs;
      std::stringstream stream;
      stream << T1;
      stream >> _strSs;
      return  EQ(T1) ? _map[_n] : _strSs;
    }

    ///=================================================================================================
    ///将字符串转化为数据类型
    static inline int    Str2Int   (const std::string& str) {return std::atoi(str.c_str());}
    static inline int    Str2Int   (const CString    & str) {return std::atoi(CString2Char(str));}
    static inline double Str2Double(const std::string& str) {return std::stod(str.c_str());}
    static inline double Str2Double(const CString    & str) {return Str2Double(CString2String(str));}
    static inline float  Str2Float (const std::string& str) {return std::stof(str.c_str());}
    static inline float  Str2Float (const CString    & str) {return std::stof(CString2String(str));}
    ///=================================================================================================
    ///将字符指针转化为数据类型
    static inline int    Char2Int   (const char* c) { return std::atoi(c)                ; }
    static inline int    Char2Int   (const char  c) {return  static_cast<int   >(c - '0'); }
    static inline double Char2Double(const char* c) { return std::stod(c)                ; }
    static inline double Char2Double(const char  c) { return static_cast<double>(c - '0'); }
    static inline float  Char2Float (const char* c) { return std::stof(c)                ; }
    static inline float  Char2Float (const char  c) { return static_cast<float >(c - '0'); }
    ///=================================================================================================
    ///CString与string封装库，可以使用该类型与CString与string的互转
    struct MergingStr
    {//多类型字符串，兼容std::string、CString
      MergingStr() = default;
      MergingStr(const CString    & str) : m_nType(1), m_Str_MFC(str) {}
      MergingStr(const std::string& str) : m_nType(2), m_Str_STD(str) {}
      MergingStr(const wchar_t    * ch ) : m_nType(1), m_Str_MFC(ch ) {}
      MergingStr(const char       * ch ) : m_nType(2), m_Str_STD(ch ) {}
      int         m_nType = 0;
      CString     m_Str_MFC;//m_nType=1
      std::string m_Str_STD;//m_nType=2
      operator CString     () 
      { 
        switch (m_nType)
        {
        case 1: return m_Str_MFC; break;
        case 2: return string2CString(m_Str_STD); break;
        }
        return L"";
      }
      operator std::string () 
      { 
        switch (m_nType)
        {
        case 1: return CString2String(m_Str_MFC); break;
        case 2: return m_Str_STD; break;
        }
        return "";
      }

      MergingStr operator+(const MergingStr& str)
      {
        //std::string _str = str;
        //MergingStr str(std::string(*this) + str.m_Str_STD);
        //return str;
      }
      MergingStr operator+(const std::string& str)
      {
        std::string str1(*this);
        MergingStr res(str1 + str);
        return res;
      }
      MergingStr operator+(const CString& str)
      {
        CString str1(*this);
        MergingStr res(str1 + str);
        return res;
      }
      MergingStr operator+(const char* str)
      {
        std::string str1(*this);
        MergingStr res(str1 + str);
        return res;
      }
      MergingStr operator+(const wchar_t* str)
      {
        CString str1(*this);
        MergingStr res(str1 + str);
        return res;
      }
    };

    ///=================================================================================================
    ///数据类型转化为字符串
    template<typename T> static inline const std::string DT2Str(const T& T1) { return std::to_string(T1); };

  public:
    //string类为空时，加/
    static inline CString NoStr2Empty(std::string str){return str.empty() ? TEXT("/") : string2CString(str);}
    static inline CString NoStr2Empty(CString cstr)   {return cstr.IsEmpty() ? TEXT("/") : cstr;}

    //字符后加单符号,且删除最后一个字符，并在返回值退出
    static inline CString AddSign(std::vector<int> vec, const CString& strSign) {CString str; for (const auto& x_D : vec) { str += Int2Str(x_D) + strSign; }str.Delete(str.GetLength() - 1, 1); return str; }
    static inline CString AddSign(std::vector<CString> vec, const CString& strSign)
    {
      CString str;
      for (const auto& x_D : vec)
      {
        str += x_D + strSign; 
      }
      str.Delete(str.GetLength() - 1, 1);
      return str; 
    }

    //删除最后一个字符，若为空，输出cstrSign
    static inline const CString DelEndSign(const CString& cstr,const CString & cstrSign=L"--")
    {
      CString res(cstr);
      if (cstr.IsEmpty()) { res = cstrSign; }
      else {res.Delete(res.GetLength() - 1, 1);}
      return res;
    }

    //百分数,兼容int,double,float类型
    ///=================================================================================================
    ///将数据后添加百分号，若数据较小，可输出"\"
    template<class T_1, class T_2> static inline CString Percentage2Str(const T_1 T_Value, const T_2 T_Sum, const int nPrecision){return dsnMath_Tool::EQ(T_Value) ? DF2Str(T_Value * 1.00 * 100 / T_Sum, nPrecision) + TEXT("%") : TEXT("/");}
    template<class T_1> static inline CString Percentage2Str(const T_1 T_Value, const int nPrecision){return dsnMath_Tool::EQ(T_Value) ? DF2Str(T_Value, nPrecision) + TEXT("%") : TEXT("/");}
    template<class T_1, class T_2> static inline CString Div2Str(const T_1 T1, const T_2 T2,const int nPre) { return dsnCh::DF2Str(!EQ(T2) ? (double)T1 / T2 : 0.0, nPre); }
    //分母为0，输出0.00
    template<class T_1, class T_2> static inline CString Div2Str_AddSlash_0(const T_1 T1, const T_2 T2) { return dsnMath::EQ(T2)? TEXT("0.00"): TEXT("1/") + dsnCh::DF2Str((double)T1 / T2,0);}
    //分子处理0.25，转化为1/4，25处理成25/1
    template<class T_1> static inline std::string DenominatorDealStr(const T_1 T1, const int _nPre,const double _dCom= std::pow(10, 6))
    {
      if (dsnMath_Tool::EQ(T1)) return "1/0";
      else if (dsnMath_Tool::LE(fabs(T1),1))
      {
        if (!dsnMath_Tool::ME(fabs(1 / T1), _dCom))
        {
          return  "1/" + CString2String(DF2Str(1 / T1, _nPre));
        }
        else
        {
          return "1/"+ CString2String(DF2Str(_dCom, _nPre));
        }
      }
      else {return CString2String(DF2Str(T1, _nPre))+ "/1";}
      return {};
    }


#pragma endregion

#pragma region 字符串与字符
    ///=================================================================================================
    ///字符const char*<-->CString
    static inline CString Char2CString(const char* ch) { return string2CString(ch); }
    static inline const char* CString2Char(const CString cstr) { return CString2String(cstr).c_str();}

    //string<-->char相互转化
    static inline const char* String2Char(const std::string _string) {return _string.c_str();}
    static inline std::string Char2String(const char* _char) {return _char;}

    //string<-->CString相互转化
    static inline CString string2CString(const std::string str) { CString cstr(str.c_str()); return cstr; }
    //CT2A是CW2A将Unicode转换为多字符集ASCII，两者都可以使用
    static inline std::string CString2String(const CString cstr) { std::string str(CW2A(cstr.GetString())); return str; }

    //wstring<-->wchar*的转化
    static inline const wchar_t* Wstring2Wchar(const std::wstring _wstring){return _wstring.c_str();}
    static inline const std::wstring Wchar2Wstring(const wchar_t* _wchar){return _wchar;}

    //wstring<-->string的转化
    static inline std::string wstring2string(const std::wstring ws) { _bstr_t t = ws.c_str(); char* pchar = (char*)t; std::string result = pchar; return result; }
    static inline std::wstring string2wstring(const std::string s) { _bstr_t t = s.c_str(); wchar_t* pwchar = (wchar_t*)t; std::wstring result = pwchar; return result; }

    //wchar<-->char
    static inline wchar_t* char2wchar(const char* pChar) {USES_CONVERSION;return A2W(pChar) ;}
    static inline char* wchar2char(const wchar_t* pWchar){USES_CONVERSION;return W2A(pWchar);}

    ///=================================================================================================
    ///适配器，unicode和ANSI两种编码方式
    //TCHAR<-->char
    static inline char* Tchar2char(const TCHAR* tchar){return CT2CA(tchar);}
    static inline TCHAR* Char2Tchar(const char* _char){return CA2CT(_char);}

    //TCHAR<-->string
    static inline std::string Tchar2Str(const TCHAR* tchar) { return Char2String(CT2CA(tchar)); }
    static inline TCHAR* Str2Tchar(const std::string str) { return CA2CT(String2Char(str)); }

    //TCHAR<-->CString
    static inline CString Tchar2CStr(const TCHAR* tchar) { return Char2CString(CT2CA(tchar)); }
    static inline TCHAR* Str2Tchar(const CString str) { return CA2CT(CString2Char(str)); }


    ///=================================================================================================
    ///从数据库读取数据时采用
    static inline const char* U2G(const char* utf8)
    {
      int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
      wchar_t* wstr = new wchar_t[len + 1];
      memset(wstr, 0, len + 1);
      MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
      len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
      char* str = new char[len + 1];
      memset(str, 0, len + 1);
      WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
      if (wstr) delete[] wstr;
      return str;
    }


    ///=================================================================================================
    ///中文写入数据库时采用
    static inline const char* G2U(const char* gb2312)
    {
      int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
      wchar_t* wstr = new wchar_t[len + 1];
      memset(wstr, 0, len + 1);
      MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
      len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
      char* str = new char[len + 1];
      memset(str, 0, len + 1);
      WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
      if (wstr) delete[] wstr;
      return str;
    }

    ///=================================================================================================
    ///UNICODE、UTF8、ANSI转换
    //UTF8<-->UNICODE
    static std::string Unicode2UTF8(const std::wstring& wstr)
    {
      //return Tchar2char(Wstring2Wchar(wstr));
      std::string ret;
      try { std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;ret = wcv.to_bytes(wstr);}
      catch (const std::exception& e) {std::cerr << e.what() << std::endl;}
      return ret;
    }
    static std::wstring UTF82Unicode(const std::string& str)
    {
      //return Char2Tchar(String2Char(str));
      std::wstring ret;
      try {std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;ret = wcv.from_bytes(str);}
      catch (const std::exception& e) {std::cerr << e.what() << std::endl;}
      return ret;
    }



    //UTF8<-->ANSI
//     static std::string UTF82ANSI(const std::string& str) {return Unicode2ANSI(UTF82Unicode(str));}
//     static std::string ANSIToUTF8(const std::string& str){return Unicode2UTF8(ANSI2Unicode(str));}

    //容器、字符、编码方式对应关系
    //string->char->单字符，utf8，ansi
    //wstring->wchar->宽字符，uincode
    //CString->tchar->适配字符，根据编码方式进行自动匹配
    ///=================================================================================================
    ///string新数据替换旧数据
    static inline const std::string Replace_all(std::string& src, const std::string& old_value, const std::string& new_value)
    {
      for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length())
      {
        if ((pos = src.find(old_value, pos)) != std::string::npos)
        {
          src.replace(pos, old_value.length(), new_value);
        }
        else break;
      }
      return src;
    }
    //删除所有
    static inline const std::string Erase_all(std::string& src, const std::string& old_value)
    {
      for (std::string::size_type pos(0); pos != std::string::npos; )
      {
        if ((pos = src.find(old_value, pos)) != std::string::npos)
        {
          src.erase(pos, old_value.length());
        }
        else break;
      }
      return src;
    }


#pragma endregion
  };
#pragma endregion


#pragma region 文件操作
  class dsnFile_Tool
  {
  public:
    dsnFile_Tool() = delete;
    ~dsnFile_Tool() = delete;
  public:
    //文件是否存在
    static inline bool bFileExist(const std::string& strFile)
    {
      return _taccess(dsnCh_Tool::string2CString(strFile), 0) != -1;
    }
    static inline bool bFileExist(const CString& strFile)
    {
      return bFileExist(dsnCh_Tool::CString2String(strFile));
    }
    static inline bool bFileFolderExist(const std::string& strFileFolder)
    {
      return PathIsDirectory(dsnCh_Tool::string2CString(strFileFolder));
    }
    //获取项目模型全路径
    static inline const std::string ModelCompletePath()
    {
      TCHAR szPath[MAX_PATH];
      GetModuleFileName(NULL, szPath, MAX_PATH);
      CString _cstr = szPath;
      _cstr += +TEXT("\\");
      return dsnCh_Tool::CString2String(_cstr);
    }

    //获取项目路径
    static inline const std::string ItemPath()
    {
      TCHAR pBuf[MAX_PATH];                     //存放路径的变量
      GetCurrentDirectory(MAX_PATH, pBuf);      //获取程序的当前目录
      CString _cstr = pBuf;
      _cstr += +TEXT("\\");
      return dsnCh_Tool::CString2String(_cstr);
    }

    //启动器文件路径.exe
    static inline std::string AppPath()
    {
      char szFilePath[MAX_PATH + 1] = { 0 };
      GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
      (strrchr(szFilePath, '\\'))[0] = 0; // 删除文件名，只获得路径字串//
      return dsnCh_Tool::Char2String(szFilePath)+"\\";
    }

    //文件不存在时，创造文件夹
    static inline void CreateFileFolder(const CString& strFileFolder)
    {
      if (!PathIsDirectory(strFileFolder))
      {
        CreateDirectory(strFileFolder, NULL);
      }
    }

    //判断路径是文件夹还是文件,根据文件后缀判断
    //0-文件,1-文件夹
    static  int IsFileFolder(const std::string& strPath)
    {
      struct stat info;
      if (stat(strPath.c_str(), &info) != 0) 
      {
        return false;
      }
      return (info.st_mode & S_IFDIR) != 0;
    }

    //判断两个文件夹内容是否一致
    static bool IsFileCloned(const std::string& strSourceFilePath, const std::string& strDestinationFilePath)
    {
      while (true)
      {
        std::vector<std::string> vecSource      = GetFilesName(strSourceFilePath);
        std::vector<std::string> vecDestination = GetFilesName(strDestinationFilePath);
        // 对文件列表进行排序
        std::sort(vecSource.begin(), vecSource.end());
        std::sort(vecDestination.begin(), vecDestination.end());
        if (vecSource == vecDestination)
        {
          return true;
        }
      }
      // 如果目标文件夹下有文件，则认为拷贝完成
      return false;
    }

    //创建多级文件夹_strPath以“\\”结尾
    static inline  void CreateFileIfNoExist(const std::string& _strPath)
    {
      if (_access(_strPath.c_str(), 0))
      {
        MakeSureDirectoryPathExists(_strPath.c_str());
      }
    }

    //创建zip文件
    void CreateZipFile(const std::string& sourceFolder, const std::string& destinationFolder, bool bDelSorceFile)
    {
      const std::string folderToZip = sourceFolder; // 要压缩的文件夹路径
      std::string zipFilePath = sourceFolder + ".zip"; // 生成的 ZIP 文件路径
      if (!destinationFolder.empty())
      {
        zipFilePath = destinationFolder;
      }
      zipFile zip = zipOpen(zipFilePath.c_str(), APPEND_STATUS_CREATE); // 创建一个新的 ZIP 文件

      if (zip)
      {
        AddFolderContentsToZip(zip, folderToZip);
        zipClose(zip, nullptr); // 关闭 ZIP 文件
        if (bDelSorceFile)
        {
          DelFile(sourceFolder);
        }
      }
    }

    void  AddFileToZip(zipFile& zip, const std::string& filePath, const std::string& zipEntryName)
    {
      std::ifstream fileStream(filePath, std::ios::binary);
      if (fileStream)
      {
        int err = zipOpenNewFileInZip(zip, zipEntryName.c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
        if (err == ZIP_OK)
        {
          char buffer[1024];
          while (fileStream.read(buffer, sizeof(buffer)).gcount() > 0)
          {
            zipWriteInFileInZip(zip, buffer, static_cast<unsigned int>(fileStream.gcount()));
          }
          zipCloseFileInZip(zip);
        }
        fileStream.close();
      }
    }

    void  AddFolderContentsToZip(zipFile& zip, const std::string& folderPath, const std::string& parentZipEntry="")
    {
      WIN32_FIND_DATA findFileData;
      HANDLE hFind = INVALID_HANDLE_VALUE;
      std::string searchPath = folderPath + "\\*";
      hFind = FindFirstFile(dsnCh_Tool::string2CString(searchPath), &findFileData);
      if (hFind != INVALID_HANDLE_VALUE)
      {
        do {
          const std::string fileName = dsnCh_Tool::U2G(dsnCh_Tool::wchar2char(findFileData.cFileName));
          if (fileName != "." && fileName != "..") {
            const std::string filePath = folderPath + "\\" + fileName;
            const std::string zipEntryName = (parentZipEntry.empty() ? fileName : parentZipEntry + "/" + fileName);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
              AddFolderContentsToZip(zip, filePath, zipEntryName);
            }
            else
            {
              AddFileToZip(zip, filePath, zipEntryName);
            }
          }
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
      }
    }

    //获取模型名称，如获取"C:\\Users\\10382\\上部_整体计算书.docx"的"上部_整体计算书.docx"
    static std::string GetModelName(const std::string& _strPath)
    {
      std::regex re("[^\\\\]+$");
      std::smatch matches;
      if (std::regex_search(_strPath, matches, re))
      {
        return matches[0];
      }
      return {};
    }

    //编写bat文件并启动
    static bool WriteBatFileAndStart(const std::string &strBatPath, const std::vector<std::string> &_vec)
    {
      std::shared_ptr<std::ofstream> pWriter = std::make_shared<std::ofstream>();
      pWriter->open(strBatPath.c_str(), std::ios::trunc | std::ios::out);
      if (!pWriter->is_open()) return false;
      for (const auto& x_Data : _vec)
      {
        pWriter->write(x_Data.c_str(), strlen(x_Data.c_str()));
      }
      pWriter->close();
      //system();
      //启动bat
     return WinExec(strBatPath.c_str(), SW_HIDE);
    }

    //删除某个文件/文件夹(末尾不用带\\)
    static bool DelFile(const std::string& _strPath)
    {
      if (IsFileFolder(_strPath))
      {
        //获取路径
        DeleteDirectory(dsnCh_Tool::string2wstring(_strPath));
        //std::string strBatPath(_strPath + "\\deleteFile.bat");
        //std::vector<std::string> _vec{ {"@echo off\n"}, {"rd /s /q \"" + _strPath + "\"\n"}};
        ////编写bat文件并启动
        //WriteBatFileAndStart(strBatPath, _vec);
      }
      else
      {
       return !std::remove(_strPath.c_str());
      }
      return true;
      //return bIsFileFolder ? RemoveDirectory(dsnCh::string2CString(_strPath)) : DeleteFile(dsnCh::string2CString(_strPath));
    }

    static bool DeleteDirectory(const std::wstring& path)
    {
      // 递归删除目录及其内容
      WIN32_FIND_DATA findFileData;
      HANDLE hFind = FindFirstFile((path + L"\\*").c_str(), &findFileData);

      if (hFind == INVALID_HANDLE_VALUE)
      {
        return false;
      }
      do
      {
        if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0)
        {
          std::wstring filePath = path + L"\\" + findFileData.cFileName;

          if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
          {
            // 递归删除子目录
            if (!DeleteDirectory(filePath))
            {
              FindClose(hFind);
              return false;
            }
          }
          else
          {
            // 删除文件
            if (DeleteFile(filePath.c_str()) == 0)
            {
              FindClose(hFind);
              return false;
            }
          }
        }
      } while (FindNextFile(hFind, &findFileData) != 0);
      FindClose(hFind);
      // 删除目录
      if (RemoveDirectory(path.c_str()) == 0)
      {
        return false;
      }
      return true;
    }

    //0-拷贝文件夹1内所有内容到文件夹2(还是拷贝文件夹1成为文件夹2)
    //1-拷贝文件夹1的文件1、2(全路径)到文件夹2内(全路径),支持重命名，路径修改也属于重命名范畴
    //2-将文件名(1.txt)所有vecFileName文件从_strExportPath1文件夹(C://1)1拷贝到文件夹_strInPath2内(C://2)
    static  bool CopyFileRename(const std::string& _strExportPath, const std::string& _strInPath, const std::vector<std::string>& vecFileName = {})
    {
      //文件夹1拷贝到文件夹2
      if (IsFileFolder(_strExportPath)==1&& IsFileFolder(_strInPath)==1)
      {
        CreateFileIfNoExist(_strInPath);
        //vecFileName不为空
        if (!vecFileName.empty())
        {
          for (const auto x_File : vecFileName)
          {
           CopyFile(dsnCh_Tool::string2CString(_strExportPath + "\\" + x_File), dsnCh_Tool::string2CString(_strInPath + "\\" + x_File), FALSE);
          }
          return true;
        }
        //获取路径
        std::string strBatPath(_strInPath + "\\copyFile.bat");
        std::vector<std::string> _vec{ {"@echo off\n"}, {"xcopy /E /I /Q /Y " + _strExportPath + " " + _strInPath + "\n"},{"del /Q \""+ strBatPath + "\"\n"}};
        //编写bat文件并启动
       return WriteBatFileAndStart(strBatPath, _vec);
      }
      //都是文件
      else if (!IsFileFolder(_strExportPath) && !IsFileFolder(_strInPath))
      {
        return CopyFile(dsnCh_Tool::string2CString(_strExportPath), dsnCh_Tool::string2CString(_strInPath), FALSE);
      }
      //源文件路径是文件(后缀为\\)，目的地文件是文件夹(后缀为\\)
      else if (!IsFileFolder(_strExportPath) && IsFileFolder(_strInPath))
      {
        CreateFileIfNoExist(_strInPath);
        //获取路径
        std::string strBatPath(_strInPath + "\\copyFile.bat");
        std::vector<std::string> _vec{ {"@echo off\n"}, {"copy /Y " + _strExportPath + " " + _strInPath + "\n"},{"del /Q \"" + strBatPath + "\""} };
        //编写bat文件并启动
        return WriteBatFileAndStart(strBatPath, _vec);
      }
      return false;
    }


    void CopyFolder(const std::wstring& sourceFolder, const std::wstring& destinationFolder)
    {
      if (!CreateDirectoryW(destinationFolder.c_str(), NULL) && ERROR_ALREADY_EXISTS != GetLastError())
      {
        AfxMessageBox(L"无法创建文件夹");
        return;
      }

      std::wstring searchPath = sourceFolder + L"\\*.*";
      WIN32_FIND_DATAW fileData;
      HANDLE hFind = FindFirstFileW(searchPath.c_str(), &fileData);

      if (hFind != INVALID_HANDLE_VALUE)
      {
        do {
          std::wstring fileName = fileData.cFileName;
          if (fileName != L"." && fileName != L"..")
          {
            std::wstring sourcePath = sourceFolder + L"\\" + fileName;
            std::wstring destinationPath = destinationFolder + L"\\" + fileName;

            if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
              // 如果是文件夹，递归拷贝
              CopyFolder(sourcePath, destinationPath);
            }
            else
            {
              // 如果是文件，复制到目标文件夹
              if (!CopyFileW(sourcePath.c_str(), destinationPath.c_str(), FALSE))
              {
                AfxMessageBox(L"无法复制文件");
              }
            }
          }
        } while (FindNextFileW(hFind, &fileData) != 0);

        FindClose(hFind);
      }
      else
      {
        AfxMessageBox(L"无法访问源文件夹");
      }
    }

    //获取文件夹filePath（C:\Users\10382\Desktop\PEC材料统计\施工图）内所有文件名
    static void getFilesName(const std::string& filePath, std::vector<std::string>& files,
      const std::vector<std::regex>& vecFormats={})
    {
      //文件句柄
      intptr_t hFile = 0;
      //文件信息
      struct _finddata_t fileinfo;
      std::string p;
      if ((hFile = _findfirst(p.assign(filePath).append("\\*").c_str(), &fileinfo)) != -1)
      {
        do
        {
          //如果是目录,迭代之
          //如果不是,加入列表
          if ((fileinfo.attrib & _A_SUBDIR))
          {
            if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
              getFilesName(p.assign(filePath).append("\\").append(fileinfo.name), files);
          }
          else
          {
            std::string strName = p.assign(filePath).append("\\").append(fileinfo.name);
            for (const auto& regex : vecFormats)
            {
              std::smatch matches;
              if (std::regex_search(strName, matches, regex))
              {
                files.push_back(matches[1]);
                break; // 如果找到匹配的格式，停止搜索
              }
            }
          }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
      }
    }
    //获取全所有路径名称
     //获取文件夹filePath（C:\Users\10382\Desktop\PEC材料统计\施工图）内所有文件名
    //0-代表全路径，1-相对路径
    static std::vector<std::string> GetFilesName(const std::string& filePath,int nType=1)
    {
      std::vector<std::string> _vecCompletePath;
      //文件句柄
      intptr_t hFile = 0;
      //文件信息
      struct _finddata_t fileinfo;
      std::string p;
      if ((hFile = _findfirst(p.assign(filePath).append("\\*").c_str(), &fileinfo)) != -1)
      {
        do
        {
          //如果是目录,迭代之
          //如果不是,加入列表
          if ((fileinfo.attrib & _A_SUBDIR))
          {
            if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
              getFilesName(p.assign(filePath).append("\\").append(fileinfo.name), _vecCompletePath);
          }
          else
          {
            std::string strName = p.assign(filePath).append("\\").append(fileinfo.name);
            _vecCompletePath.emplace_back(strName);
          }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
      }
      if (nType==1)
      {
        for (auto& x_File : _vecCompletePath)
        {
          CString strPath(dsnCh_Tool::string2CString(x_File));
          strPath.Replace(dsnCh_Tool::string2CString(filePath + "\\"), L"");
          x_File = dsnCh_Tool::CString2String(strPath);
        }
      }
      return _vecCompletePath;
    }

    //读取文件所有数据的字符串
    static inline std::string GetFileDataStr(const std::string& _strPath)
    {
      std::ifstream fin(_strPath);         // filename: xly2016I.txt
      if (fin.fail()) return "";           // 若文件不存在，则返回空
      std::stringstream buffer;            // stringstream object
      buffer << fin.rdbuf();               // read file content in stringstream object
      std::string str(buffer.str());       // store file content in a string
      fin.close();// 关闭文件
      return dsnCh_Tool::U2G(str.c_str());
    }

    //写字符串
    static void WriteText(const std::vector<std::string>& _vecText, const std::string& _strPath)
    {
      std::shared_ptr<std::ofstream> pWriter = std::make_shared<std::ofstream>();
      pWriter->open(_strPath.c_str(), std::ios::trunc | std::ios::out);
      if (!pWriter->is_open()) return;
      for (const auto& x_Data : _vecText)
      {
        std::string strText(x_Data + "\n");
        pWriter->write(strText.c_str(), strlen(strText.c_str()));
      }
      pWriter->close();
    }

    //读取字符串内所有数字
    template<typename T>
    static inline std::vector<T> GetNumberFromStr(const std::string& _str)
    {
      // 定义一个正则表达式模式，用于匹配数字
      std::regex pattern("\\d+");
      // 创建一个正则表达式迭代器，用于找到所有匹配的数字
      std::sregex_iterator iter(text.begin(), text.end(), pattern);
      std::sregex_iterator end;
      // 遍历迭代器，提取并输出匹配到的数字
      std::vector<T> _vecData;
      while (iter != end) 
      {
        std::smatch match = *iter;  // 获取匹配结果
        std::string number_str = match.str();  // 将匹配结果转换为字符串
        T number = std::stoi(number_str);  // 将字符串转换为整数
        _vecData.emplace_back(number);
        ++iter;  // 移动到下一个匹配项
      }
      return _vecData;
    }

    //数据拆分，如“，”，“|”，“ ”分割等
    static inline std::vector<std::string> SplitStr(const std::string& _strText,const std::string&strSeg)
    {
      // 定义一个正则表达式模式，用于匹配数字t分割符
      std::regex pattern("([^" + std::string() + strSeg + "]+)");
      // 创建一个正则表达式迭代器，用于找到所有匹配的数字
      std::sregex_iterator iter(_strText.begin(), _strText.end(), pattern);
      std::sregex_iterator end;
      // 遍历迭代器，提取并输出匹配到的数字
      std::vector<std::string> _vecData;
      while (iter != end)
      {
        std::smatch match = *iter;  // 获取匹配结果
        std::string _str = match.str();  // 将匹配结果转换为字符串
        _str = std::regex_replace(_str, std::regex("^\\s+|\\s+$"), ""); //将开头和结尾的空格取消
        _vecData.emplace_back(_str);
        ++iter;  // 移动到下一个匹配项
      }
      return _vecData;
    }

    //弹出word,txt,excel文件
    static inline void PopupFile(const std::string& _strTxtPath)
    {
      ShellExecute(NULL, _T("open"), dsnCh_Tool::string2CString(_strTxtPath), NULL, NULL, SW_SHOWNORMAL);
      //::WinExec(("notepad.exe " + _strTxtPath).c_str(), SW_SHOWNORMAL);
    }

  };
#pragma endregion

#pragma region 应用程序
  class dsnApp_Tool
  {
  public:
    dsnApp_Tool() = delete;
    ~dsnApp_Tool() = delete;
  public:


  };
#pragma endregion

#pragma region 对话框
  class dsnDlg_Tool:public CDialogEx
  {
  public:
    dsnDlg_Tool () = default;
    ~dsnDlg_Tool() = default;
  public:
    //树控件选项核查，绑定消息应为TVN_ITEMCHANGED
    //如ON_NOTIFY(TVN_ITEMCHANGED, IDC_TREE_WaveCase  , &DlgShockReport_HystereticCurve::OnCheckTreeItem)
    void OnCheckTreeItem(NMHDR* pNMHDR, LRESULT* pResult, CTreeCtrl& treeCtrl)
    {
      NMITEMACTIVATE* pNMItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);
      HTREEITEM hItem = GetHitItem(treeCtrl);
      TreeCheckUpdata(hItem, treeCtrl);
    }
    //点击测试
    HTREEITEM GetHitItem(CTreeCtrl& treeCtrl)
    {
      UINT uFlags;
      CPoint point;
      GetCursorPos(&point);
      treeCtrl.ScreenToClient(&point);  // 转换为树控件的客户端坐标
      HTREEITEM hItem = treeCtrl.HitTest(point, &uFlags);
      return hItem;
    }
    //结点复选框状态
    //两种情况：
    //（1）选中父节点，根据父节点勾选情况，更新子节点；
    //（2）选中子节点，根据子节点勾选情况，更新父节点
    void TreeCheckUpdata(HTREEITEM hItem, CTreeCtrl& treeCtrl)
    {
      if (!hItem) return;
      //若点击一级结点
      if (!treeCtrl.GetParentItem(hItem))
      {
        BOOL bCheck = treeCtrl.GetCheck(hItem);
        TreeChildNodeSelectedUpdata(hItem, bCheck, treeCtrl);
      }
      //二级标题
      else if (treeCtrl.GetParentItem(hItem) && !treeCtrl.GetParentItem(treeCtrl.GetParentItem(hItem)))
      {
        BOOL bChecked = CheckChildNodeSel(hItem, treeCtrl);//二级标题是否全部勾选
        treeCtrl.SetCheck(treeCtrl.GetParentItem(hItem), bChecked);//初始化一级标题
      }
    }
    void TreeChildNodeSelectedUpdata(HTREEITEM hItem, BOOL bSelect, CTreeCtrl& treeCtrl)
    {
      while (hItem != NULL)
      {
        treeCtrl.SetCheck(hItem, bSelect);
        if (treeCtrl.ItemHasChildren(hItem))
        {
          TreeChildNodeSelectedUpdata(treeCtrl.GetChildItem(hItem), bSelect, treeCtrl);
        }
        hItem = treeCtrl.GetNextSiblingItem(hItem);
      }
    }

    BOOL CheckChildNodeSel(HTREEITEM hItem, CTreeCtrl& treeCtrl)
    {
      //GetNextSiblingItem只能获取结点以下的内容，结点以上不行
      HTREEITEM hParentItem = treeCtrl.GetParentItem(hItem);
      if (hParentItem)//由父节点以下进行遍历
      {
        hItem = treeCtrl.GetChildItem(hParentItem);
      }
      while (hItem != NULL)
      {
        if (!treeCtrl.GetCheck(hItem)) return FALSE;
        if (treeCtrl.ItemHasChildren(hItem))
        {
          if (!CheckChildNodeSel(treeCtrl.GetChildItem(hItem), treeCtrl))
          {
            return FALSE;
          }
        }
        hItem = treeCtrl.GetNextSiblingItem(hItem);
      }
      return TRUE;
    }
  };
#pragma endregion

}





///=================================================================================================
///类和命名空间的重命名
namespace dsnGTL = dsnGeneralTool;
using dsnCompare = dsnGeneralTool::dsnCompare_Tool;
using dsnCh      = dsnGeneralTool::dsnCh_Tool     ;
using dsnApp     = dsnGeneralTool::dsnApp_Tool    ;
using dsnFile    = dsnGeneralTool::dsnFile_Tool   ;
using dsnMath    = dsnGeneralTool::dsnMath_Tool   ;
using dsnDlg     = dsnGeneralTool::dsnDlg_Tool    ;




///=================================================================================================
///iostream类
namespace FileReadWrite
{
  class dsnFileRW
  {
  public:
    dsnFileRW() = delete;
    ~dsnFileRW() = delete;
  public:

    std::string m_strPath;//文件路径
    std::string m_ReportTitle;//报告头
    std::shared_ptr<std::ifstream> m_ReadFile;//读文件
    std::map<std::string, std::string> m_mapKeyText;//键值-所需内容
    //创建数据流
    void createMyStream()
    {
      m_ReadFile = std::make_shared<std::ifstream>();
    }

    //判断路径下的文件是否存在，若存在返回true，否则，返回false
    const bool setPath(const std::string& strPath, const std::string& strTitle)
    {
      if (dsnFile::bFileExist(strPath))
      {
        m_strPath = strPath;
        m_ReportTitle = "[" + strTitle + "]";
        return true;
      }
      return false;
    }
  };
}

using myStream = FileReadWrite::dsnFileRW;
///=================================================================================================
///控制台程序
#pragma warning(disable:4996)
class StartupConsole
{
public:
  StartupConsole()
  {
    AllocConsole();
    freopen("CON", "r", stdin);
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
  }
  ~StartupConsole()
  {
    FreeConsole();
  }
};

//时间函数
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <map>
#include<ratio>
class TimeMgr
{
public:
  TimeMgr() :m_begin(std::chrono::high_resolution_clock::now()) {}
  enum class TimeType
  {
    DTT_milliseconds,//毫秒
    DTT_microseconds,//微秒
    DTT_nanoseconds,//纳秒
    DTT_seconds,//秒
    DTT_minutes,//分
    DTT_hours,//小时
  };
public:
  void reset() { m_begin = std::chrono::high_resolution_clock::now(); }
  //哈希表
  int64_t NowTime(TimeType _type)
  {
    static std::unordered_map <TimeMgr::TimeType, TimeMgr::timeFunc> _hash
    {
      {TimeType::DTT_milliseconds ,&TimeMgr::elapsed_milli  },
      {TimeType::DTT_microseconds ,&TimeMgr::elapsed_micro  },
      {TimeType::DTT_nanoseconds  ,&TimeMgr::elapsed_nano   },
      {TimeType::DTT_seconds      ,&TimeMgr::elapsed_seconds},
      {TimeType::DTT_minutes      ,&TimeMgr::elapsed_minutes},
      {TimeType::DTT_hours        ,&TimeMgr::elapsed_hours  }
    };
    auto it = find2Value(_hash, _type, &TimeMgr::elapsed_default);
    auto _timeDif = (this->*it)();
    reset();
    return _timeDif;
  }
private:
  //默认输出毫秒
  template<typename Duration = std::chrono::milliseconds>
  int64_t elapsed() { return std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now() - m_begin).count(); }
  //毫秒
  int64_t elapsed_milli() { return elapsed<std::chrono::milliseconds>(); }
  //微秒
  int64_t elapsed_micro() { return elapsed<std::chrono::microseconds>(); }
  //纳秒
  int64_t elapsed_nano() { return elapsed<std::chrono::nanoseconds >(); }
  //秒
  int64_t elapsed_seconds() { return elapsed<std::chrono::seconds     >(); }
  //分                                                              
  int64_t elapsed_minutes() { return elapsed<std::chrono::minutes     >(); }
  //时                                                              
  int64_t elapsed_hours() { return elapsed<std::chrono::hours       >(); }
  //默认
  int64_t elapsed_default() { return 0; }

  template<class T1, class T2> T2 find2Value(const std::unordered_map<T1, T2>& _map, const T1& _t, const T2& _default)
  {
    auto it = _map.find(_t);
    return it != _map.end() ? it->second : _default;
  }

  const std::string DurTime2Str()
  {
    //换算成毫秒
    auto _nTotalTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin);

    //小时
    auto _hh = std::chrono::duration_cast<std::chrono::hours       >(_nTotalTime).count();
    //分钟
    auto _mm = std::chrono::duration_cast<std::chrono::minutes     >(_nTotalTime).count();
    int _nDIfmm = _mm - _hh * 60;
    //秒
    auto _ss = std::chrono::duration_cast<std::chrono::seconds     >(_nTotalTime).count();
    int _nDIfss = _ss - _mm * 60;
    ////毫秒
    //auto _ms = std::chrono::duration_cast<std::chrono::milliseconds>(_nTotalTime).count();
    //int _nDIfms = _ms - _ss * 60;

    std::string _str;
    _str += "总耗时为" + dsnCh::DT2Str(_hh) + "时" + dsnCh::DT2Str(_mm) + "分" + dsnCh::DT2Str(_ss) + "秒";
    return _str;
  }

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
  using timeFunc = int64_t(TimeMgr::*)();
};


//函数封装器
// 实现 index_sequence 和 make_index_sequence 的 C++14 版本
// 实现 index_sequence 和 make_index_sequence 的 C++14 版本
namespace custom {
  template<std::size_t... Ints>
  struct index_sequence {};

  template<std::size_t N, std::size_t... Ints>
  struct make_index_sequence_helper : make_index_sequence_helper<N - 1, N - 1, Ints...> {};

  template<std::size_t... Ints>
  struct make_index_sequence_helper<0, Ints...> {
    using type = index_sequence<Ints...>;
  };

  template<std::size_t N>
  using make_index_sequence = typename make_index_sequence_helper<N>::type;
}

namespace dsnGeneralTool {
  // Helper class to get the tuple type and return type of a function
  template<typename Func>
  struct FunctionTraits;

  template<typename Ret, typename... Args>
  struct FunctionTraits<Ret(Args...)> {
    using TupleType = std::tuple<Args...>;
    using ReturnType = Ret;
  };

  template<typename Ret, typename... Args>
  struct FunctionTraits<Ret(*)(Args...)> : FunctionTraits<Ret(Args...)> {};

  template<typename Ret, typename... Args>
  struct FunctionTraits<std::function<Ret(Args...)>> : FunctionTraits<Ret(Args...)> {};

  template<typename Func>
  struct FunctionTraits : FunctionTraits<decltype(&Func::operator())> {};

  template<typename Ret, typename ClassType, typename... Args>
  struct FunctionTraits<Ret(ClassType::*)(Args...)> : FunctionTraits<Ret(Args...)> {};

  template<typename Ret, typename ClassType, typename... Args>
  struct FunctionTraits<Ret(ClassType::*)(Args...) const> : FunctionTraits<Ret(Args...)> {};

  template<typename Key>
  class FunctionWrap {
  public:
    FunctionWrap() = default;
    ~FunctionWrap() = default;

    template<typename Func>
    void add(const Key& name, Func func) {
      using TupleType = typename FunctionTraits<Func>::TupleType;
      using ReturnType = typename FunctionTraits<Func>::ReturnType;
      func_map[name] = [func](std::shared_ptr<void> args, std::shared_ptr<void> result) {
        callFunc(func, *std::static_pointer_cast<TupleType>(args), result);
      };
    }

    template<typename Ret, typename ClassType, typename... Args>
    void add(const Key& name, ClassType* instance, Ret(ClassType::* func)(Args...)) {
      using Func = Ret(ClassType::*)(Args...);
      add(name, [instance, func](Args... args) {
        return (instance->*func)(std::forward<Args>(args)...);
        });
    }

    template<typename Ret, typename... Args>
    typename std::enable_if<!std::is_void<Ret>::value, Ret>::type
      run(const Key& name, Args&&... args) const {
      auto it = func_map.find(name);
      if (it == func_map.end()) {
        std::cerr << "Function not found: " << name << std::endl;
        throw std::runtime_error("Function not found");
      }

      using TupleType = std::tuple<std::decay_t<Args>...>;
      auto argsPtr = std::make_shared<TupleType>(std::forward<Args>(args)...);
      auto resultPtr = std::make_shared<Ret>();

      it->second(argsPtr, resultPtr);
      return *resultPtr;
    }

    template<typename Ret, typename... Args>
    typename std::enable_if<std::is_void<Ret>::value, void>::type
      run(const Key& name, Args&&... args) const {
      auto it = func_map.find(name);
      if (it == func_map.end()) {
        std::cerr << "Function not found: " << name << std::endl;
        throw std::runtime_error("Function not found");
      }

      using TupleType = std::tuple<std::decay_t<Args>...>;
      auto argsPtr = std::make_shared<TupleType>(std::forward<Args>(args)...);
      auto resultPtr = std::shared_ptr<void>(nullptr); // For void return type

      it->second(argsPtr, resultPtr);
    }

    template<typename Ret, typename... Args>
    auto operator[](const Key& name) const {
      return [this, name](Args&&... args) {
        return this->template run<Ret>(name, std::forward<Args>(args)...);
      };
    }

    template<typename... Args>
    void operator()(const Key& name, Args&&... args) const {
      run<void>(name, std::forward<Args>(args)...);
    }

    void clear() { func_map.clear(); }
    size_t size() const { return func_map.size(); }

  private:
    std::map<Key, std::function<void(std::shared_ptr<void>, std::shared_ptr<void>)>> func_map;

    // Function call helper
    template<typename Func, typename Tuple, std::size_t... I>
    static void callFuncImpl(Func func, Tuple args, std::shared_ptr<void> result, custom::index_sequence<I...>, std::true_type) {
      func(std::get<I>(args)...);
    }

    template<typename Func, typename Tuple, std::size_t... I>
    static void callFuncImpl(Func func, Tuple args, std::shared_ptr<void> result, custom::index_sequence<I...>, std::false_type) {
      using ReturnType = typename FunctionTraits<Func>::ReturnType;
      auto res = func(std::get<I>(args)...);
      *std::static_pointer_cast<ReturnType>(result) = res;
    }

    template<typename Func, typename Tuple>
    static void callFunc(Func func, Tuple args, std::shared_ptr<void> result) {
      using ReturnType = typename FunctionTraits<Func>::ReturnType;
      callFuncImpl(func, args, result, custom::make_index_sequence<std::tuple_size<Tuple>::value>{}, std::is_void<ReturnType>{});
    }
  };
};


template<typename Key>
using dsnFuncWrap = dsnGeneralTool::FunctionWrap<Key>;
//使用方式
//using FunctionWrapper = dsnGeneralTool::FunctionWrap<std::string>;
//FunctionWrapper funcWrapper;
//// Use the helper function to add member functions
//funcWrapper.addFunction("func1", this, &FuncExample::exampleFunc1);
//funcWrapper.addFunction("func2", this, &FuncExample::exampleFunc2);
//funcWrapper.addFunction("func3", this, &FuncExample::exampleFunc3);
//
//int b = 2;
//double dValue = funcWrapper.callFunction<double>("func1", 1.5f, std::ref(b));
//int dValue1 = funcWrapper.callFunction<int>("func2", 3.0, 4, 5);
//std::string dValue2 = funcWrapper.callFunction<std::string>("func3", 6, 7, 8, 9.0);