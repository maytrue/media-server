#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

using json = nlohmann::json;

namespace test {
struct Person {
  std::string name;
  std::string address;
  int age;
};

std::ostream& operator<<(std::ostream& os, const Person& p) {
  os << "name: " << p.name << ", address: " << p.address << ", age: " << p.age;
  return os;
}

void to_json(json& j, const Person& p) {
  j = json{{"name", p.name}, {"address", p.address}, {"age", p.age}};
}

void from_json(const json& j, Person& p) {
  j.at("name").get_to(p.name);
  j.at("address").get_to(p.address);
  j.at("age").get_to(p.age);
}
}  // namespace test

int main(int argc, char const* argv[]) {
  test::Person person{"Geogorge", "No. 1, Road 1, Beijing", 20};
  nlohmann::json j = person;
  std::cout << j << std::endl;

  auto p2 = j.template get<test::Person>();
  std::cout << p2 << std::endl;

  return 0;
}
