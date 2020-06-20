#ifndef DATABASE_H
#define DATABASE_H

#include <string>

class Database
{
public:
  Database(std::string dbname, std::string fullpath);

  std::string getDirectory(void);

  // Key-Value use cases
  void        setKeyValue(std::string key,std::string value);
  std::string getKeyValue(std::string key);

  // management functions
  static Database createEmpty(std::string dbname);
  static Database load(std::string dbname);
  void        destroy();

protected:
    std::string m_name;
    std::string m_fullpath;
    // TODO std::pointer<DatabaseImpl> pImpl; // C++11 PIMPL IDIOM for forward binary compatibility
};

#endif // DATABASE_H
