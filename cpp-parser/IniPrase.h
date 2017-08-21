//
//  IniPrase.h
//
//  Created by King.Sollyu on 14-8-7.
//  Copyright (c) 2014年 sollyu. All rights reserved.
//
//  Site: http://www.sollyu.com/ini-parse-class

#ifndef __LXRIniParser_h__
#define __LXRIniParser_h__

#ifdef _AFX
#pragma warning(disable : 4786)
#endif

#include <map>
#include <string>

//namespace FileConfig
//{
    class IniParser
    {
    public:
        struct IgnoreCaseLT
        {
            bool operator()(const std::string& lhs, const std::string& rhs) const
            {
                #ifdef _WIN32
                return stricmp(lhs.c_str(), rhs.c_str()) < 0;
                #else
                return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
                #endif
            }
        };
        
    public:
        typedef std::map<std::string, std::string, IgnoreCaseLT> KeyMap;
        typedef std::map<std::string, KeyMap,      IgnoreCaseLT> SectionMap;
        typedef KeyMap::iterator     KeyIterator;
        typedef SectionMap::iterator SectionIterator;
        
    public:
        // 默认的构造函数和析构函数
        IniParser();
        ~IniParser();
        
        // 构造函数 - 加载文件
        IniParser(const std::string& file_name);
        
        // 加载一个ini文件, 如果之前的文件被修改, 那么之前的ini文件将会被保持。
        bool load(const std::string& file_name);
        
        // 从字符串中作为ini文件加载
        bool loadString(const std::string& str);
        
        // 保持到加载ini位置
        bool save();
        
        // 另存为一个和加载路径不一样的文件中
        bool saveAs(const std::string& file_name);
        
        // 返回ini是否被修改, 或者他最后一次操作是保存
        bool isModified() const { return m_modified; }
        
    public:    // high level member function.
        
        // 下面的成员函数是从Section中获得一些值
        long getInteger(const std::string& section, const std::string& key, long def_val);
        float getFloat(const std::string& section, const std::string& key, float def_val);
        long getStruct(const std::string& section, const std::string& key, void* buffer, long size);
        long getString(const std::string&  section, const std::string& key, const std::string& def_val, std::string& buffer);
        const std::string getString(const std::string&  section, const std::string& key, const std::string& def_val);
        
#ifdef _AFX
        // save it in a MFC::CString object dst_str.
        long getString(const char* section, const char* key, const char* def_val, CString& dst_str);
#endif

        void setInteger(const std::string& section, const std::string& key, long value);
        void setFloat(const std::string& section, const std::string& key, float value);
        void setStruct(const std::string& section, const std::string& key, const void* buffer, long size);
        void setString(const std::string& section, const std::string& key, const std::string& value);
        
    public:
        bool delSection( const std::string& section );
        bool delKey( const std::string& section, const std::string& key );
        
    public:
        // 返回一个section的map键值对
        const KeyMap& getSection(const std::string& section) const;
        
        // 返回整个ini的Sections
        const SectionMap& getIni() const { return m_map; }
        
    private:
        void saveBeforeLoad();
        const char* key_value(const std::string& section, const std::string& key);
        
    private:
        // 禁止复制构造函数和赋值操作符。
        IniParser(const IniParser& copy);
        IniParser& operator=(const IniParser& rhs);
        
    private:
        static const KeyMap ms_emptySection;
        static const char   left_tag       ;
        static const char   right_tag      ;
        static const char   equal          ;
        static const char   cr             ;
        static const char   new_line       ;
        static const char*  empty_str      ;
        static const int    BUFFER_LEN     ;
        
        SectionMap  m_map;
        std::string m_file_name;
        bool        m_modified;
    };
//}// end of namespace leiXure
#endif//__LXRIniParser_h__
