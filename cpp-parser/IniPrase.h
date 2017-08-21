//
//  IniPrase.h
//
//  Created by King.Sollyu on 14-8-7.
//  Copyright (c) 2014�� sollyu. All rights reserved.
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
        // Ĭ�ϵĹ��캯������������
        IniParser();
        ~IniParser();
        
        // ���캯�� - �����ļ�
        IniParser(const std::string& file_name);
        
        // ����һ��ini�ļ�, ���֮ǰ���ļ����޸�, ��ô֮ǰ��ini�ļ����ᱻ���֡�
        bool load(const std::string& file_name);
        
        // ���ַ�������Ϊini�ļ�����
        bool loadString(const std::string& str);
        
        // ���ֵ�����iniλ��
        bool save();
        
        // ���Ϊһ���ͼ���·����һ�����ļ���
        bool saveAs(const std::string& file_name);
        
        // ����ini�Ƿ��޸�, ���������һ�β����Ǳ���
        bool isModified() const { return m_modified; }
        
    public:    // high level member function.
        
        // ����ĳ�Ա�����Ǵ�Section�л��һЩֵ
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
        // ����һ��section��map��ֵ��
        const KeyMap& getSection(const std::string& section) const;
        
        // ��������ini��Sections
        const SectionMap& getIni() const { return m_map; }
        
    private:
        void saveBeforeLoad();
        const char* key_value(const std::string& section, const std::string& key);
        
    private:
        // ��ֹ���ƹ��캯���͸�ֵ��������
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
