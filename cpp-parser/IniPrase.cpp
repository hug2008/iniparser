//
//  IniPrase.cpp
//  iniPraseDemo
//
//  Created by King.Sollyu on 14-8-7.
//  Copyright (c) 2014年 sollyu. All rights reserved.
//
//  Site: http://www.sollyu.com/ini-parse-class

//////////////////////////////////////////////////////////////////////////
// IniParser.cpp

#ifdef _AFX
#include "StdAfx.h"      // remove this line, if you're not using msvc.
#pragma warning(disable : 4786 4503)
#endif

#include "IniParser.h"

#include <fstream>
#include <strstream>


using namespace std;


#ifdef _AFX
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

//namespace FileConfig
//{
    const char  IniParser::left_tag   = '[' ;
    const char  IniParser::right_tag  = ']' ;
    const char  IniParser::equal      = '=' ;
    const char  IniParser::cr         = '\r';
    const char  IniParser::new_line   = '\n';
    const char* IniParser::empty_str  = ""  ;
    const int   IniParser::BUFFER_LEN = 255 ;
    
    const IniParser::KeyMap IniParser::ms_emptySection;
    
    
    IniParser::IniParser() : m_modified(false)
    {
    }
    
    
    IniParser::IniParser(const std::string& file_name) : m_modified(false)
    {
        load(file_name);
    }
    
    
    IniParser::~IniParser()
    {
        if(m_modified)
            save();
    }
    
    
    void IniParser::saveBeforeLoad()
    {
        if(m_modified)
            save();
        
        
        m_file_name.resize(0);
        
        
        m_map.clear();
        
        
        m_modified = false;
    }
    
    
    const char* IniParser::key_value(const std::string& section, const std::string& key)
    {
        SectionIterator itSection = m_map.find(section);
        if(m_map.end() != itSection)
        {
            KeyIterator itKey = itSection->second.find(key);
            if(itKey != itSection->second.end())
                return itKey->second.c_str();
        }
        return 0;
    }
    
    
    bool IniParser::load(const std::string& file_name)
    {
        saveBeforeLoad();
        
        ifstream file(file_name);
        if(!file)
            return false;
        
        file.seekg(0, ios::end);
        long len = file.tellg();
        if(len < 0)
            return false;
        
        
        char* buffer = new char[len + 1];
        if(0 == buffer)
            return false;
        
        
        file.seekg(0, ios::beg);
        file.read(buffer, len);
    
        buffer[len = file.gcount()] = 0;
        
        loadString(buffer);
        m_file_name = file_name;
        
        
        delete[] buffer;
        
        
        return true;
    }
    
    
    bool IniParser::loadString(const std::string& str)
    {
        saveBeforeLoad();
        
        unsigned long length = str.size();
        
        if (str.size() == 0)
            return false;
        
        
        enum status
        {
            after_left_tag,
            after_section_name,
            after_section_name_ws,
            after_key_name,
            after_key_name_ws,
            after_equal,
            start
        };
        
        
        string section;                       // 当前 section.
        string key;                           // 当前 key.
        status sta          = start;          // 解析状态.
        const char* p       = str.c_str();    // 当前解析字符串的位置.
        const char* beg     = p;              // 当前元素的开始.
        const char* last_ws = p;              // 最后一个空格字符.
        
        for(; length; ++p, --length)
        {
            if(new_line == *p)
            {
                if(after_equal == sta)
                {
                    if(cr == *(p - 1))
                        --p;
                    
                    m_map[section][key] = string(beg, p - beg);
                    
                    if(cr == *p)
                        ++p;
                }
                sta = start;
            }
            else
            {
                switch(sta)
                {
                    case after_left_tag:
                        if(right_tag == *p)
                        {
                            sta     = start;
                            section = empty_str; // empty section name.
                        }
                        else if(!isspace((unsigned char)*p))
                        {
                            sta = after_section_name;
                            beg = p;
                        }
                        break;
                        
                        
                    case after_section_name:
                        if(right_tag == *p)
                        {
                            sta     = start;
                            section = string(beg, p - beg);
                        }
                        else if(isspace((unsigned char)*p))
                        {
                            sta     = after_section_name_ws;
                            last_ws = p;
                        }
                        break;
                        
                        
                    case after_section_name_ws:
                        if(right_tag == *p)
                        {
                            sta     = start;
                            section = string(beg, last_ws - beg);
                        }
                        else if(!isspace((unsigned char)*p))
                        {
                            sta = after_section_name;
                        }
                        break;
                        
                        
                    case after_key_name:
                        if(equal == *p)
                        {
                            sta = after_equal;
                            key = string(beg, p - beg);
                            beg = p + 1;
                        }
                        else if(isspace((unsigned char)*p))
                        {
                            sta     = after_key_name_ws;
                            last_ws = p;
                        }
                        break;
                        
                        
                    case after_key_name_ws:
                        if(equal == *p)
                        {
                            sta = after_equal;
                            key = string(beg, last_ws - beg);
                            beg = p + 1;
                        }
                        else if(!isspace((unsigned char)*p))
                        {
                            sta = after_key_name;
                        }
                        break;
                        
                        
                    case start:
                        if(left_tag == *p)
                        {
                            sta = after_left_tag;
                        }
                        else if(equal == *p)
                        {
                            key = empty_str;  // an empty key.
                            sta = after_equal;
                            beg = p + 1;
                        }
                        else if(!isspace((unsigned char)*p))
                        {
                            sta = after_key_name;
                            beg = p;
                        }
                        break;
                }
            }
        }
        
        
        if(after_equal == sta)
            m_map[section][key] = string(beg, p - beg);
        
        return true;
    }
    
    
    bool IniParser::save()
    {
        if(0==m_file_name.c_str() || 0==m_file_name[0])
            return false;    // file name invalid
        
        ofstream file(m_file_name.c_str());
        if(!file)
            return false;
        
        for(SectionMap::iterator itApp=m_map.begin(); itApp!=m_map.end(); ++itApp)
        {
            file << left_tag << itApp->first << right_tag << endl;
            
            for(KeyMap::iterator itKey=itApp->second.begin(); itKey!=itApp->second.end(); ++itKey)
                file << itKey->first << equal << itKey->second << endl;
            
            file << endl;
        }
        m_modified = false;
        return true;
    }
    
    
    bool IniParser::saveAs(const std::string& file_name)
    {
        string old_file_name = m_file_name;
        m_file_name = file_name;
        
        if(save())
            return true;
        
        m_file_name = old_file_name;
        return false;
    }
    
    
    long IniParser::getInteger(const std::string& section, const std::string& key, long def_val)
    {
        istrstream(key_value(section, key)) >> def_val;
        return def_val;
    }
    
    
    float IniParser::getFloat(const std::string& section, const std::string& key, float def_val)
    {
        istrstream(key_value(section, key)) >> def_val;
        return def_val;
    }
    
    
    long IniParser::getStruct(const std::string& section, const std::string& key_, void* buffer, long size)
    {
        std::string key = key_value(section, key_);
        
        if (key.size() == 0)
            return 0;
        
        const char* p        = key.c_str();
        char*       dst      = (char*)buffer;
        long        read_len = 0;
        char        value;
        
        
        while(*p && read_len<size)
        {
            switch(*p)
            {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    value = *p - '0';
                    break;
                case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                    value = *p - 'a' + 10;
                    break;
                case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                    value = *p - 'A' + 10;
                    break;
                default:
                    return read_len;
            }
            
            
            if(0 == (p - key.c_str())%2)
                *(dst + read_len) = value << 4;
            else
                *(dst + read_len) = (*(dst + read_len) & 0xf0) + value;
            
            
            if(0 == (++p - key.c_str())%2)
                ++read_len;
        }
        
        
        return read_len;
    }
    
    
    long IniParser::getString(const std::string& section, const std::string& key_, const std::string& def_val, std::string& dst_str)
    {
        std::string key = key_value(section, key_);
        dst_str = key.length() ? key : def_val;
        return dst_str.length();
    }
    
    const std::string IniParser::getString(const std::string& section, const std::string& key_, const std::string& def_val)
    {
        std::string key = key_value(section, key_);
        if(key.length() == 0)
            key = def_val;
        
        return key;
    }
    
