#include "database.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Database::Database(std::string dbname,std::string fullpath)
  : m_name(dbname), m_fullpath(fullpath)
{
  ;
}

// Management functions

Database Database::createEmpty(std::string dbname) {
  std::string basedir(".groundupdb");
  if (!fs::exists(basedir)) {
      fs::create_directory(basedir);
  }
  std::string dbfolder(basedir + "/" + dbname);
  if (!fs::exists(dbfolder)) {
      fs::create_directory(dbfolder);
  }
  return Database(dbname,dbfolder);
}

Database Database::load(std::string dbname) {
  std::string basedir(".groundupdb");
  std::string dbfolder(basedir + "/" + dbname);
  return Database(dbname,dbfolder);
}

void Database::destroy() {
  if (fs::exists(m_fullpath)) {
      fs::remove_all(m_fullpath);
  }
}

// Instance users functions

std::string Database::getDirectory() {
  return m_fullpath;
}

void Database::setKeyValue(std::string key,std::string value) {
  std::ofstream os;
  os.open(m_fullpath + "/" + key + "_string.kv", std::ios::out | std::ios::trunc);
  os << value;
  os.close();
}

std::string Database::getKeyValue(std::string key) {
  std::ifstream t(m_fullpath + "/" + key + "_string.kv");
  std::string value;
  //t >> value;
  //t.close();

  t.seekg(0, std::ios::end);
  value.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  value.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());

  return value;
}