#ifdef _AFX
    long IniParser::getString(const char* section, const char* key, const char* def_val, CString& dst_str)
    {
        key = key_value(section, key);
        dst_str = key ? key : def_val;
        return dst_str.GetLength();
    }
#endif//_AFX
    
    void IniParser::setInteger(const std::string& section, const std::string& key, long value)
    {
        char buffer[BUFFER_LEN + 1];
        ostrstream ostr(buffer, BUFFER_LEN);
        ostr << value;
        buffer[ostr.pcount()] = 0;
        setString(section, key, buffer);
    }
    
    void IniParser::setFloat(const std::string& section, const std::string& key, float value)
    {
        char buffer[BUFFER_LEN + 1];
        ostrstream ostr(buffer, BUFFER_LEN);
        ostr << value;
        buffer[ostr.pcount()] = 0;
        setString(section, key, buffer);
    }
    
    
    inline char bin2hex(char bin)
    {
        return bin<10 ? bin+'0' : bin-10+'A';
    }
    
    
    void IniParser::setStruct(const std::string& section, const std::string& key, const void* buffer, long size)
    {
        char* dst = new char[size*2 + 1];
        if(dst)
        {
            const char* src = (const char*)buffer;
            long i=0;
            for(i=0; i<size; ++i)
            {
                dst[i << 1]       = bin2hex((src[i] >> 4) & 0x0f );
                dst[(i << 1) + 1] = bin2hex(src[i] & 0x0f);
            }

            dst[i << 1] = 0;
            setString(section, key, dst);
            
            delete[] dst;
        }
    }
    
    
    void IniParser::setString(const std::string& section, const std::string& key, const std::string& value)
    {
        m_map[section][key] = value;
        m_modified = true;
    }
    
    bool IniParser::delSection( const std::string& section )
    {
        SectionIterator itSection = m_map.find(section);
        if(m_map.end() != itSection)
        {
            m_map.erase(itSection);
            return true;
        }
        return false;
    }
    
    bool IniParser::delKey( const std::string& section, const std::string& key )
    {
        SectionIterator itSection = m_map.find(section);
        if(m_map.end() != itSection)
        {
            KeyIterator itKey = itSection->second.find(key);
            if(itKey != itSection->second.end())
            {
                itSection->second.erase(itKey);
                return true;
            }
        }
        return false;
    }
    
    const IniParser::KeyMap& IniParser::getSection(const std::string& section) const
    {
        SectionMap::const_iterator itApp = m_map.find(section);
        return m_map.end()==itApp ? ms_emptySection : itApp->second;
    }
//}
